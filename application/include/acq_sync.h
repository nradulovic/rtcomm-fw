/*
 * trigger.h
 *
 *  Created on: Mar 12, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_ACQ_SYNC_H_
#define APPLICATION_INCLUDE_ACQ_SYNC_H_

#include "prim_gpio.h"
#include "hw_config.h"

void acq_sync_init(void);

#define acq_sync_set_high()             gpio_set(SYNC_PORT, SYNC_PIN);


#define acq_sync_set_low()              gpio_clr(SYNC_PORT, SYNC_PIN);


#endif /* APPLICATION_INCLUDE_ACQ_SYNC_H_ */
