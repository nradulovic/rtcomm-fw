/*
 * epa_controller.h
 *
 *  Created on: May 30, 2017
 *      Author: nenad
 */

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
	struct nevent				super;
	struct io_ctrl_config		config;
};

struct controller_evt_param
{
	struct nevent				super;
	struct io_ctrl_param		param;
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct nepa_define 		g_controller_epa_define;
extern struct nepa 				g_controller_epa;

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
