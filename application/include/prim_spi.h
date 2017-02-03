/*
 * spi.h
 *
 *  Created on: Jan 19, 2015
 *      Author: nenad
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       SPI Driver include header
 * @defgroup    spi SPI driver Interface
 * @brief       Interface
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_PRIM_SPI_H_
#define APPLICATION_INCLUDE_PRIM_SPI_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stddef.h>
#include <stdbool.h>

#include "stm32f4xx.h"

/*===============================================================  MACRO's  ==*/

#define CONFIG_TRANSFER_BUFF_SIZE		8

#define SPI_TRANSFER_RX			        (0x1u << 0 )
#define SPI_TRANSFER_TX			        (0x1u << 1 )
#define SPI_CLK_POL_LOW                 (0x1u << 2 )
#define SPI_CLK_POL_HIGH                (0x1u << 3 )
#define SPI_CLK_PHA_1EDGE               (0x1u << 4 )
#define SPI_CLK_PHA_2EDGE               (0x1u << 5 )
#define SPI_DATA_8BIT                   (0x1u << 6 )
#define SPI_DATA_16BIT                  (0x1u << 7 )
#define SPI_SS_SOFTWARE                 (0x1u << 8 )
#define SPI_SS_HARDWARE                 (0x1u << 9 )
#define SPI_MODE_MS                     (0x1u << 10)
#define SPI_MODE_SLV                    (0x1u << 11)

#define SPI_TRANSFER_CS_DISABLE         (0x1u << 0)

/*============================================================  DATA TYPES  ==*/

struct spi_host;

struct spi_config
{
    uint32_t                    prescaler;
    uint32_t                    flags;
};

struct spi_transfer
{
    uint8_t                     buff[CONFIG_TRANSFER_BUFF_SIZE];
	void					 (* complete)(struct spi_transfer *);
	uint16_t					size;
	uint32_t                    flags;
};

struct spi_client
{
    struct spi_host * 	        host;
	void 					 (* cs_activate)(void);
	void 					 (* cs_deactivate)(void);
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct spi_host g_spi1;
extern struct spi_host g_spi2;
extern struct spi_host g_spi3;
extern struct spi_host g_spi4;
extern struct spi_host g_spi5;

/*===================================================  FUNCTION PROTOTYPES  ==*/


void spi_host_isr(struct spi_host * host);



void spi_host_init(
    struct spi_host *           host,
    const struct spi_config *   config,
    void                     (* setup)(void));



void spi_client_init(
    struct spi_client *         client,
    struct spi_host *           host,
    void                     (* cs_activate)(void),
    void                     (* cs_deactivate)(void));




void spi_write_async(struct spi_client * client, struct spi_transfer * transfer);



void spi_read_async(struct spi_client * client, struct spi_transfer * transfer);



void spi_write_sync(struct spi_client * client, struct spi_transfer * transfer);



void spi_read_sync(struct spi_client * client, struct spi_transfer * transfer);



void spi_cs_force_low(struct spi_client * client);



void spi_cs_force_high(struct spi_client * client);

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of spi.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_PRIM_SPI_H_ */
