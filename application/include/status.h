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

#ifndef APPLICATION_INCLUDE_STATUS_H_
#define APPLICATION_INCLUDE_STATUS_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>

/*===============================================================  MACRO's  ==*/

#define STATUS_UNHANDLED_EXCP			1
#define STATUS_ASSERT_FAILED			2
#define STATUS_HW_INIT_FAILED			3
#define STATUS_RUNTIME_CHECK_FAILED		4
#define STATUS_RTCOMM_FAILED			5
#define STATUS_CTRL_FAILED				6
#define STATUS_RESOURCE_FAILED			7

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

extern uint32_t							g_status_counters[];

/*===================================================  FUNCTION PROTOTYPES  ==*/

void status_warn(uint32_t error);

void status_error(uint32_t error);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of app_stat.h
 ******************************************************************************/

#endif /* APPLICATION_INCLUDE_STATUS_H_ */
