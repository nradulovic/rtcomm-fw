/*
 *  teslameter_3mhx-fw - 2017
 *
 *  epa_controller.h
 *
 *  Created on: May 30, 2017
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
 * @brief       Controller EPA
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_EPA_CONTROLLER_H_
#define APPLICATION_INCLUDE_EPA_CONTROLLER_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "config/epacon.h"
#include "cdi/io.h"

/*===============================================================  MACRO's  ==*/
/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================  DATA TYPES  ==*/

enum controller_event
{
    CONTROLLER_SIG_INIT = EPA_CONTROLLER_EVENT_BASE,
    CONTROLLER_SIG_INIT_DONE,
    CONTROLLER_EVT_SET_CONFIG,
    CONTROLLER_EVT_SET_PARAM,
};

struct controller_evt_config
{
    struct nevent               super;
    struct io_ctrl_config       config;
};

struct controller_evt_param
{
    struct nevent               super;
    struct io_ctrl_param        param;
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct nepa_define       g_controller_epa_define;
extern struct nepa              g_controller_epa;

/*===================================================  FUNCTION PROTOTYPES  ==*/
/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of epa_controller.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_EPA_CONTROLLER_H_ */
