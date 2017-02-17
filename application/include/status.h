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

#endif /* APPLICATION_INCLUDE_STATUS_H_ */
