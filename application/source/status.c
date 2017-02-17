/*
 * app_stat.c
 *
 *  Created on: May 26, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Application status
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "status.h"
#include "hwcon.h"
#include "stm32f4xx_hal.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define LOOP_COUNT                      (50ul * 100000ul)

#define led_heartbeat_on()														\
	HAL_GPIO_WritePin(HWCON_HEARTBEAT_PORT, HWCON_HEARTBEAT_PIN, 			\
			GPIO_PIN_SET)

#define led_heartbeat_off()                                                     \
    	HAL_GPIO_WritePin(HWCON_HEARTBEAT_PORT, HWCON_HEARTBEAT_PIN, 			\
			GPIO_PIN_RESET)

/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/



void status_panic(uint32_t error)
{
	uint32_t					counter;
    volatile uint32_t           idx;

    for (;;) {
        for(idx = 0; idx < LOOP_COUNT; idx++);
        led_heartbeat_off();
        for(idx = 0; idx < LOOP_COUNT; idx++);
        led_heartbeat_on();

        for (counter = 0; counter < error; counter++) {
            for(idx = 0; idx < LOOP_COUNT / 5; idx++);
            led_heartbeat_off();
            for(idx = 0; idx < LOOP_COUNT / 5; idx++);
            led_heartbeat_on();
        }
    }
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of app_stat.c
 ******************************************************************************/
