/*
 * ads1256.h
 *
 *  Created on: Jan 19, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_ADS1256_H_
#define APPLICATION_INCLUDE_ADS1256_H_


#include <stdint.h>
#include <stdbool.h>

#include "base/bitop.h"
#include "prim_spi.h"

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

struct ads1256_chip_vt
{
	bool					 (* drdy_is_active)(void);
	void 					 (* drdy_isr_enable)(void);
	void					 (* drdy_isr_disable)(void);
	void                     (* nss_activate)(void);
	void                     (* nss_deactivate)(void);
};

struct ads1256_chip
{
    struct spi_transfer     transfer;
	struct spi_device    	device;
	struct ads1256_chip_vt  vt;
	uint8_t                 l_buffer[3];
};

void ads1256_init(struct ads1256_chip * chip, struct spi_bus * bus,
        const struct ads1256_chip_vt * vt);

void ads1256_wait_ready(struct ads1256_chip * chip);

void ads1256_reset_sync(struct ads1256_chip * chip);

void ads1256_write_reg_sync(struct ads1256_chip * chip, uint32_t reg,
        uint8_t data);

void ads1256_read_reg_sync(struct ads1256_chip * chip, uint32_t reg,
        uint8_t * data);

void ads1256_set_cmd_sync(struct ads1256_chip * chip, uint8_t cmd);

void ads1256_read_data_sync(struct ads1256_chip * chip);

void ads1256_rdc_start_sync(struct ads1256_chip * chip);

void ads1256_rdc_start_read_async(struct ads1256_chip * chip,
        void (* complete)(void *), void * arg);

void ads1256_rdc_stop_sync(struct ads1256_chip * chip);

static inline
void ads1256_drdy_isr_enable_high(struct ads1256_chip * chip)
{
    chip->vt.drdy_isr_enable();
}

static inline
void ads1256_drdy_isr_disable(struct ads1256_chip * chip)
{
    chip->vt.drdy_isr_disable();
}

#endif /* APPLICATION_INCLUDE_ADS1256_H_ */
