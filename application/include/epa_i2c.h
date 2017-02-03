/*
 * epa_heartbeat.h
 *
 *  Created on: May 29, 2015
 *      Author: nenad
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       EPA for I2C communication
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_EPA_I2C_H_
#define APPLICATION_INCLUDE_EPA_I2C_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "config_epa.h"
#include "neon_eds.h"

/*===============================================================  MACRO's  ==*/

#if !defined(EPA_I2C_EVENT_BASE_ID)
# error "Define EPA_I2C_EVENT_BASE_ID event ID base"
#endif

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

enum epa_i2c_events
{
	SIG_I2C_INIT 				= EPA_I2C_EVENT_BASE_ID,
	SIG_I2C_READY,
	SIG_I2C_NOT_READY,
	SIG_I2C_CANCEL,
	EVENT_I2C_WRITE,
	EVENT_I2C_READ,
	EVENT_I2C_COMPLETE
};

struct event_i2c_transfer
{
	struct nevent				super;
	void *						buffer;
	uint16_t					size;
	uint8_t						dev_id;
	uint32_t					timeout_ms;
};

struct event_i2c_complete
{
	struct nevent 				super;
	nerror						error;
	uint16_t					transferred;
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct nepa                     g_epa_i2c;

extern const struct nepa_define        g_epa_i2c_define;

/*===================================================  FUNCTION PROTOTYPES  ==*/
/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of epa_i2c.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_EPA_I2C_H_ */
