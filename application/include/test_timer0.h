/*
 *  rtcomm-fw - 2017
 *
 *  test_timer0.h
 *
 *  Created on: Feb 6, 2017
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
 * @brief       Test timer 0
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_TEST_TIMER0_H_
#define APPLICATION_INCLUDE_TEST_TIMER0_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "stm32f4xx_hal.h"

/*===============================================================  MACRO's  ==*/
/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

extern TIM_HandleTypeDef        g_test_timer0;

/*===================================================  FUNCTION PROTOTYPES  ==*/

void test_timer0_disable(void);

void test_timer0_enable(void);

/* NOTE:
 * This function shall be provided by application code.
 */
extern void test_timer0_callback(void);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of test_timer0.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_TEST_TIMER0_H_ */
