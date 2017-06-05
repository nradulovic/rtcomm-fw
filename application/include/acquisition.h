/*
 *  teslameter_3mhx-fw - 2017
 *
 *  acquisition.h
 *
 *  Created on: May 29, 2017
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
 * @brief       Acquisition
 *********************************************************************//** @{ */

#ifndef APPLICATION_ACQUISITION_H_
#define APPLICATION_ACQUISITION_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "ads1256.h"
#include "cdi/io.h"

/*===============================================================  MACRO's  ==*/

#define acquisition_probe_drdy_isr(channel) \
    ads1256_drdy_isr(&(channel)->group)

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================  DATA TYPES  ==*/

struct probe
{
    struct ads1256_group        group;
    struct ads1256_chip         chip[IO_PROBE_CHANNELS];
    struct ads1256_group_config group_config;
    struct ads1256_chip_config  chip_config[IO_PROBE_CHANNELS];
};

struct aux
{
    struct ads1256_group        group;
    struct ads1256_chip         chip;
    struct ads1256_group_config group_config;
    struct ads1256_chip_config  chip_config;
    int32_t                     mvalues[IO_AUX_CHANNELS];
};

struct autorange
{
    bool                        is_enabled;
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct probe             g_probe;
extern struct aux               g_aux;
extern struct autorange         g_autorange;

/*===================================================  FUNCTION PROTOTYPES  ==*/

void acquisition_init(void);

int acquisition_set_config(const struct io_ctrl_config * config);

int acquisition_probe_set_param(const struct io_ctrl_param * param);

int acquisition_aux_set_param(const struct io_ctrl_param * param);

int acquisition_autorange_set_param(const struct io_ctrl_param * param);

void acquisition_stop_sampling(void);

int acquisition_start_sampling(void);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of acquisition.h
 ******************************************************************************/
#endif /* APPLICATION_ACQUISITION_H_ */
