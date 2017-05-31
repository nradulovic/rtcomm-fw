/*
 *  teslameter_3mhx-fw - 2017
 *
 *  epa_ctrl.h
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
 * @brief       Control bus interface
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_EPA_CTRL_H_
#define APPLICATION_INCLUDE_EPA_CTRL_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "config/epacon.h"
#include "cdi/io.h"
#include "neon_eds.h"

/*===============================================================  MACRO's  ==*/
/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================  DATA TYPES  ==*/

enum ctrl_epa_events
{
	CTRL_EVT_CONFIG = EPA_CTRL_EVENT_BASE,
	CTRL_EVT_PARAM
};

struct ctrl_evt_config
{
	struct nevent				super;
	struct io_ctrl_config		config;
};

struct ctrl_evt_param
{
	struct nevent				super;
	struct io_ctrl_param		param;
};

struct ctrl
{
	I2C_HandleTypeDef			i2c;
	void *						buffer;
	uint16_t					size;
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct nepa_define 		g_ctrl_epa_define;
extern struct nepa 				g_ctrl_epa;

extern struct ctrl 				g_ctrl;

/*===================================================  FUNCTION PROTOTYPES  ==*/

/* -- Methods to be used by ISRs -------------------------------------------- */

void ctrl_isr_rx_complete(struct ctrl * ctrl);

void ctrl_isr_tx_complete(struct ctrl * ctrl);

void ctrl_isr_error(struct ctrl * ctrl);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of epa_ctrl.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_EPA_CTRL_H_ */
