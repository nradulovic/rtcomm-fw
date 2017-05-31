/*
 *  teslameter_3mhx-fw - 2017
 *
 *  epacon.h
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
 * @brief       EPA configuration header
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_EPACON_H_
#define APPLICATION_INCLUDE_EPACON_H_

/*=========================================================  INCLUDE FILES  ==*/
/*===============================================================  MACRO's  ==*/

#define EPA_RTCOMM_PRIO                 31
#define EPA_RTCOMM_QUEUE_SIZE           10
#define EPA_RTCOMM_EVENT_BASE           1000

#define EPA_CTRL_PRIO					30
#define EPA_CTRL_QUEUE_SIZE				10
#define EPA_CTRL_EVENT_BASE				2000

#define EPA_CONTROLLER_PRIO				29
#define EPA_CONTROLLER_QUEUE_SIZE		10
#define EPA_CONTROLLER_EVENT_BASE		3000

#define EPA_PROBE_PRIO					28
#define EPA_PROBE_QUEUE_SIZE			10
#define EPA_PROBE_EVENT_BASE			4000

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/
/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of epacon.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_EPACON_H_ */
