/*
 *  rtcomm-fw - 2017
 *
 *  prim_gpio.h
 *
 *  Created on: May 29, 2017
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
 * @brief       Primitive GPIO driver
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_PRIM_GPIO_H_
#define APPLICATION_INCLUDE_PRIM_GPIO_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "stm32f4xx.h"

/*===============================================================  MACRO's  ==*/

#define gpio_set(port, pin)             (port)->BSRR = (pin);

#define gpio_clr(port, pin)             (port)->BSRR = (uint32_t)(pin) << 16u;

#define gpio_read(port, pin)			((port)->IDR & pin)

/*------------------------------------------------------  C++ extern begin  --*/
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
/** @endcond *//** @} *//** @} *//*********************************************
 * END of prim_gpio.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_PRIM_GPIO_H_ */
