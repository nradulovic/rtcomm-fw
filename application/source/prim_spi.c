/*
 * spi.c
 *
 *  Created on: Jan 19, 2015
 *      Author: Nenad Radulovic
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       SPI driver implementation
 * @addtogroup  spi
 *********************************************************************//** @{ */
/**@defgroup    spi_impl Implementation
 * @brief       SPI driver implementation
 * @{ *//*--------------------------------------------------------------------*/

/*=========================================================  INCLUDE FILES  ==*/

#include <stddef.h>

#include "prim_spi.h"
#include "neon_eds.h"
#include "status.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define SPI_TRANSFER_IS_VALID(value)                                            \
    ((value) & SPI_TRANSFER)
#define SPI_CLK_POL_IS_VALID(value)                                             \
    ((((value) & SPI_CLK_POL) == SPI_CLK_POL_HIGH) ||                           \
            (((value) & SPI_CLK_POL) == SPI_CLK_POL_LOW))
#define SPI_CLK_PHA_IS_VALID(value)                                             \
    ((((value) & SPI_CLK_PHA) == SPI_CLK_PHA_1EDGE) ||                          \
            (((value) & SPI_CLK_PHA) == SPI_CLK_PHA_2EDGE))
#define SPI_DATA_IS_VALID(value)                                                \
    ((((value) & SPI_DATA) == SPI_DATA_8BIT) ||                                 \
            (((value) & SPI_DATA) == SPI_DATA_16BIT))
#define SPI_SS_IS_VALID(value)                                                  \
    ((((value) & SPI_SS) == SPI_SS_SOFTWARE) ||                                 \
            (((value) & SPI_SS) == SPI_SS_HARDWARE))
#define SPI_MODE_IS_VALID(value)                                                \
    ((((value) & SPI_MODE) == SPI_MODE_MS) ||                                   \
            (((value) & SPI_MODE) == SPI_MODE_SLV))

#define SPI_TRANSFER                    (SPI_TRANSFER_RX   | SPI_TRANSFER_TX)
#define SPI_CLK_POL                     (SPI_CLK_POL_HIGH  | SPI_CLK_POL_LOW)
#define SPI_CLK_PHA                     (SPI_CLK_PHA_1EDGE | SPI_CLK_PHA_2EDGE)
#define SPI_DATA                        (SPI_DATA_8BIT     | SPI_DATA_16BIT)
#define SPI_SS                          (SPI_SS_SOFTWARE   | SPI_SS_HARDWARE)
#define SPI_MODE                        (SPI_MODE_MS       | SPI_MODE_SLV)

#define is_datasize_8bit(host)          (!((host)->regs->CR1 & SPI_CR1_DFF))
#define is_tx_empty(host)               (  (host)->regs->SR  & SPI_SR_TXE)
#define is_rx_empty(host)               (!((host)->regs->SR  & SPI_SR_RXNE))
#define is_busy(host)                   (  (host)->regs->SR  & SPI_SR_BSY)

#define TRANSFER_IS_READING     true

/*======================================================  LOCAL DATA TYPES  ==*/

enum spi_state
{
    STATE_RX_CONT,
    STATE_TX_CONT,
    STATE_TX_WAIT,
    STATE_TX_COMPLETE,
    STATE_IDLE
};

