/*
 * This file is part of teslameter_3mhx-fw.
 *
 * Copyright (C) 2010 - 2017 nenad
 *
 * teslameter_3mhx-fw is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * teslameter_3mhx-fw is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with teslameter_3mhx-fw.  If not, see <http://www.gnu.org/licenses/>.
 *
 * web site:    
 * e-mail  :    
 *//***********************************************************************//**
 * @file
 * @author      nenad
 * @brief       Brief description
 * @defgroup    def_group Name
 * @brief       Brief description
 *********************************************************************//** @{ */

#ifndef ENVIRONMENT_INCLUDE_NEON_EDS_APP_CONFIG_H_
#define ENVIRONMENT_INCLUDE_NEON_EDS_APP_CONFIG_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "hwcon.h"

/*===============================================================  MACRO's  ==*/

#define CONFIG_DEBUG                    1
#define CONFIG_API_VALIDATION           1
#define CONFIG_ASSERT_INTERNAL          1

#define CONFIG_CORE_TIMER_SOURCE        5

/* NOTE:
 * Timer 5 is allocated to APB1 with prescaler 2
 */
#define CONFIG_CORE_TIMER_CLOCK_FREQ    (SystemCoreClock / 2)

#define CONFIG_CORE_TIMER_EVENT_FREQ    1000

/* NOTE:
 * Some ISR will have higher level of priority than Neon ISRs
 */
#define CONFIG_CORE_LOCK_MAX_LEVEL      NCORE_CODE_TO_LOCK(IRQ_PRIO_NEON)

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
