/*
 *  rtcomm-fw - 2017
 *
 *  status.c (former app_stat.c)
 *
 *  Created on: May 26, 2015
 * ----------------------------------------------------------------------------
 *  This file is part of rtcomm-fw.
 *
 *  rtcomm-fw is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  rtcomm-fw is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with rtcomm-fw.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------- *//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Status
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "status.h"
#include "config/hwcon.h"
#include "prim_gpio.h"
#include "base/bitop.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define LOOP_COUNT                      (50ul * 100000ul)

#define led_heartbeat_on()                                                      \
    gpio_set(HWCON_HEARTBEAT_PORT, HWCON_HEARTBEAT_PIN)

#define led_heartbeat_off()                                                     \
    gpio_clr(HWCON_HEARTBEAT_PORT, HWCON_HEARTBEAT_PIN)


/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

uint32_t                        g_status_counters[_STATUS_LAST_ID];

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void status_warn(enum status_msg warn)
{
    if (warn > NARRAY_DIMENSION(g_status_counters)) {
        g_status_counters[STATUS_FATAL_UNHANDLED_EXCP]++;
    } else {
        g_status_counters[warn]++;
    }
}

void status_error(enum status_msg error)
{
    uint32_t                    counter;
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
 * END of status.c
 ******************************************************************************/
