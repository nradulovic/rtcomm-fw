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
#include <string.h>

#include "prim_spi.h"
#include "port/core.h"
#include "support.h"
#include "main.h"

/*=========================================================  LOCAL MACRO's  ==*/

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

/*======================================================  LOCAL DATA TYPES  ==*/

enum spi_state
{
    STATE_RX_CONT,
    STATE_TX_CONT,
    STATE_TX_WAIT,
    STATE_TX_COMPLETE,
    STATE_IDLE
};

struct spi_host
{
    SPI_TypeDef *               regs;
    size_t                      count;
    struct spi_client *         client;
    struct spi_transfer *       transfer;
    enum spi_state              state;
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/


PORT_C_INLINE
void spi_host_write_async(
        struct spi_host *       host,
        struct spi_client *     client,
        struct spi_transfer *   transfer);



PORT_C_INLINE
void spi_host_read_async(
        struct spi_host *       host,
        struct spi_client *     client,
        struct spi_transfer *   transfer);



static void spi_host_write_sync(
    struct spi_host *           host,
    struct spi_client *         client,
    struct spi_transfer *       transfer);



static void spi_host_read_sync(
    struct spi_host *           host,
    struct spi_client *         client,
    struct spi_transfer *       transfer);

/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

struct spi_host                 g_spi1 =
{
  SPI1,
	0,
	NULL,
	NULL,
	STATE_RX_CONT
};

struct spi_host                 g_spi2 =
{
  SPI2,
	0,
	NULL,
	NULL,
	STATE_RX_CONT
};

struct spi_host                 g_spi3 =
{
  SPI3,
	0,
	NULL,
	NULL,
	STATE_RX_CONT
};

struct spi_host                 g_spi4 =
{
  SPI4,
	0,
	NULL,
	NULL,
	STATE_RX_CONT
};

struct spi_host                 g_spi5 =
{
  SPI5,
	0,
	NULL,
	NULL,
	STATE_RX_CONT
};

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

#if defined(PORT_C_GCC)
static
__attribute__((naked))
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
#elif defined(PORT_C_ARMCC)
static __asm void delay_us(uint32_t __attribute__((unused)) delay)
{
    CBZ R0, end	
	MOV R1, #19
loop2
	MOV R2, R0
loop
	NOP
	SUB R2, R2, #1
	CMP R2, #0
	BNE loop
	SUB R1, R1, #1
	CMP R1, #0
	BNE loop2
end
	BX lr
}
#endif



void host_clear_overrun(struct spi_host * host);

void
host_clear_overrun(struct spi_host * host)
{
	ncore_dummy_rd(host->regs->DR);                                       		/* Clear status flag */
	ncore_dummy_rd(host->regs->SR);
}



PORT_C_INLINE
void spi_host_write_async(
    struct spi_host *           host,
    struct spi_client *         client,
    struct spi_transfer *       transfer)
{
    while (is_busy(host));

    /* Activate CS pin */
    if (!(transfer->flags & SPI_TRANSFER_CS_DISABLE)) {
        client->cs_activate();
    }
    host->client   = client;
    host->transfer = transfer;
    host->count    = 0;

    /* Clear any overrun errors */
    if (host->regs->SR & SPI_SR_OVR) {
    	host_clear_overrun(host);
    }

    /* Activate CS pin */

    if (transfer->size == 1) {
        host->state     = STATE_TX_COMPLETE;
        host->regs->DR  = transfer->buff[host->count++];
        host->regs->CR2 = SPI_CR2_RXNEIE;
    } else if (transfer->size == 2) {
        host->state     = STATE_TX_WAIT;
        host->regs->DR  = transfer->buff[host->count++];

        while (!is_tx_empty(host));

        host->regs->DR  = transfer->buff[host->count++];
        host->regs->CR2 = SPI_CR2_RXNEIE;
    } else {
        host->state     = STATE_TX_CONT;
        host->regs->DR  = transfer->buff[host->count++];

        while (!is_tx_empty(host));

        host->regs->DR  = transfer->buff[host->count++];
        host->regs->CR2 = SPI_CR2_TXEIE;
    }
}



PORT_C_INLINE
void spi_host_read_async(
    struct spi_host *           host,
    struct spi_client *         client,
    struct spi_transfer *       transfer)
{
    while (is_busy(host));