struct spi_bus
{
    SPI_TypeDef *               regs;
    size_t                      count;
    struct spi_device *         device;
    struct spi_transfer *       transfer;
    enum spi_state              state;
    void                     (* isr_handler)(struct spi_bus * bus);
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static
void bus_clear_overrun(struct spi_bus * host);

PORT_C_INLINE
void spi_host_write_async(struct spi_bus * bus, struct spi_device * device,
        struct spi_transfer * transfer);



PORT_C_INLINE
void spi_host_read_async(struct spi_bus * bus, struct spi_device * device,
        struct spi_transfer * transfer);

/*=======================================================  LOCAL VARIABLES  ==*/

static const NCOMPONENT_DEFINE("primitive SPI");

/*======================================================  GLOBAL VARIABLES  ==*/

struct spi_bus                 g_spi1 =
{
    .regs = SPI1,
};

struct spi_bus                 g_spi2 =
{
    .regs = SPI2,
};

struct spi_bus                 g_spi3 =
{
    .regs = SPI3,
};

struct spi_bus                 g_spi4 =
{
    .regs = SPI4,
};

struct spi_bus                 g_spi5 =
{
    .regs = SPI5,
};

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static __attribute__((naked))
void delay_us(uint32_t __attribute__((unused)) delay)
{
	__asm__ __volatile__(
		"	 CBZ R0, end				\n"
		"    MOV R1, #20				\n"
		"loop2:							\n"
		"    MOV R2, R0					\n"
		"loop:							\n"
		"    NOP						\n"
		"    SUB R2, R2, #1				\n"
		"    CMP R2, #0					\n"
		"    BNE loop					\n"
		"    SUB R1, R1, #1				\n"
		"    CMP R1, #0					\n"
		"    BNE loop2					\n"
		"end:							\n"
		"    BX lr						\n"
        :
        :
        : "r0", "r1", "r2");
}



static void
set_done(void * arg)
{
    volatile bool * wait_condition = arg;

    *wait_condition = true;
}



static
void bus_clear_overrun(struct spi_bus * host)
{
	ncore_dummy_rd(host->regs->DR);                                       		/* Clear status flag */
	ncore_dummy_rd(host->regs->SR);
}



static
void isr_tx_only(struct spi_bus * bus)
{
    if (bus->regs->SR & SPI_SR_OVR) {
        bus->transfer->error = NERROR_DEVICE_FAIL;
        bus_clear_overrun(bus);
    }

    switch (bus->state) {
        case STATE_TX_CONT: {
            bus->regs->DR = bus->transfer->buff[bus->count++];

            if (bus->transfer->size == bus->count) {
                ncore_dummy_rd(bus->regs->DR);
                bus->regs->CR2 = SPI_CR2_RXNEIE;
                bus->state = STATE_TX_WAIT;
            }
            break;
        }
        case STATE_TX_WAIT: {
            bus->state = STATE_TX_COMPLETE;
            break;
        }
        case STATE_TX_COMPLETE: {
            bus->regs->CR2 = 0;

            if (!(bus->device->flags & SPI_TRANSFER_CS_DISABLE)) {
                /* Deactivate CS pin */
                delay_us(1);
                bus->device->cs_deactivate();
            }
            bus->transfer->complete(bus->transfer->arg);
            break;
        }
        default: {
            status_panic(STATUS_RUNTIME_CHECK_FAILED);
        }
    }
}



static
void isr_rx_only(struct spi_bus * bus)
{
    struct spi_transfer * transfer = bus->transfer;

    if (bus->regs->SR & SPI_SR_OVR) {
        transfer->error = NERROR_DEVICE_FAIL;
        bus_clear_overrun(bus);
    }
    transfer->buff[bus->count++] = (uint8_t)bus->regs->DR;

    if ((transfer->size - 1u) > bus->count) {
        while (!is_tx_empty(bus));                                              /* Wait for two bit clocks */

        bus->regs->DR = 0;
    } else if (transfer->size == bus->count) {
        bus->regs->CR2 = 0;

        if (!(bus->device->flags & SPI_TRANSFER_CS_DISABLE)) {
            /* Deactivate CS pin */
            delay_us(1);
            bus->device->cs_deactivate();
        }
        transfer->complete(transfer->arg);
    }
}



PORT_C_INLINE
void spi_host_write_async(struct spi_bus * bus, struct spi_device * device,
    struct spi_transfer * transfer)
{
    while (is_busy(bus));

    /* Activate CS pin */
    if (!(device->flags & SPI_TRANSFER_CS_DISABLE)) {
        device->cs_activate();
    }
    bus->device   = device;
    bus->transfer = transfer;
    bus->count    = 0;
    bus->transfer->error = NERROR_NONE;
    bus->isr_handler = isr_tx_only;

    /* Clear any overrun errors */
    if (bus->regs->SR & SPI_SR_OVR) {
    	bus_clear_overrun(bus);
    }

    if (transfer->size == 1) {
        bus->state     = STATE_TX_COMPLETE;
        bus->regs->DR  = transfer->buff[bus->count++];
        bus->regs->CR2 = SPI_CR2_RXNEIE;
    } else if (transfer->size == 2) {
        bus->state     = STATE_TX_WAIT;
        bus->regs->DR  = transfer->buff[bus->count++];

        while (!is_tx_empty(bus));

        bus->regs->DR  = transfer->buff[bus->count++];
        bus->regs->CR2 = SPI_CR2_RXNEIE;
    } else {
        bus->state     = STATE_TX_CONT;
        bus->regs->DR  = transfer->buff[bus->count++];

        while (!is_tx_empty(bus));

        bus->regs->DR  = transfer->buff[bus->count++];
        bus->regs->CR2 = SPI_CR2_TXEIE;
    }
}



PORT_C_INLINE
void spi_host_read_async(struct spi_bus * bus, struct spi_device * device,
    struct spi_transfer * transfer)
{
    while (is_busy(bus));

    /* Activate CS pin */
    if (!(device->flags & SPI_TRANSFER_CS_DISABLE)) {
        device->cs_activate();
    }

    bus->device   = device;
    bus->transfer = transfer;
    bus->count    = 0;
    bus->transfer->error = NERROR_NONE;
    bus->isr_handler = isr_rx_only;

    /* Clear any overrun errors */
    if (bus->regs->SR & SPI_SR_OVR) {
    	bus_clear_overrun(bus);
    }
    bus->regs->DR = 0;

    if (transfer->size > 1) {
        while (!is_tx_empty(bus));

        bus->regs->DR = 0;
    }
    bus->regs->CR2 = SPI_CR2_RXNEIE | SPI_CR2_ERRIE;
}



static
void spi_transfer_sync(struct spi_device * device,
        struct spi_transfer * transfer, bool is_reading)
{
    volatile bool               is_done = false;
    void                     (* old_complete)(void *);
    void                      * old_arg;

