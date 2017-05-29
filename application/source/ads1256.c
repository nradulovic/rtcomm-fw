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

#define ADS_REG_STATUS                  0x00
#define ADS_REG_MUX                     0x01
#define ADS_REG_ADCON                   0x02
#define ADS_REG_DRATE                   0x03
#define ADS_REG_IO                      0x04
#define ADS_REG_OFC0                    0x05
#define ADS_REG_OFC1                    0x06
#define ADS_REG_OFC2                    0x07
#define ADS_REG_FSC0                    0x08
#define ADS_REG_FSC1                    0x09
#define ADS_REG_FSC2                    0x0a

#define ADS_STATUS_ORDER_LSB            (0x1u << 3)
#define ADS_STATUS_ACAL_ENABLED         (0x1u << 2)
#define ADS_STATUS_BUFEN_ENABLED        (0x1u << 1)

#define ADS_MUX(pos, neg)               ((uint8_t)((uint8_t)((pos & 0xfu) << 4u) | (uint8_t)(neg & 0xfu)))

#define ADS_ADCON_CLKOUT_FCLKIN         (0x1u << 5)
#define ADS_ADCON_CLKOUT_FCLKIN_DIV_2   (0x2u << 5)
#define ADS_ADCON_CLKOUT_FLCKIN_DIV_4   (0x3u << 5)
#define ADS_ADCON_SDCS_05UA             (0x1u << 3)
#define ADS_ADCON_SDCS_2UA              (0x2u << 3)
#define ADS_ADCON_SDCS_10UA             (0x3u << 3)
#define ADS_ADCON_PGA_2                 (0x1u << 0)
#define ADS_ADCON_PGA_4                 (0x2u << 0)
#define ADS_ADCON_PGA_8                 (0x3u << 0)
#define ADS_ADCON_PGA_16                (0x4u << 0)
#define ADS_ADCON_PGA_32                (0x5u << 0)
#define ADS_ADCON_PGA_64                (0x6u << 0)

#define ADS_DRATE_30KSPS                0xf0
#define ADS_DRATE_15KSPS                0xe0
#define ADS_DRATE_7500SPS               0xd0
#define ADS_DRATE_3750SPS               0xc0
#define ADS_DRATE_2000SPS               0xb0
#define ADS_DRATE_1000SPS               0xa1
#define ADS_DRATE_500SPS                0x92
#define ADS_DRATE_100SPS                0x82
#define ADS_DRATE_60SPS                 0x72
#define ADS_DRATE_50SPS                 0x63
#define ADS_DRATE_30SPS                 0x53
#define ADS_DRATE_25SPS                 0x43
#define ADS_DRATE_15SPS                 0x33
#define ADS_DRATE_10SPS                 0x23
#define ADS_DRATE_5SPS                  0x13
#define ADS_DRATE_2_5SPS                0x03

#define ADS_IO_DIR3                     0x80
#define ADS_IO_DIR2                     0x40
#define ADS_IO_DIR1                     0x20
#define ADS_IO_DIR0                     0x10

#define ADS_CMD_WAKEUP                  0x00
#define ADS_CMD_RDATA                   0x01
#define ADS_CMD_RDATAC                  0x03
#define ADS_CMD_SDATAC                  0x0f
#define ADS_CMD_RREG                    0x10
#define ADS_CMD_WREG                    0x50
#define ADS_CMD_SELFCAL                 0xf0
#define ADS_CMD_SELFOCAL                0xf1
#define ADS_CMD_SELFGCAL                0xf2
#define ADS_CMD_SYSOCAL                 0xf3
#define ADS_CMD_SYSGCAL                 0xf4
#define ADS_CMD_SYNC                    0xfc
#define ADS_CMD_STANDBY                 0xfd
#define ADS_CMD_RESET                   0xfe

#define ADS_CLOCK_SLAVE                 0x1u

#define ads_raw_to_int(raw_data)		n_ext_i24((int32_t)(raw_data))

/* Possible drivers state
 */
