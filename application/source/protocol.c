/*
 *  teslameter_3mhx-fw - 2017
 *
 *  protocol.c
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
 * @brief       Protocol conversion functions
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "protocol.h"
#include "cdi/io.h"
#include "ads1256.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

bool protocol_from_en_probe_buffer(const struct io_ctrl_config * config)
{
	return (config->en_probe_buffer == 1u ? true : false);
}

uint8_t protocol_from_probe_mux_hi(const struct io_ctrl_config * config)
{
	return (config->probe_mux_hi <= 8 ? config->probe_mux_hi : 8);
}

uint8_t protocol_from_probe_mux_lo(const struct io_ctrl_config * config)
{
	return (config->probe_mux_lo <= 8 ? config->probe_mux_lo : 8);
}

uint8_t protocol_from_workmode(const struct io_ctrl_param * param)
{
	switch (param->workmode) {
		case IO_WORKMODE_NORMAL: 			return (ADS1256_SAMPLE_MODE_CONT);
		case IO_WORKMODE_TRIG_CONTINUOUS: 	return (ADS1256_SAMPLE_MODE_CONT);
		case IO_WORKMODE_TRIG_SINGLE_SHOT:	return (ADS1256_SAMPLE_MODE_CONT);
		default: 							return (ADS1256_SAMPLE_MODE_CONT);
	}
}

uint32_t protocol_from_vspeed(const struct io_ctrl_param * param)
{
	/*
	 * This follows JNI document.
	 */
	switch (param->vspeed) {
		case 1:  	return (10);
		case 4:		return (30);
		case 5:		return (50);
		case 6: 	return (60);
		case 7:		return (100);
		case 8:		return (500);
		case 9: 	return (1000);
		case 10:	return (2000);
		case 11:	return (3750);
		case 12:	return (7500);
		case 13: 	return (15000);
		case 14: 	return (30000);
		default:	return (1000);
	}
}

bool protocol_from_en_autorange(const struct io_ctrl_param * param)
{
	return (param->en_autorange == 1u ? true : false);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of protocol.c
 ******************************************************************************/
