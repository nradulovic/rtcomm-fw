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

#include "prim_spi.h"

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
