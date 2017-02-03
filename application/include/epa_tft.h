/* 
 *
 * web site:    www.
 * e-mail  :    predrag_nedeljkovic@sentronis.rs
 *//***********************************************************************//**
 * @file      epa_tft.h
 * @date      21.04.2016
 * @author    Predrag Nedeljkovic 
 * @brief        
 *********************************************************************//** @{ */
#ifndef EPA_TFT_H_
#define EPA_TFT_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "config_epa.h"
#include "neon_eds.h"
#include <string.h>
#include "device_mem_map.h"
#include "epa_acq_interface.h"

/*===============================================================  MACRO's  ==*/
#define ACQ_CHANNEL_X  0
#define ACQ_CHANNEL_Y  1
#define ACQ_CHANNEL_Z  2
#define AUX_CHANNEL    3
#define TEd            4

#if !defined(EPA_TFT_EVENT_BASE_ID)
#error "Define EPA_TFT_EVENT_BASE_ID event ID base"
#endif

/*============================================================  DATA TYPES  ==*/


enum epa_tft_events
{
    SIG_TFT_INIT		 =  EPA_TFT_EVENT_BASE_ID,
    SIG_TFT_READY,
    EVENT_TFT_INIT,
    SIG_TFT_NOT_READY,
    SIG_TFT_READY_TO_PRINT_VAL,
    SIG_DATA_AVAILABLE,
};



struct event_tft_transfer
{
	struct nevent 	  super;
	void *		  buffer;
	uint8_t		  size;
};


/*======================================================  GLOBAL VARIABLES  ==*/

extern struct nepa              g_epa_tft;

extern const struct nepa_define g_epa_tft_define;

/*==================================================  CONFIGURATION ERRORS  ==*/

/*******************************************************************************
 * END of   .h
 *******************************************************************************/
#endif /* EPA_TFT_H_ */
