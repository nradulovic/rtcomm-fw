/*
 * acquisition.h
 *
 *  Created on: May 30, 2017
 *      Author: nenad
 */

#ifndef APPLICATION_ACQUISITION_H_
#define APPLICATION_ACQUISITION_H_

#include "ads1256.h"
#include "cdi/io.h"

#define acquisition_probe_drdy_isr(probe) \
	ads1256_drdy_isr(&(probe)->group)

struct probe
{
	struct ads1256_group 		group;
	struct ads1256_chip			chip[IO_PROBE_CHANNELS];
	struct ads1256_chip_config  chip_config[IO_PROBE_CHANNELS];
	struct ads1256_group_config group_config;
};

struct aux
{
	struct ads1256_group		group;
	struct ads1256_chip			chip;
};

struct autorange
{
	bool						is_enabled;
};

extern struct probe 			g_probe;
extern struct aux				g_aux;
extern struct autorange			g_autorange;

void acquisition_init(void);
int acquisition_probe_set_config(const struct io_ctrl_config * config);
int acquisition_aux_set_config(const struct io_ctrl_config * config);
int acquisition_autorange_set_config(const struct io_ctrl_config * config);

int acquisition_probe_set_param(const struct io_ctrl_param * param);
int acquisition_aux_set_param(const struct io_ctrl_param * param);
int acquisition_autorange_set_param(const struct io_ctrl_param * param);

void acquisition_stop_sampling(void);
int acquisition_start_sampling(void);

#endif /* APPLICATION_ACQUISITION_H_ */
