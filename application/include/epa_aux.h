/*
 * epa_aux.h
 *
 *  Created on: May 26, 2015
 *      Author: nenad
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       EPA for AUX ADC channel
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_EPA_AUX_H_
#define APPLICATION_INCLUDE_EPA_AUX_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "config_epa.h"
#include "neon_eds.h"

/*===============================================================  MACRO's  ==*/

#if !defined(EPA_AUX_EVENT_BASE_ID)
# error "Define EPA_AUX_EVENT_BASE_ID event ID base"
#endif

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

enum epa_aux_events
{
    SIG_AUX_INIT 				= EPA_AUX_EVENT_BASE_ID,
	SIG_AUX_READY,
	SIG_AUX_NOT_READY,
	SIG_AUX_START,
    SIG_AUX_STOP,
	SIG_AUX_SET_CONFIG,
};



struct aux_config
{
	uint8_t						mux_channels;
	uint8_t						mux_configs[8];
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct nepa              g_epa_aux;

extern const struct nepa_define g_epa_aux_define;

extern struct aux_config		g_aux_current_config;

extern struct aux_config		g_aux_pending_config;

extern const struct aux_config  g_aux_default_config;

/*===================================================  FUNCTION PROTOTYPES  ==*/
/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of epa_aux.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_EPA_AUX_H_ */
