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

#define ADS1256_SAMPLE_MODE_REQ			0u
#define ADS1256_SAMPLE_MODE_CONT		1u

struct ads1256_chip_vt
{
	bool					 (* drdy_is_active)(void);
	void 					 (* drdy_isr_enable)(void);
	void					 (* drdy_isr_disable)(void);
	void                     (* nss_activate)(void);
	void                     (* nss_deactivate)(void);
	void				     (* reader)(int32_t);
	void					  * reader_arg;
};

struct ads1256_config
{
	uint8_t					mux_lo;
	uint8_t					mux_hi;
	uint8_t					sspeed;
	uint8_t					gpio;
	/* 0 - mode RDATA
	 * 1 - mode RDATAC
	 */
	uint8_t					sampling_mode;
	bool					enable_ext_osc;
	bool					enable_buffer;
};

struct ads1256_chip
{
    struct spi_transfer     transfer;
	struct spi_device    	device;
	struct ads1256_chip_vt  vt;
	uint8_t                 l_buffer[4];
	struct ads1256_config	config;
	uint32_t				state;
};

void ads1256_init(struct ads1256_chip * chip, struct spi_bus * bus,
        const struct ads1256_chip_vt * vt);

int ads1256_set_config(struct ads1256_chip * chip,
		const struct ads1256_config * config);

void ads1256_start_sampling(struct ads1256_chip * chip);

void ads1256_stop_sampling(struct ads1256_chip * chip);

void ads1256_drdy_isr(struct ads1256_chip * chip);

#endif /* APPLICATION_INCLUDE_ADS1256_H_ */
