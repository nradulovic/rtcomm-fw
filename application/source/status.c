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
#include "config/hwcon.h"
#include "prim_gpio.h"
#include "base/bitop.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define LOOP_COUNT                      (50ul * 100000ul)

#define led_heartbeat_on()														\
	gpio_set(HWCON_HEARTBEAT_PORT, HWCON_HEARTBEAT_PIN)

#define led_heartbeat_off()                                                     \
    gpio_clr(HWCON_HEARTBEAT_PORT, HWCON_HEARTBEAT_PIN)


/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

uint32_t 						g_status_counters[7];

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void status_warn(uint32_t error)
{
	if (error > NARRAY_DIMENSION(g_status_counters)) {
		g_status_counters[STATUS_RUNTIME_CHECK_FAILED]++;
		g_status_counters[STATUS_UNHANDLED_EXCP]++;
	} else {
		g_status_counters[error]++;
	}
}

void status_error(uint32_t error)
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
