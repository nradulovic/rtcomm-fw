/*
 * protocol.h
 *
 *  Created on: Aug 17, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_EPA_PROTOCOL_H_
#define APPLICATION_INCLUDE_EPA_PROTOCOL_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "config_epa.h"
#include "neon_eds.h"

/*===============================================================  MACRO's  ==*/

#if !defined(EPA_PROTOCOL_EVENT_BASE_ID)
# error "Define EPA_PROTOCOL_EVENT_BASE_ID event ID base"
#endif

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

enum epa_protocol_events
{
	SIG_PROTOCOL_INIT = EPA_PROTOCOL_EVENT_BASE_ID,
	SIG_PROTOCOL_READY,
	SIG_PROTOCOL_NOT_READY
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct nepa              g_epa_protocol;
extern struct nepa_define       g_epa_protocol_define;

/*===================================================  FUNCTION PROTOTYPES  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of epa_main.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_EPA_PROTOCOL_H_ */