    /* Activate CS pin */
    if (!(transfer->flags & SPI_TRANSFER_CS_DISABLE)) {
        client->cs_activate();
    }

    host->client   = client;
    host->transfer = transfer;
    host->count    = 0;
    host->state    = STATE_RX_CONT;

    /* Clear any overrun errors */
    if (host->regs->SR & SPI_SR_OVR) {
    	host_clear_overrun(host);
    }

    /* Activate CS pin */
    host->regs->DR = 0;

    if (transfer->size > 1) {
        while (!is_tx_empty(host));

        host->regs->DR = 0;
    }
    host->regs->CR2 = SPI_CR2_RXNEIE | SPI_CR2_ERRIE;
}



PORT_C_INLINE
void spi_host_write_sync(
    struct spi_host *           host,
    struct spi_client *         client,
    struct spi_transfer *       transfer)
{
	uint32_t					tx_count;

    while (is_busy(host));

    host->client   = client;
    host->transfer = transfer;
    host->count    = 0;

    if (!(transfer->flags & SPI_TRANSFER_CS_DISABLE)) {
    	/* Activate CS pin */
        client->cs_activate();
    }
    tx_count = transfer->size;

    while (tx_count--) {

    	while (!(host->regs->SR & SPI_SR_TXE));
    	host->regs->DR = transfer->buff[host->count++];

		while (!(host->regs->SR & SPI_SR_RXNE));
		ncore_dummy_rd(host->regs->DR);

        if (host->regs->SR & SPI_SR_OVR) {
        	host_clear_overrun(host);
        	break;
        }
    }
    while (is_busy(host));

    /* Deactivate CS pin */
    if (!(transfer->flags & SPI_TRANSFER_CS_DISABLE)) {
        client->cs_deactivate();
    }

    if (transfer->complete) {
        transfer->complete(transfer);
    }
}



PORT_C_INLINE
void spi_host_read_sync(
    struct spi_host *           host,
    struct spi_client *         client,
    struct spi_transfer *       transfer)
{
	uint32_t					rx_count;

    while (is_busy(host));

    host->client   = client;
    host->transfer = transfer;
    host->count    = 0;

    if (!(transfer->flags & SPI_TRANSFER_CS_DISABLE)) {
        /* Activate CS pin */
        client->cs_activate();
    }
    rx_count = transfer->size;

    while (rx_count--) {

    	while (!(host->regs->SR & SPI_SR_TXE));
    	host->regs->DR = 0u;

		while (!(host->regs->SR & SPI_SR_RXNE));
		transfer->buff[host->count++] = (uint8_t)host->regs->DR;

        if (host->regs->SR & SPI_SR_OVR) {
        	host_clear_overrun(host);
        	break;
        }
	}
    while (is_busy(host));

    if (!(transfer->flags & SPI_TRANSFER_CS_DISABLE)) {
        /* Deactivate CS pin */
        client->cs_deactivate();
    }

