/*
 *  teslameter_3mhx-fw - 2017
 *
 *  protocol.h
 *
 *  Created on: May 31, 2017
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
 * @brief       Protocol conversion functions interface
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_PROTOCOL_H_
#define APPLICATION_INCLUDE_PROTOCOL_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>
#include <stdbool.h>
#include "cdi/io.h"

/*===============================================================  MACRO's  ==*/
/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

bool protocol_from_en_probe_buffer(const struct io_ctrl_config * config);
uint8_t protocol_from_probe_mux_hi(const struct io_ctrl_config * config);
uint8_t protocol_from_probe_mux_lo(const struct io_ctrl_config * config);

bool protocol_from_en_aux_bufer(const struct io_ctrl_config * config);
uint8_t protocol_from_aux_mux_hi(const struct io_ctrl_config * config,
        uint32_t mchannel);
uint8_t protocol_from_aux_mux_lo(const struct io_ctrl_config * config,
        uint32_t mchannel);
bool protocol_from_aux_en_aux1(const struct io_ctrl_config * config);
bool protocol_from_aux_en_aux2(const struct io_ctrl_config * config);

uint8_t protocol_from_workmode(const struct io_ctrl_param * param);
uint32_t protocol_from_vspeed(const struct io_ctrl_param * param);
bool protocol_from_en_autorange(const struct io_ctrl_param * param);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of protocol.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_PROTOCOL_H_ */
