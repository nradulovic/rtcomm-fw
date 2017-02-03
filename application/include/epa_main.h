/*
 * epa_main.h
 *
 *  Created on: May 25, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Event Processing Agent for Main process
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_EPA_MAIN_H_
#define APPLICATION_INCLUDE_EPA_MAIN_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "config_epa.h"
#include "neon_eds.h"
#include "cdi/io.h"

/*===============================================================  MACRO's  ==*/

#define MAIN_TRIGG_STOP					0
#define MAIN_TRIGG_RUNNING				1
#define MAIN_TRIGG_BUFFER_FULL			2
#define MAIN_TRIGG_WR_FAILED			3

#if !defined(EPA_MAIN_EVENT_BASE_ID)
# error "Define EPA_MAIN_EVENT_BASE_ID event ID base"
#endif

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

enum epa_main_events
{
	/* Input */
	SIG_MAIN_UPDATE_CONFIG = EPA_MAIN_EVENT_BASE_ID,
	SIG_MAIN_START_SAMPLING,
	SIG_MAIN_STOP_SAMPLING,

	/* Output */
	SIG_MAIN_OK,
	SIG_MAIN_NOT_OK,
	EVENT_MAIN_SAMPLE,
};

struct event_main_sample
{
	struct nevent 				super;
	struct acq_sample			data;
    
};

struct event_main_disp
{
    struct nevent               super;
    enum process_data_mode      proc_mode;
};    

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct nepa              g_epa_main;

extern const struct nepa_define g_epa_main_define;

extern uint32_t 				g_trigg_samples;

extern uint32_t					g_trigg_last_state;

extern uint32_t					g_trigg_current_no;

/*===================================================  FUNCTION PROTOTYPES  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of epa_main.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_EPA_MAIN_H_ */
