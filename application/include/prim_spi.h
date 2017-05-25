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
#include "neon_eds.h"

/*===============================================================  MACRO's  ==*/

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

struct spi_bus;

struct spi_config
{
    uint32_t                    prescaler;
    uint32_t                    flags;
};

struct spi_transfer
{
    uint8_t *                   buff;
	void					 (* complete)(void * arg);
	void *                      arg;
	uint16_t					size;
	uint32_t                    error;
};

struct spi_device
{
    struct spi_bus * 	        bus;
    uint32_t                    flags;
	void 					 (* cs_activate)(void);
	void 					 (* cs_deactivate)(void);
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct spi_bus g_spi1;
extern struct spi_bus g_spi2;
extern struct spi_bus g_spi3;
extern struct spi_bus g_spi4;
extern struct spi_bus g_spi5;

/*===================================================  FUNCTION PROTOTYPES  ==*/


void spi_bus_isr(struct spi_bus * bus);



nerror spi_bus_init(struct spi_bus * bus, const struct spi_config * config);



void spi_device_init(struct spi_device * device, struct spi_bus * bus,
        void (* cs_activate)(void), void (* cs_deactivate)(void));



void spi_write_async(struct spi_device * device, struct spi_transfer * transfer);



void spi_read_async(struct spi_device * device, struct spi_transfer * transfer);



void spi_write_sync(struct spi_device * device, struct spi_transfer * transfer);



void spi_read_sync(struct spi_device * device, struct spi_transfer * transfer);



void spi_nss_force_active(struct spi_device * device);



void spi_cs_force_inactive(struct spi_device * device);



void spi_nss_release_active(struct spi_device * device);

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of spi.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_PRIM_SPI_H_ */