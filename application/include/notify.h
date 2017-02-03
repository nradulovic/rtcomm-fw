/*
 * notify.h
 *
 *  Created on: Mar 12, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_NOTIFY_H_
#define APPLICATION_INCLUDE_NOTIFY_H_

#include <stdbool.h>

void notify_init_as_output(void);
void notify_init_as_input(void);

bool notify_state(void);

#define notify_enable()                 gpio_set(NOTIFY_PORT, NOTIFY_PIN)
#define notify_disable()                gpio_clr(NOTIFY_PORT, NOTIFY_PIN)

#endif /* APPLICATION_INCLUDE_NOTIFY_H_ */
