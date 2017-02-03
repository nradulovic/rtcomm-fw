/*
 * epa_adt7410.h
 *
 *  Created on: Mar 3, 2016
 *      Author: nenad
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       EPA for ADT7410 sensor
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_EPA_ADT7410_H_
#define APPLICATION_INCLUDE_EPA_ADT7410_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdbool.h>

#include "config_epa.h"
#include "neon_eds.h"

/*===============================================================  MACRO's  ==*/

#if !defined(EPA_ADT7410_EVENT_BASE_ID)
# error "Define EPA_ADT7410_EVENT_BASE_ID event ID base"
#endif

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

enum epa_adt7410_events
{
    SIG_ADT7410_INIT			=  EPA_ADT7410_EVENT_BASE_ID, 					/* Inicijalizuj ADT7410 */
	SIG_ADT7410_READY,										  					/* Odgovor: spreman sam */
	SIG_ADT7410_NOT_READY,									  					/* Odgovor: nisam spreman */
	SIG_ADT7410_START,															/* Startuj akviziciju */
	SIG_ADT7410_STOP,															/* Zaustavi akviziciju */
	EVENT_ADT7410_SET_CONFIG,													/* Postavi konfiguraciju za ADT7410 */
};



/* Konfiguraciona struktura za ADT7410 EPA.
 *
 */
struct event_adt7410_set_config
{
	struct nevent 				super;											/* Deo Neon dogadjaja, mora biti na prvom mestu */

	/* Period osvezavanja rezulatata u baferu, u milisekundama.
	 */
	uint32_t					period_ms;
};

/*======================================================  GLOBAL VARIABLES  ==*/

/* Struktura za ADT7410 EPA, preko ove strukture se ovaj automat vidi.
 */
extern struct nepa              g_epa_adt7410;

/* Podesavanja vezana za ADT7410 EPA
 */
extern const struct nepa_define g_epa_adt7410_define;

/*===================================================  FUNCTION PROTOTYPES  ==*/
/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of epa_adt7410.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_EPA_ADT7410_H_ */
