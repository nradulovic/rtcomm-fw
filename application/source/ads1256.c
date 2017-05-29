/*
 * ads1256.c
 *
 *  Created on: May 29, 2017
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       ADS1256 driver
 *********************************************************************//** @{ */
/*
 *
 */

/*=========================================================  INCLUDE FILES  ==*/

#include <string.h>

#include "prim_spi.h"
#include "ads1256.h"

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

#define ADS_MUX(pos, neg)               \
	((uint8_t)((uint8_t)((pos & 0xfu) << 4u) | (uint8_t)(neg & 0xfu)))

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

#define ADS_READ_WRITE_DELAY			24u

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



static uint8_t map_sample_rate_to_reg(uint32_t speed_int)
{
	switch (speed_int) {
		case 30000u : return ADS_DRATE_30KSPS;
		case 15000u : return ADS_DRATE_15KSPS;
		case 7500u  : return ADS_DRATE_7500SPS;
		case 3750u  : return ADS_DRATE_3750SPS;
		case 2000u	: return ADS_DRATE_2000SPS;
		case 1000u	: return ADS_DRATE_1000SPS;
		case 500u	: return ADS_DRATE_500SPS;
		case 100u	: return ADS_DRATE_100SPS;
		case 60u	: return ADS_DRATE_60SPS;
		case 50u    : return ADS_DRATE_50SPS;
		case 25u    : return ADS_DRATE_25SPS;
		case 15u    : return ADS_DRATE_15SPS;
		case 10u    : return ADS_DRATE_10SPS;
		case 5u     : return ADS_DRATE_5SPS;
		case 2		: return ADS_DRATE_2_5SPS;
		default     : return (0);
	}
}



static void ads_chip_spi_write(struct ads1256_chip * chip)
{
	delay_us(ADS_READ_WRITE_DELAY);
	spi_write_sync(&chip->device, &chip->transfer);
	delay_us(ADS_READ_WRITE_DELAY);
}



static void ads_chip_spi_read(struct ads1256_chip * chip)
{
	delay_us(ADS_READ_WRITE_DELAY);
	spi_read_sync(&chip->device, &chip->transfer);
	delay_us(ADS_READ_WRITE_DELAY);
}



static void ads_chip_write_reg_sync(struct ads1256_chip * chip, uint32_t reg,
         uint8_t data)
{
 	chip->l_buffer[0] = (uint8_t)(ADS_CMD_WREG | reg);
 	chip->l_buffer[1] = 0;
 	chip->l_buffer[2] = data;
 	chip->transfer.size     = 3;
 	chip->transfer.complete = NULL;
 	ads_chip_spi_write(chip);
}



static void ads_chip_set_cmd_sync(struct ads1256_chip * chip, uint8_t cmd)
{
    chip->l_buffer[0] 		 = cmd;
    chip->transfer.size     = 1;
    chip->transfer.complete = NULL;
    ads_chip_spi_write(chip);
}



static void ads_chip_read_data_sync(struct ads1256_chip * chip)
{
	spi_nss_force_active(&chip->device);
 	ads_chip_set_cmd_sync(chip, ADS_CMD_RDATA);
 	chip->transfer.size     = 3;
 	chip->transfer.complete = NULL;
 	spi_nss_release_active(&chip->device);
 	ads_chip_spi_read(chip);
 	chip->vt->reader(chip->vt->reader_arg);
}



static void ads_chip_read_data_async(struct ads1256_chip * chip)
{
	chip->transfer.size     = 3;
	chip->transfer.complete = chip->vt->reader;
	chip->transfer.arg      = chip->vt->reader_arg;
	spi_read_async(&chip->device, &chip->transfer);
}



