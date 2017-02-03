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

#include <stdbool.h>

#include "app_stat.h"
#include "device_mem_map.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define LOOP_COUNT                      (50ul * 100000ul)

#define led_heartbeat_on()                                                      \
    gpio_set(LED_HARTBEAT_PORT, LED_HARTBEAT_PIN)

#define led_heartbeat_off()                                                     \
    gpio_clr(LED_HARTBEAT_PORT, LED_HARTBEAT_PIN)

/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/


void status_init(void)
{
}



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
