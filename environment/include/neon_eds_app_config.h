/*
 * This file is part of rtcomm-fw.
 *
 * Copyright (C) 2010 - 2017 nenad
 *
 * rtcomm-fw is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rtcomm-fw is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rtcomm-fw.  If not, see <http://www.gnu.org/licenses/>.
 *
 * web site:    
 * e-mail  :    
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Configuration for Neon
 *********************************************************************//** @{ */

#ifndef ENVIRONMENT_INCLUDE_NEON_EDS_APP_CONFIG_H_
#define ENVIRONMENT_INCLUDE_NEON_EDS_APP_CONFIG_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "config/hwcon.h"

/*===============================================================  MACRO's  ==*/

#define CONFIG_DEBUG                    0
#define CONFIG_API_VALIDATION           1
#define CONFIG_ASSERT_INTERNAL          1

#define CONFIG_EVENT_SIZE               1

#define CONFIG_CORE_TIMER_SOURCE        5

/* NOTE:
 * Timer 5 is allocated to APB1 with prescaler 2
 */
#define CONFIG_CORE_TIMER_CLOCK_FREQ    (SystemCoreClock / 2)

#define CONFIG_CORE_TIMER_EVENT_FREQ    1000

/* NOTE:
 * Some ISR will have higher level of priority than Neon ISRs
 */
#define CONFIG_CORE_LOCK_MAX_LEVEL      NCORE_CODE_TO_LOCK(HWCON_IRQ_PRIO_NEON)

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/
/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of neon_eds_app_config.h
 ******************************************************************************/
#endif /* ENVIRONMENT_INCLUDE_NEON_EDS_APP_CONFIG_H_ */