static int ads_chip_set_config(struct ads1256_chip * chip,
		const struct ads1256_chip_config * config)
{
	uint8_t					reg_val;

	/* Setup MUX */
	reg_val = (uint8_t)(((config->mux_hi & 0x0fu) << 4u) |
			             (config->mux_lo & 0x0fu));
	ads_chip_write_reg_sync(chip, ADS_REG_MUX, reg_val);

	/* Setup OSC */
	reg_val = config->enable_ext_osc ? ADS_ADCON_CLKOUT_FCLKIN : 0u;
	ads_chip_write_reg_sync(chip, ADS_REG_ADCON, reg_val);

	/* Setup GPIO */
	ads_chip_write_reg_sync(chip, ADS_REG_IO, config->gpio);

	/* Setup buffer */
	reg_val = config->enable_buffer ? ADS_STATUS_BUFEN_ENABLED : 0u;
	ads_chip_write_reg_sync(chip, ADS_REG_STATUS, reg_val);

	memcpy(&chip->config, config, sizeof(chip->config));

	return (0);
}



static void ads_chip_sampling_prepare(struct ads1256_chip * chip)
{
	if (chip->group->config.sampling_mode == ADS1256_SAMPLE_MODE_CONT) {
		/* Continuous mode */
		ads_chip_set_cmd_sync(chip, ADS_CMD_RDATAC);
	}
}



static void ads_chip_sampling_enable(struct ads1256_chip * chip)
{
	if (chip->group->config.sampling_mode == ADS1256_SAMPLE_MODE_CONT) {
		/* Continuous mode */
		if (chip->config.is_master) {
			chip->vt->drdy_isr_enable();
		}
	} else {
		/* Read by command */
		chip->vt->drdy_isr_enable();
	}
}

static void ads_chip_sampling_disable(struct ads1256_chip * chip)
{
	chip->vt->drdy_isr_disable();
}

static void ads_chip_sampling_unprepare(struct ads1256_chip * chip)
{
	if (chip->group->config.sampling_mode == ADS1256_SAMPLE_MODE_CONT) {
		/* Continuous mode */
		ads_chip_set_cmd_sync(chip, ADS_CMD_SDATAC);
	}
}

static void ads_group_power_activate(struct ads1256_group * group)
{
	if (group->vt->power_activate) {
		group->vt->power_activate();
	}
}

static void ads_group_power_deactivate(struct ads1256_group * group)
{
	if (group->vt->power_deactivate) {
		group->vt->power_deactivate();
	}
}



