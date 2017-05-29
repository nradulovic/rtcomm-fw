/*
 * prim_gpio.h
 *
 *  Created on: May 29, 2017
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_PRIM_GPIO_H_
#define APPLICATION_INCLUDE_PRIM_GPIO_H_

#define gpio_set(port, pin)             (port)->BSRR = (pin);

#define gpio_clr(port, pin)             (port)->BSRR = (uint32_t)(pin) << 16u;

#endif /* APPLICATION_INCLUDE_PRIM_GPIO_H_ */
