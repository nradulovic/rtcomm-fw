/*
 *  rtcomm-fw - 2017
 *
 *  ads1256.h
 *
 *  Created on: Jan 19, 2015
 * ----------------------------------------------------------------------------
 *  This file is part of rtcomm-fw.
 *
 *  rtcomm-fw is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  rtcomm-fw is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with rtcomm-fw.  If not, see <http://www.gnu.org/licenses/>.
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
#include "cdi/io.h"

/*===============================================================  MACRO's  ==*/

#define ADS1256_SAMPLE_MODE_REQ         0u
#define ADS1256_SAMPLE_MODE_CONT        1u

#define ADS1256_SAMPLE_RATE_2_5         2u
#define ADS1256_SAMPLE_RATE_5           5u
#define ADS1256_SAMPLE_RATE_10          10u
#define ADS1256_SAMPLE_RATE_15          15u
#define ADS1256_SAMPLE_RATE_25          25u
#define ADS1256_SAMPLE_RATE_30          30u
#define ADS1256_SAMPLE_RATE_50          50u
#define ADS1256_SAMPLE_RATE_60          60u
#define ADS1256_SAMPLE_RATE_100         100u
#define ADS1256_SAMPLE_RATE_500         500u
#define ADS1256_SAMPLE_RATE_1000        1000u
#define ADS1256_SAMPLE_RATE_2000        2000u
#define ADS1256_SAMPLE_RATE_3750        3750u
#define ADS1256_SAMPLE_RATE_7500        7500u
#define ADS1256_SAMPLE_RATE_15000       15000u
#define ADS1256_SAMPLE_RATE_30000       30000u

#define ADS1256_MAX_MCHANNELS           IO_AUX_CHANNELS

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================  DATA TYPES  ==*/

/* Forward declarations */
struct ads1256_group;

/* Virtual Table of functions associated with one ADC chip. */
struct ads1256_chip_vt
{
	uint32_t				 (* drdy_is_high)(void);
    void                     (* drdy_isr_enable)(void);
    void                     (* drdy_isr_disable)(void);
    void                     (* nss_activate)(void);
    void                     (* nss_deactivate)(void);
};

/* Virtual Table of functions associated with one group of chips. */
struct ads1256_group_vt
{
    void                     (* power_activate)(void);
    void                     (* power_deactivate)(void);
    void                     (* sample_finished)(const struct ads1256_group *);
};

/* Configuration for one ADC chip. */
struct ads1256_chip_config
{
    uint8_t                     mux_lo[ADS1256_MAX_MCHANNELS];
    uint8_t                     mux_hi[ADS1256_MAX_MCHANNELS];

    /* Number of MUX channels in mux_lo and mux_hi arrays */
    uint8_t                     no_mux_channels;

    /* GPIO value to set at start */
    uint8_t                     gpio;

    /* Enable ADC clocking to external pin */
    bool                        enable_ext_osc;

    /* Enable ADC input buffer */
    bool                        enable_buffer;

    /* When true then this ADC is the master of the group. It's DRDY signal is
     * taken as master DRDY signal for all ADCs in the group.
     */
    bool                        is_master;
};

struct ads1256_group_config
{
    /* ADC sampling mode:
     * 1 - ADS1256_SAMPLE_MODE_REQ - data is read by explicit RDATA command
     * 2 - ADS1256_SAMPLE_MODE_CONT - data is read just after DRDY signal has
     *      gone to low.
     */
    uint8_t                     sampling_mode;

    /* ADC sampling rate, value in SPS. See ADS1256_SAMPLE_RATE_* macros for
     * possible values.
     */
    uint32_t                    sampling_rate;
};

/* Main ADS1256 struct. Each struct is serving one physical ADC chip. */
struct ads1256_chip
{
    struct spi_transfer         transfer;
    struct spi_device           device;
    const struct ads1256_chip_vt *
                                vt;
    const struct ads1256_chip_config *
                                config;
    struct ads1256_group *      group;
    struct ads1256_chip *       next;
    union local_buffer
    {
        uint8_t                     buffer[4];
        uint32_t                    integer;
    }                           l;
    uint32_t                    current_mchannel;
    uint32_t                    id;
    uint32_t                    id_mask;
};

/* A group of ADC chips. This structure is used to synchronize several ADC
 * chips, where all chips are clocked by one master chip. A group must have at
 * least one chip.
 */
struct ads1256_group
{
	void 					 (* isr)(struct ads1256_group *);
    struct ads1256_chip *       chips;
    struct ads1256_chip *       master;
    uint32_t                    state;
    uint32_t                    sampled;
    uint32_t                    enabled;
    const struct ads1256_group_config *
                                config;
    const struct ads1256_group_vt *
                                vt;
};

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

void ads1256_init_chip(struct ads1256_chip * chip, struct spi_bus * bus,
        const struct ads1256_chip_vt * vt, uint32_t id);

void ads1256_group_init(struct ads1256_group * group,
        const struct ads1256_group_vt * vt);

void ads1256_group_add_chip(struct ads1256_group * group,
        struct ads1256_chip * chip);

void ads1256_set_per_chip_config(struct ads1256_chip * chip,
        const struct ads1256_chip_config * config);

void ads1256_set_group_config(struct ads1256_group * group,
        const struct ads1256_group_config * config);

int ads1256_apply_group_config(struct ads1256_group * group);

int ads1256_wait_ready(struct ads1256_group * group);

int ads1256_start_sampling(struct ads1256_group * group);

int ads1256_stop_sampling(struct ads1256_group * group);

#define ads1256_get_value(chip) n_ext_i24((int32_t)(__REV((chip)->l.integer) >> 8u))

#define ads1256_drdy_isr(group) (group)->isr(group);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of ads1256.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_ADS1256_H_ */
