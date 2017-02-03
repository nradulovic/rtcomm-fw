/* ************************************************************************//**
 *
 * web site:  
 * e-mail  :    predrag_nedeljkovic@sentronis.rs
 *//***********************************************************************//**
 * @file		  epa_acq_interface.h
 * @date			July 06, 2016
 * @author    Predrag Nedeljkovic 
 * @brief     TFT - analog channel inteface: merges aux channal and acq channel epa's; 
							this epa forward analog data values to TFT epa after epa's synchronization;  
 *********************************************************************//** @{ */
#ifndef APPLICATION_INCLUDE_EPA_ACQ_INTERFACE_H_
#define APPLICATION_INCLUDE_EPA_ACQ_INTERFACE_H_

/*=========================================================  INCLUDE FILES  ==*/
#include "config_epa.h"
#include "epa_main.h"
#include "epa_aux.h"
#include "epa_tft.h"
#include "device_mem_map.h"
#include "cdi/io.h"
#include "device_mem_map.h"
/*===============================================================  MACRO's  ==*/

/*-------------------------------------------------------  C++ extern base  --*/

#ifdef __cplusplus

extern "C" {

#endif

/*============================================================  DATA TYPES  ==*/
	
enum epa_acq_interface_events
{
    SIG_ACQ_INTERFACE_INIT 				= EPA_ACQ_INTERFACE_EVENT_BASE_ID,
    SIG_ACQ_INTERFACE_READY,
	SIG_ACQ_INT_EMIT_DATA,
	SIG_ACQ_INT_STOP_EMITING,
	EVENT_ACQ_INT_DATA_MODE_SELECT, 
	EVENT_DATA_READY,
};


/*======================================================  GLOBAL VARIABLES  ==*/

struct event_acq_transfer
{
	struct nevent 	        super;
    enum process_data_mode  proc_mode;
};


/*======================================================  GLOBAL VARIABLES  ==*/
extern struct nepa              g_epa_acqintc;
extern const struct nepa_define g_epa_acqintc_define;

/*=================================================  CONFIGURATION ERRORS  ==*/
/*--------------------------------------------------------  C++ extern end  --*/

#ifdef __cplusplus

}

#endif

/*******************************************************************************
 * END of  epa_acq_interface.h
 *
********************************************************************************/
#endif /* APPLICATION_INCLUDE_EPA_ACQ_INTERFACE_H_ */