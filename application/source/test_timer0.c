/*
 *  teslameter_3mhx-fw - 2017
 *
 *  test_timer0.c
 *
 *  Created on: Feb 6, 2017
 * ----------------------------------------------------------------------------
 *  This file is part of teslameter_3mhx-fw.
 *
 *  teslameter_3mhx-fw is free software: you can redistribute it and/or modify
 *  it under the terms of the Lesser GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  teslameter_3mhx-fw is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with teslameter_3mhx-fw.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------- *//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Test timer 0
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "test_timer0.h"
#include "stm32f4xx_hal.h"
#include "config/hwcon.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

TIM_HandleTypeDef               g_test_timer0;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void test_timer0_disable(void)
{
    HAL_TIM_Base_Stop_IT(&g_test_timer0);
}

void test_timer0_enable(void)
{
    HAL_TIM_Base_Start_IT(&g_test_timer0);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of test_timer0.c
 ******************************************************************************/

