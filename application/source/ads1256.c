/*
 * ads1256.c
 *
 *  Created on: Jan 19, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       ADS1256 driver
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include <string.h>

#include "prim_spi.h"
#include "ads1256.h"
#include "port/compiler.h"
#include "cdi/io.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static __attribute__((naked))
void delay_us(uint32_t __attribute__((unused)) delay)
{
	__asm__ __volatile__(
	"	 CBZ R0, end				\n"
	"    MOV R1, #19				\n"
	"loop2:						\n"
	"    MOV R2, R0					\n"
    "loop:						\n"
	"    NOP						\n"
	"    SUB R2, R2, #1				\n"
	"    CMP R2, #0					\n"
	"    BNE loop					\n"
	"    SUB R1, R1, #1				\n"
	"    CMP R1, #0					\n"
	"    BNE loop2					\n"
	"end:						\n"
	"    BX lr						\n"
	:
	:
	: "r0", "r1", "r2");
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/


void ads1256_init(struct ads1256_chip * chip, struct spi_bus * bus,
		const struct ads1256_chip_vt * vt)
{
    spi_device_init(&chip->device, bus, vt->nss_activate, vt->nss_deactivate);
	chip->transfer.buff = chip->l_buffer;
	memcpy(&chip->vt, vt, sizeof(*vt));
}



void ads1256_wait_ready(struct ads1256_chip * chip)
{
    volatile uint32_t           timeout;

    while (!chip->vt.drdy_is_active() && timeout--);
    while (chip->vt.drdy_is_active() && timeout--);
}


void ads1256_reset_sync(struct ads1256_chip * chip)
{
	chip->l_buffer[0] = ADS_CMD_RESET;
	chip->transfer.size     = 1;
	chip->transfer.complete = NULL;
	spi_write_sync(&chip->device, &chip->transfer);
}



void ads1256_write_reg_sync(struct ads1256_chip * chip, uint32_t reg,
        uint8_t data)
{
	chip->l_buffer[0] = (uint8_t)(ADS_CMD_WREG | reg);
	chip->l_buffer[1] = 0;
	chip->l_buffer[2] = data;
	chip->transfer.size     = 3;
	chip->transfer.complete = NULL;
	spi_write_sync(&chip->device, &chip->transfer);
}



void ads1256_read_reg_sync(struct ads1256_chip * chip, uint32_t reg,
        uint8_t * data)
{
	chip->l_buffer[0] = (uint8_t)(ADS_CMD_RREG | reg);
	chip->l_buffer[1] = 0;
	chip->transfer.size     = 2;
	chip->transfer.complete = NULL;
	spi_nss_force_active(&chip->device);
	spi_write_sync(&chip->device, &chip->transfer);
	delay_us(25);
	chip->transfer.size     = 1;
	spi_nss_release_active(&chip->device);
	spi_read_sync(&chip->device, &chip->transfer);
	*data = chip->l_buffer[0];
}



void ads1256_set_cmd_sync(struct ads1256_chip * chip, uint8_t cmd)
{
    chip->l_buffer[0] = cmd;
    chip->transfer.size     = 1;
    chip->transfer.complete = NULL;
    delay_us(5);
    spi_write_sync(&chip->device, &chip->transfer);
    delay_us(5);
}


void ads1256_read_data_sync(struct ads1256_chip * chip)
{
	spi_nss_force_active(&chip->device);
	ads1256_set_cmd_sync(chip, ADS_CMD_RDATA);
	delay_us(10);
	chip->transfer.size     = 3;
	chip->transfer.complete = NULL;
	spi_nss_release_active(&chip->device);
	spi_read_sync(&chip->device, &chip->transfer);
}



void ads1256_rdc_start_sync(struct ads1256_chip * chip)
{
    ads1256_set_cmd_sync(chip, ADS_CMD_RDATAC);
}



void ads1256_rdc_start_read_async(struct ads1256_chip * chip,
        void (* complete)(void *), void * arg)
{
    chip->transfer.size     = 3;
    chip->transfer.complete = complete;
    chip->transfer.arg      = arg;
    spi_read_async(&chip->device, &chip->transfer);
}

void ads1256_rdc_stop_sync(struct ads1256_chip * chip)
{
    ads1256_set_cmd_sync(chip, ADS_CMD_SDATAC);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of ads1256.c
 ******************************************************************************/

