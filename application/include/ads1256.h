/*
 * ads1256.h
 *
 *  Created on: Jan 19, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_ADS1256_H_
#define APPLICATION_INCLUDE_ADS1256_H_

#include <prim_spi.h>
#include <stdint.h>
#include <stdbool.h>

#include "cdi/io.h"

#define MUX_MAX_CHANNEL                 7

struct ads1256_chip_vt
{
	bool					 (* drdy_is_active)(void);
	void 					 (* drdy_isr_enable)(void);
	void					 (* drdy_isr_disable)(void);
};

struct spi_client;
struct spi_transfer;

struct ads1256_chip
{
	struct spi_client *  	client;
	struct spi_transfer		transfer;
	struct ads1256_chip_vt  vt;
};

void ads1256_init(struct ads1256_chip * chip, struct spi_client * client,
		const struct ads1256_chip_vt * vtable);
void ads1256_reset_sync(struct ads1256_chip * chip);
void ads1256_wait(struct ads1256_chip * chip);
void ads1256_write_reg_sync(struct ads1256_chip * chip, uint32_t reg, uint8_t data);
void ads1256_write_reg_async(struct ads1256_chip * chip, uint32_t reg, uint8_t data);
uint8_t ads1256_read_reg_sync(struct ads1256_chip * chip, uint32_t reg);
uint8_t ads1256_write_then_read_reg_sync(struct ads1256_chip * chip, uint32_t reg, uint8_t data);
void ads1256_read_sync(struct ads1256_chip * chip);

void ads1256_read_begin_sync(struct ads1256_chip * chip,
	void                     (* complete)(struct spi_transfer *));



void ads1256_read_finish_sync(
	struct ads1256_chip * 		chip,
	void                     (* complete)(struct spi_transfer *));



void ads1256_read_delay(void);



void ads1256_sync_min_delay(void);



void ads1256_set_cmd_async(struct ads1256_chip * chip, uint8_t cmd);
void ads1256_set_cmd_sync(struct ads1256_chip * chip, uint8_t cmd);
void ads1256_set_cmd_sync_no_wait(struct ads1256_chip * chip, uint8_t cmd);
void ads1256_set_cmds_sync(struct ads1256_chip * chip, uint8_t * cmd, uint16_t size);
void ads1256_rdc_read_async(struct ads1256_chip * chip, void (* complete)(struct spi_transfer *));
void ads1256_rdc_read_sync(struct ads1256_chip * chip, void (* complete)(struct spi_transfer *));
void ads1256_rdc_setup(struct ads1256_chip * chip);
void ads1256_rdc_finish(struct ads1256_chip * chip);
void ads1256_drdy_isr_enable(struct ads1256_chip * chip);
void ads1256_drdy_isr_disable(struct ads1256_chip * chip);

#endif /* APPLICATION_INCLUDE_ADS1256_H_ */