#define ADS_DRV_STATE_INIT				0u
#define ADS_DRV_STATE_CONFIG			1u
#define ADS_DRV_STATE_SAMPLING			2u
#define ADS_DRV_STATE_SUSPENDED			3u

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

 static
 void ads_wait_ready(struct ads1256_chip * chip)
 {
     volatile uint32_t           timeout;

     while (!chip->vt.drdy_is_active() && timeout--);
     while (chip->vt.drdy_is_active() && timeout--);
 }



 static
 void ads_reset_sync(struct ads1256_chip * chip)
 {
 	chip->l_buffer[0] = ADS_CMD_RESET;
 	chip->transfer.size     = 1;
 	chip->transfer.complete = NULL;
 	spi_write_sync(&chip->device, &chip->transfer);
 }



 static
 void ads_write_reg_sync(struct ads1256_chip * chip, uint32_t reg,
         uint8_t data)
 {
 	chip->l_buffer[0] = (uint8_t)(ADS_CMD_WREG | reg);
 	chip->l_buffer[1] = 0;
 	chip->l_buffer[2] = data;
 	chip->transfer.size     = 3;
 	chip->transfer.complete = NULL;
 	spi_write_sync(&chip->device, &chip->transfer);
 }



 static
 void ads_read_reg_sync(struct ads1256_chip * chip, uint32_t reg,
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



 static
 void ads_set_cmd_sync(struct ads1256_chip * chip, uint8_t cmd)
 {
     chip->l_buffer[0] = cmd;
     chip->transfer.size     = 1;
     chip->transfer.complete = NULL;
     delay_us(5);
     spi_write_sync(&chip->device, &chip->transfer);
     delay_us(5);
 }



 static
 void ads_read_data_sync(struct ads1256_chip * chip)
 {
 	spi_nss_force_active(&chip->device);
 	ads1256_set_cmd_sync(chip, ADS_CMD_RDATA);
 	delay_us(10);
 	chip->transfer.size     = 3;
 	chip->transfer.complete = NULL;
 	spi_nss_release_active(&chip->device);
 	spi_read_sync(&chip->device, &chip->transfer);
 }



 static
 void ads_rdc_start_sync(struct ads1256_chip * chip)
 {
     ads1256_set_cmd_sync(chip, ADS_CMD_RDATAC);
 }



 static
 void ads_rdc_start_read_async(struct ads1256_chip * chip,
         void (* complete)(void *), void * arg)
 {
     chip->transfer.size     = 3;
     chip->transfer.complete = complete;
     chip->transfer.arg      = arg;
     spi_read_async(&chip->device, &chip->transfer);
 }



 static
 void ads_rdc_stop_sync(struct ads1256_chip * chip)
 {
     ads1256_set_cmd_sync(chip, ADS_CMD_SDATAC);
 }


/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/


void ads1256_init(struct ads1256_chip * chip, struct spi_bus * bus,
		const struct ads1256_chip_vt * vt)
{
    spi_device_init(&chip->device, bus, vt->nss_activate, vt->nss_deactivate);
	chip->transfer.buff = chip->l_buffer;
	chip->state = ADS_DRV_STATE_INIT;
	memcpy(&chip->vt, vt, sizeof(*vt));
	memset(&chip->config, 0, sizeof(chip->config));
	memset(&chip->l_buffer, 0, sizeof(chip->l_buffer));
}



int ads1256_set_config(struct ads1256_chip * chip,
		const struct ads1256_config * config)
{
	if (chip->state == ADS_DRV_STATE_SAMPLING &&
		chip->config.sampling_mode == ADS1256_SAMPLE_MODE_CONT) {

		/* We are trying to do impossible with the ADC. Return error.
		 */
		return (1);
	}

	if ((chip->state == ADS_DRV_STATE_INIT) ||
	   ((chip->state == ADS_DRV_STATE_SAMPLING) &&
	    (chip->config.sampling_mode == ADS1256_SAMPLE_MODE_REQ))) {

	}
	return (0);
}



void ads1256_start_sampling(struct ads1256_chip * chip)
{

}



void ads1256_stop_sampling(struct ads1256_chip * chip)
{

}



void ads1256_drdy_isr(struct ads1256_chip * chip)
{

}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of ads1256.c
 ******************************************************************************/

