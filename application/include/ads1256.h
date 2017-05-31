/*
 *  teslameter_3mhx-fw - 2017
 *
 *  ads1256.h
 *
 *  Created on: Jan 19, 2015
 * ----------------------------------------------------------------------------
 *  This file is part of teslameter_3mhx-fw.
 *
 *  teslameter_3mhx-fw is free software: you can redistribute it and/or modify
 *  it under the terms of the Lesser GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  teslameter_3mhx-fw is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with teslameter_3mhx-fw.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------- *//**
 * @file
 * @author      Nenad Radulovic
 * @brief       ADS1256
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_ADS1256_H_
#define APPLICATION_INCLUDE_ADS1256_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>
#include <stdbool.h>

#include "prim_spi.h"

/*===============================================================  MACRO's  ==*/

#define ADS1256_SAMPLE_MODE_REQ			0u
#define ADS1256_SAMPLE_MODE_CONT		1u

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================  DATA TYPES  ==*/

struct ads1256_chip_vt
{
	void 					 (* drdy_isr_enable)(void);
	void					 (* drdy_isr_disable)(void);
	void                     (* nss_activate)(void);
	void                     (* nss_deactivate)(void);
	void				     (* reader)(void *);
};

struct ads1256_group_vt
{
	void                     (* power_activate)(void);
	void                     (* power_deactivate)(void);
};

struct ads1256_chip_config
{
	uint8_t						mux_lo;
	uint8_t						mux_hi;
	uint8_t						gpio;
	bool						enable_ext_osc;
	bool						enable_buffer;
	bool						is_master;
};

struct ads1256_group_config
{
	/* 0 - mode RDATA
	 * 1 - mode RDATAC
	 */
	uint8_t						sampling_mode;
	uint32_t					sampling_rate;
};

struct ads1256_chip
{
    struct spi_transfer     	transfer;
	struct spi_device    		device;
	const struct ads1256_chip_vt *
								vt;
	uint8_t                 	l_buffer[4];
	const struct ads1256_chip_config *
								config;
	struct ads1256_group *		group;
	struct ads1256_chip *		next;
};

struct ads1256_group
{
	struct ads1256_chip *		chips;
	uint32_t					state;
	const struct ads1256_group_config *
								config;
	const struct ads1256_group_vt *
								vt;
};

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

void ads1256_init_chip(struct ads1256_chip * chip, struct spi_bus * bus,
        const struct ads1256_chip_vt * vt);

void ads1256_group_init(struct ads1256_group * group,
		const struct ads1256_group_vt * vt);

void ads1256_group_add_chip(struct ads1256_group * group,
		struct ads1256_chip * chip);

void ads1256_set_per_chip_config(struct ads1256_chip * chip,
		const struct ads1256_chip_config * config);

void ads1256_set_group_config(struct ads1256_group * group,
		const struct ads1256_group_config * config);

int ads1256_apply_group_config(struct ads1256_group * group);

int ads1256_start_sampling(struct ads1256_group * group);

int ads1256_stop_sampling(struct ads1256_group * group);

void ads1256_drdy_isr(struct ads1256_group * group);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of ads1256.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_ADS1256_H_ */