    if (transfer->complete) {
        transfer->complete(transfer);
    }
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/


void spi_host_isr(struct spi_host * host)
{
    switch (host->state) {
        case STATE_RX_CONT: {
            ncore_dummy_rd(host->regs->SR);                                            /* Clear status flag */
            host->transfer->buff[host->count++] = (uint8_t)host->regs->DR;

            if ((host->transfer->size - 1u) > host->count) {
                while (!is_tx_empty(host));                                     /* Wait for two bit clocks */

                host->regs->DR = 0;
            } else if (host->transfer->size == host->count) {
                host->regs->CR2 = 0;

                if (!(host->transfer->flags & SPI_TRANSFER_CS_DISABLE)) {
                    /* Deactivate CS pin */
                	delay_us(1);
                    host->client->cs_deactivate();
                }
                host->transfer->complete(host->transfer);
            }
            break;
        }
        case STATE_TX_CONT: {
        	ncore_dummy_rd(host->regs->SR);                                            /* Clear status flag */
            host->regs->DR = host->transfer->buff[host->count++];

            if (host->transfer->size == host->count) {
            	ncore_dummy_rd(host->regs->DR);
                host->regs->CR2 = SPI_CR2_RXNEIE;
                host->state = STATE_TX_WAIT;
            }
            break;
        }
        case STATE_TX_WAIT: {
        	host_clear_overrun(host);
            host->state = STATE_TX_COMPLETE;
            break;
        }
        case STATE_TX_COMPLETE: {
        	host_clear_overrun(host);
            host->regs->CR2 = 0;

            if (!(host->transfer->flags & SPI_TRANSFER_CS_DISABLE)) {
				/* Deactivate CS pin */
            	delay_us(1);
				host->client->cs_deactivate();
			}
            host->transfer->complete(host->transfer);
            break;
        }
        default: {
            for (;;);
        }
    }
}



void spi_host_init(
    struct spi_host *           host,
    const struct spi_config *   config,
    void                     (* setup)(void))
{
    setup();

    host->regs->CR1     = 0;
    host->regs->CR2     = 0;
    host->regs->I2SCFGR = 0;

    switch (config->flags & SPI_TRANSFER) {
        case SPI_TRANSFER_RX:
            host->regs->CR1 |= SPI_CR1_RXONLY;
            break;
        case SPI_TRANSFER_TX:
        case SPI_TRANSFER_TX | SPI_TRANSFER_RX: {
            break;
        }
        default:
            return;
    }

    switch (config->flags & SPI_CLK_POL) {
        case SPI_CLK_POL_HIGH: {
            host->regs->CR1 |= SPI_CR1_CPOL;
            break;
        }
        case SPI_CLK_POL_LOW:
            break;
        default:
            return;
    }

    switch (config->flags & SPI_CLK_PHA) {
        case SPI_CLK_PHA_1EDGE:
            break;
        case SPI_CLK_PHA_2EDGE: {
            host->regs->CR1 |= SPI_CR1_CPHA;
            break;
        }
        default:
            return;
    }

    switch (config->flags & SPI_DATA) {
        case SPI_DATA_8BIT:
            break;
        case SPI_DATA_16BIT:
            host->regs->CR1 |= SPI_CR1_DFF;
            break;
        default:
            return;
    }

    switch (config->flags & SPI_SS) {
        case SPI_SS_HARDWARE:
            break;
        case SPI_SS_SOFTWARE:
            host->regs->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
            break;
        default:
            return;
    }

    switch (config->flags & SPI_MODE) {
        case SPI_MODE_MS:
            host->regs->CR1 |= SPI_CR1_MSTR;
            break;
        case SPI_MODE_SLV:
            break;
        default:
            return;
    }
    host->regs->CR1   |= config->prescaler & SPI_CR1_BR;
    host->regs->CRCPR  = 1;
    host->regs->CR1   |= SPI_CR1_SPE;
}



void spi_client_init(
    struct spi_client *         client,
    struct spi_host *           host,
    void                     (* cs_activate)(void),
    void                     (* cs_deactivate)(void))
{
	client->host     		= host;
	client->cs_activate 	= cs_activate;
	client->cs_deactivate	= cs_deactivate;
	cs_deactivate();
}



void spi_write_async(struct spi_client * client, struct spi_transfer * transfer)
{
    spi_host_write_async(client->host, client, transfer);
}



void spi_read_async(struct spi_client * client, struct spi_transfer * transfer)
{
    spi_host_read_async(client->host, client, transfer);
}



void spi_write_sync(struct spi_client * client, struct spi_transfer * transfer)
{
    spi_host_write_sync(client->host, client, transfer);
}



void spi_read_sync(struct spi_client * client, struct spi_transfer * transfer)
{
    spi_host_read_sync(client->host, client, transfer);
}



void spi_cs_force_low(struct spi_client * client)
{
    client->cs_activate();
}



void spi_cs_force_high(struct spi_client * client)
{
	delay_us(1);
    client->cs_deactivate();
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of spi.c
 ******************************************************************************/
