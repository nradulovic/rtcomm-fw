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

#define STAT_SOURCE_SYSTEM              (0x1u << 0)
#define STAT_SOURCE_CTRL                (0x1u << 1)
#define STAT_SOURCE_CMD_ID              (0x1u << 2)
#define STAT_SOURCE_CMD_ARG             (0x1u << 3)
#define STAT_SOURCE_ACQ                 (0x1u << 4)
#define STAT_SOURCE_AUX                 (0x1u << 5)
#define STAT_SOURCE_MS                  (0x1u << 6)
#define STAT_SOURCE_FLASH				(0x1u << 7)

#define STAT_LEVEL_WARNING              0
#define STAT_LEVEL_ERROR                16

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/


void status_init(void);



void status_set(uint8_t source, uint8_t level);



void status_clear(void);



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
