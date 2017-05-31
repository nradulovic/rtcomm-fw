/*
 * epa_ctrl.h
 *
 *  Created on: May 30, 2017
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_EPA_CTRL_H_
#define APPLICATION_INCLUDE_EPA_CTRL_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "config/epacon.h"
#include "cdi/io.h"
#include "neon_eds.h"

/*===============================================================  MACRO's  ==*/
/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================  DATA TYPES  ==*/

enum ctrl_epa_events
{
	CTRL_EVT_CONFIG = EPA_CTRL_EVENT_BASE,
	CTRL_EVT_PARAM
};

struct ctrl_evt_config
{
	struct nevent				super;
	struct io_ctrl_config		config;
};

struct ctrl_evt_param
{
	struct nevent				super;
	struct io_ctrl_param		param;
};

struct ctrl
{
	I2C_HandleTypeDef			i2c;
	void *						buffer;
	uint16_t					size;
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct nepa_define 		g_ctrl_epa_define;
extern struct nepa 				g_ctrl_epa;

extern struct ctrl 				g_ctrl;

/*===================================================  FUNCTION PROTOTYPES  ==*/

/* -- Methods to be used by ISRs -------------------------------------------- */

void ctrl_isr_rx_complete(struct ctrl * ctrl);

void ctrl_isr_tx_complete(struct ctrl * ctrl);

void ctrl_isr_error(struct ctrl * ctrl);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of epa_ctrl.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_EPA_CTRL_H_ */