    old_complete = transfer->complete;
    old_arg = transfer->arg;

    transfer->complete = set_done;
    transfer->arg = (void *)&is_done;

    if (is_reading) {
        spi_host_read_async(device->bus, device, transfer);
    } else {
        spi_host_write_async(device->bus, device, transfer);
    }

    while (!is_done) {
        ncore_idle();
    }

    if (old_complete) {
        old_complete(old_arg);
    }
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/


void spi_bus_isr(struct spi_bus * bus)
{
    bus->isr_handler(bus);
}



nerror spi_bus_init(struct spi_bus * bus, const struct spi_config * config)
{
    bus->regs->CR1     = 0;
    bus->regs->CR2     = 0;
    bus->regs->I2SCFGR = 0;

    NREQUIRE("SPI_TRANSFER flags are invalid",
            SPI_TRANSFER_IS_VALID(config->flags));
    NREQUIRE("SPI_CLK_POL flags are invalid",
            SPI_CLK_POL_IS_VALID(config->flags));
    NREQUIRE("SPI_CLK_PHA flags are invalid",
            SPI_CLK_PHA_IS_VALID(config->flags));
    NREQUIRE("SPI_DATA flags are invalid",
            SPI_DATA_IS_VALID(config->flags));
    NREQUIRE("SPI_SS flags are invalid",
            SPI_SS_IS_VALID(config->flags));
    NREQUIRE("SPI_MODE flags are invalid",
            SPI_MODE_IS_VALID(config->flags));

    switch (config->flags & SPI_TRANSFER) {
        case SPI_TRANSFER_RX:
            bus->regs->CR1 |= SPI_CR1_RXONLY;
            break;
        case SPI_TRANSFER_TX:
        case SPI_TRANSFER_TX | SPI_TRANSFER_RX: {
            break;
        }
        default:
            return (NERROR_ARG_INVALID);
    }

    switch (config->flags & SPI_CLK_POL) {
        case SPI_CLK_POL_HIGH: {
            bus->regs->CR1 |= SPI_CR1_CPOL;
            break;
        }
        case SPI_CLK_POL_LOW:
            break;
        default:
            return (NERROR_ARG_INVALID);
    }

    switch (config->flags & SPI_CLK_PHA) {
        case SPI_CLK_PHA_1EDGE:
            break;
        case SPI_CLK_PHA_2EDGE: {
            bus->regs->CR1 |= SPI_CR1_CPHA;
            break;
        }
        default:
            return (NERROR_ARG_INVALID);
    }

    switch (config->flags & SPI_DATA) {
        case SPI_DATA_8BIT:
            break;
        case SPI_DATA_16BIT:
            bus->regs->CR1 |= SPI_CR1_DFF;
            break;
        default:
            return (NERROR_ARG_INVALID);
    }

    switch (config->flags & SPI_SS) {
        case SPI_SS_HARDWARE:
            break;
        case SPI_SS_SOFTWARE:
            bus->regs->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
            break;
        default:
            return (NERROR_ARG_INVALID);
    }

    switch (config->flags & SPI_MODE) {
        case SPI_MODE_MS:
            bus->regs->CR1 |= SPI_CR1_MSTR;
            break;
        case SPI_MODE_SLV:
            break;
        default:
            return (NERROR_ARG_INVALID);
    }
    bus->regs->CR1   |= config->prescaler & SPI_CR1_BR;
    bus->regs->CRCPR  = 1;
    bus->regs->CR1   |= SPI_CR1_SPE;

    return (NERROR_NONE);
}



void spi_device_init(struct spi_device * device, struct spi_bus * bus,
        void (* cs_activate)(void), void (* cs_deactivate)(void))
{
	device->bus     		= bus;
	device->flags           = 0;
	device->cs_activate 	= cs_activate;
	device->cs_deactivate	= cs_deactivate;
	device->cs_deactivate();
}



void spi_write_async(struct spi_device * device, struct spi_transfer * transfer)
{
    spi_host_write_async(device->bus, device, transfer);
}



void spi_read_async(struct spi_device * device, struct spi_transfer * transfer)
{
    spi_host_read_async(device->bus, device, transfer);
}



void spi_write_sync(struct spi_device * device, struct spi_transfer * transfer)
{
    spi_transfer_sync(device, transfer, !TRANSFER_IS_READING);
}



void spi_read_sync(struct spi_device * device, struct spi_transfer * transfer)
{
    spi_transfer_sync(device, transfer, TRANSFER_IS_READING);
}



void spi_nss_force_active(struct spi_device * device)
{
    device->flags |= SPI_TRANSFER_CS_DISABLE;
    device->cs_activate();
}



void spi_cs_force_inactive(struct spi_device * device)
{
    while (is_busy(device->bus));
    device->flags &= ~SPI_TRANSFER_CS_DISABLE;
    device->cs_deactivate();
}



void spi_nss_release_active(struct spi_device * device)
{
    device->flags &= ~SPI_TRANSFER_CS_DISABLE;
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of spi.c
 ******************************************************************************/
