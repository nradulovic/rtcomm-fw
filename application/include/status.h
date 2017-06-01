/*
 *  teslameter_3mhx-fw - 2017
 *
 *  status.h (former app_stat.h)
 *
 *  Created on: May 26, 2015
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
 * @brief       Status interface
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_STATUS_H_
#define APPLICATION_INCLUDE_STATUS_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>

/*===============================================================  MACRO's  ==*/

#define STATUS_UNHANDLED_EXCP			1
#define STATUS_ASSERT_FAILED			2
#define STATUS_HW_INIT_FAILED			3
#define STATUS_RUNTIME_CHECK_FAILED		4
#define STATUS_RTCOMM_COMPLETE_ERROR	5
#define STATUS_CTRL_FAILED				6
#define STATUS_RESOURCE_FAILED			7
#define STATUS_ADS_OVR					8
#define STATUS_RTCOMM_SKIPPED_ERROR		9
#define STATUS_RTCOMM_TRANSFER_ERROR	10

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

extern uint32_t							g_status_counters[];

/*===================================================  FUNCTION PROTOTYPES  ==*/

void status_warn(uint32_t error);

void status_error(uint32_t error);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of app_stat.h
 ******************************************************************************/

#endif /* APPLICATION_INCLUDE_STATUS_H_ */
