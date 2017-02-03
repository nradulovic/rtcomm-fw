/*
 * epa_adt7410.h
 *
 *  Created on: Mar 3, 2016
 *      Author: nenad
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       EPA for EEPROM
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_EPA_EEPROM_H_
#define APPLICATION_INCLUDE_EPA_EEPROM_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "config_epa.h"
#include "neon_eds.h"

/*===============================================================  MACRO's  ==*/

#if !defined(EPA_EEPROM_EVENT_BASE_ID)
# error "Define EPA_EEPROM_EVENT_BASE_ID event ID base"
#endif

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

enum epa_eeprom_events
{
    SIG_EEPROM_INIT			=  EPA_EEPROM_EVENT_BASE_ID,
	SIG_EEPROM_INVAL,
	SIG_EEPROM_READY,
	SIG_EEPROM_NOT_READY,
	EVENT_EEPROM_READ,
	EVENT_EEPROM_WRITE
};

struct event_eeprom_transfer
{
	struct nevent 				super;
	uint32_t					address;
	void *						buffer;
	uint32_t					size;
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct nepa              g_epa_eeprom;

extern const struct nepa_define g_epa_eeprom_define;

extern struct nepa              g_epa_fram;

extern const struct nepa_define g_epa_fram_define;

/*===================================================  FUNCTION PROTOTYPES  ==*/
/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of epa_eeprom.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_EPA_EEPROM_H_ */