static int ads_group_set_config(struct ads1256_group * group,
		const struct ads1256_group_config * config)
{
	struct ads1256_chip * 		chip;

	for (chip = group->chips; chip != NULL; chip = chip->next) {
		uint8_t					reg_val;

		/* Setup sampling frequency */
		reg_val = map_sample_rate_to_reg(config->sampling_rate);

		if (reg_val == 0) {
			return (-1);
		}
		ads_chip_write_reg_sync(chip, ADS_REG_DRATE, reg_val);
	}
	memcpy(&group->config, config, sizeof(group->config));

	return (0);
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/


void ads1256_init_chip(struct ads1256_chip * chip, struct spi_bus * bus,
		const struct ads1256_chip_vt * vt)
{
	memset(chip, 0, sizeof(*chip));
    spi_device_init(&chip->device, bus, vt->nss_activate, vt->nss_deactivate);
	chip->transfer.buff = chip->l_buffer;
	chip->vt = vt;
}



void ads1256_group_init(struct ads1256_group * group,
		const struct ads1256_group_vt * vt)
{
	memset(group, 0, sizeof(*group));
	group->vt = vt;
	group->state = ADS_DRV_STATE_INIT;
}



int ads1256_group_add_chip(struct ads1256_group * group,
		struct ads1256_chip * chip)
{
	if (group->chips == NULL) {
		group->chips = chip;
	} else {
		struct ads1256_chip * current;

		current = group->chips;

		while (current->next != NULL) {
			current = current->next;
		}
		current->next = chip;
	}

	return (0);
}



int ads1256_set_per_chip_config(struct ads1256_chip * chip,
		const struct ads1256_chip_config * config)
{
	int							retval = -1;
	struct ads1256_group * 		group;

	if (chip->group == NULL) {
		return (-1);
	}
	group = chip->group;

	switch (group->state) {
		case ADS_DRV_STATE_INIT: {
			break;
		}
		case ADS_DRV_STATE_CONFIG: {
			retval = ads_chip_set_config(chip, config);
			break;
		}
		case ADS_DRV_STATE_SAMPLING: {
			retval = ads1256_stop_sampling(chip->group);

			if (retval != 0) {
				break;
			}
			retval = ads_chip_set_config(chip, config);
			break;
		}
	default:
		break;
	}

	if (retval != 0) {
		group->state = ADS_DRV_STATE_INIT;
	}

	return (retval);
}



int ads1256_set_group_config(struct ads1256_group * group,
		const struct ads1256_group_config * config)
{
	int							retval = -1;

	switch (group->state) {
		case ADS_DRV_STATE_INIT: {
			struct ads1256_chip * chip;

			ads_group_power_activate(group);
			/* Wait for power-on cycle */
			HAL_Delay(10);

			for (chip = group->chips; chip != NULL; chip = chip->next) {
				ads_chip_set_cmd_sync(chip, ADS_CMD_RESET);
			}
			/* Wait for reset cycle */
			HAL_Delay(10);
			retval = ads_group_set_config(group, config);

			if (retval == 0) {
				group->state = ADS_DRV_STATE_CONFIG;
			}
			break;
		}
		case ADS_DRV_STATE_CONFIG: {
			retval = ads_group_set_config(group, config);
			break;
		}
		case ADS_DRV_STATE_SAMPLING: {
			retval = ads1256_stop_sampling(group);

			if (retval != 0) {
				break;
			}
			retval = ads_group_set_config(group, config);
			break;
		}
	default:
		break;
	}

	if (retval != 0) {
		group->state = ADS_DRV_STATE_INIT;
	}

	return (retval);
}



int ads1256_start_sampling(struct ads1256_group * group)
{
	struct ads1256_chip * chip;

	if (group->state == ADS_DRV_STATE_INIT) {
		return (-1);
	}

	if (group->state == ADS_DRV_STATE_SAMPLING) {
		return (0);
	}

	/* Setup chips for sampling */

	for (chip = group->chips; chip != NULL; chip = chip->next) {
		ads_chip_sampling_prepare(chip);
	}

	/* Synchronize the chips */
	ads_group_power_deactivate(group);

	for (chip = group->chips; chip != NULL; chip = chip->next) {
		ads_chip_sampling_enable(chip);
		chip = chip->next;
	}
	group->state = ADS_DRV_STATE_SAMPLING;
	ads_group_power_activate(group);

	return (0);
}



int ads1256_stop_sampling(struct ads1256_group * group)
{
	struct ads1256_chip * chip;

	if (group->state != ADS_DRV_STATE_SAMPLING) {
		return (-1);
	}
	ads_group_power_deactivate(group);

	for (chip = group->chips; chip != NULL; chip = chip->next) {
		ads_chip_sampling_disable(chip);
	}
	ads_group_power_activate(group);

	for (chip = group->chips; chip != NULL; chip = chip->next) {
		ads_chip_sampling_unprepare(chip);
	}
	group->state = ADS_DRV_STATE_CONFIG;

	return (0);
}



void ads1256_drdy_isr(struct ads1256_group * group)
{
	struct ads1256_chip * chip;

	for (chip = group->chips; chip != NULL; chip = chip->next) {
		if (group->config.sampling_mode == ADS1256_SAMPLE_MODE_CONT) {
			/* Continuous mode */
			ads_chip_read_data_async(chip);
		} else {
			/* Read by command */
			ads_chip_read_data_sync(chip);
		}
	}
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of ads1256.c
 ******************************************************************************/

