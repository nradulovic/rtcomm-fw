/*
 * epa_heartbeat.h
 *
 *  Created on: May 29, 2015
 *      Author: nenad
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       EPA for AUX ADC channel
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_EPA_HEARTBEAT_H_
#define APPLICATION_INCLUDE_EPA_HEARTBEAT_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "config_epa.h"
#include "neon_eds.h"

/*===============================================================  MACRO's  ==*/

#if !defined(EPA_HEARTBEAT_BASE_ID)
# error "Define EPA_HEARTBEAT_BASE_ID event ID base"
#endif

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

enum epa_heartbeat_events
{
	SIG_HEARTBEAT_INIT 						= EPA_HEARTBEAT_BASE_ID,
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct nepa                     g_epa_heartbeat;

extern const struct nepa_define        g_epa_heartbeat_define;

/*===================================================  FUNCTION PROTOTYPES  ==*/
/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of epa_aux.h
 ******************************************************************************/






#endif /* APPLICATION_INCLUDE_EPA_HEARTBEAT_H_ */
