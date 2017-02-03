/*
 * acq_gpio.h
 *
 *  Created on: May 26, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_PRIM_GPIO_H_
#define APPLICATION_INCLUDE_PRIM_GPIO_H_

#include "stm32f4xx.h"

#define gpio_set(port, pin)             (port)->BSRR = (pin);

#define gpio_clr(port, pin)             (port)->BSRR = (uint32_t)(pin) << 16u;

#define gpio_read(port, pin)            ((port)->IDR & pin)

#endif /* APPLICATION_INCLUDE_PRIM_GPIO_H_ */
