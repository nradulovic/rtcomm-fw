/*
 * app_stat.h
 *
 *  Created on: May 26, 2015
 *      Author: Nenad Radulovic
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Application status
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_APP_STAT_H_
#define APPLICATION_INCLUDE_APP_STAT_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>

#include "hw_config.h"
#include "prim_gpio.h"

/*===============================================================  MACRO's  ==*/

#define led_ms_send_on()
#define led_ms_send_off()

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

void status_panic(uint32_t error);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of app_stat.h
 ******************************************************************************/

#endif /* APPLICATION_INCLUDE_APP_STAT_H_ */
