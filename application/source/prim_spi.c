/*
 *  teslameter_3mhx-fw - 2017
 *
 *  spi.c
 *
 *  Created on: May 29, 2017
 * ----------------------------------------------------------------------------
 *  This file is part of teslameter_3mhx-fw.
 *
 *  teslameter_3mhx-fw is free software: you can redistribute it and/or modify
 *  it under the terms of the Lesser GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  teslameter_3mhx-fw is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with teslameter_3mhx-fw.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------- *//**
 * @file
 * @author      Nenad Radulovic
 * @brief       SPI driver
 *********************************************************************//** @{ */

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

struct spi_bus
{
    SPI_TypeDef *               regs;
    size_t                      count;
    struct spi_device *         device;
    struct spi_transfer *       transfer;
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

static void
set_done(void * arg)
{
    volatile bool * wait_condition = arg;

    *wait_condition = true;
}



static
void bus_clear_overrun(struct spi_bus * host)
{
    ncore_dummy_rd(host->regs->DR);                                             /* Clear status flag */
    ncore_dummy_rd(host->regs->SR);
}



static
void isr_tx_only(struct spi_bus * bus)
{
	uint32_t                    itflag;
	struct spi_transfer *       transfer = bus->transfer;

	itflag = bus->regs->SR;

    if (itflag & SPI_SR_RXNE) {
    	ncore_dummy_rd(bus->regs->DR);
    	bus->count++;

        if (bus->count != transfer->size) {
            bus->regs->DR = transfer->buff[bus->count];
        }
    }

	if (itflag & (SPI_SR_FRE | SPI_SR_OVR | SPI_SR_MODF)) {
		if (itflag & SPI_SR_OVR) {
			bus_clear_overrun(bus);
			transfer->error = NERROR_DEVICE_FAIL;
		} else {
			/*
			 * TODO: clear other errors
			 */
			transfer->error = NERROR_DEVICE_NO_COMM;
		}
	}

    if (bus->count == transfer->size) {
        if (!(bus->device->flags & SPI_TRANSFER_CS_DISABLE)) {
            /* Deactivate CS pin */
            bus->device->cs_deactivate();
        }
        bus->regs->CR2 &= ~(SPI_CR2_RXNEIE | SPI_CR2_ERRIE);
        transfer->complete(transfer->arg);
    }
}



static
void isr_rx_only(struct spi_bus * bus)
{
    struct spi_transfer *       transfer = bus->transfer;
    uint32_t                    itflag;

    itflag = bus->regs->SR;

    if (itflag & SPI_SR_RXNE) {
        transfer->buff[bus->count++] = (uint8_t)bus->regs->DR;

        if (bus->count != transfer->size) {
            bus->regs->DR = 0u;
        }
    }

    if (itflag & (SPI_SR_FRE | SPI_SR_OVR | SPI_SR_MODF)) {
        if (itflag & SPI_SR_OVR) {
            bus_clear_overrun(bus);
            transfer->error = NERROR_DEVICE_FAIL;
        } else {
            /*
             * TODO: clear other errors
             */
            transfer->error = NERROR_DEVICE_NO_COMM;
        }
    }

    if (bus->count == transfer->size) {
        if (!(bus->device->flags & SPI_TRANSFER_CS_DISABLE)) {
            /* Deactivate CS pin */
            bus->device->cs_deactivate();
        }
        bus->regs->CR2 &= ~(SPI_CR2_RXNEIE | SPI_CR2_ERRIE);
        transfer->complete(transfer->arg);
    }
}



PORT_C_INLINE
void spi_host_write_async(struct spi_bus * bus, struct spi_device * device,
    struct spi_transfer * transfer)
{
    while (is_busy(bus));

    bus->device = device;
    bus->count = 0u;
    bus->isr_handler = isr_tx_only;
    bus->transfer = transfer;
    bus->transfer->error = NERROR_NONE;

    /* Activate CS pin */
    if (!(device->flags & SPI_TRANSFER_CS_DISABLE)) {
        device->cs_activate();
    }
    bus->regs->DR = transfer->buff[0];
    bus->regs->CR2 |= SPI_CR2_RXNEIE | SPI_CR2_ERRIE;
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
    bus->device = device;
    bus->count = 0u;
    bus->isr_handler = isr_rx_only;
    bus->transfer = transfer;
    bus->transfer->error = NERROR_NONE;

    bus->regs->DR = 0u;
    bus->regs->CR2 |= SPI_CR2_RXNEIE | SPI_CR2_ERRIE;
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
    device->bus             = bus;
    device->flags           = 0;
    device->cs_activate     = cs_activate;
    device->cs_deactivate   = cs_deactivate;
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
