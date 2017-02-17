/*
 * test_timer.h
 *
 *  Created on: Feb 6, 2017
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_TEST_TIMER0_H_
#define APPLICATION_INCLUDE_TEST_TIMER0_H_

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef        g_test_timer0;

void test_timer0_disable(void);
void test_timer0_enable(void);

/* NOTE:
 * This function shall be provided by application code.
 */
extern void test_timer0_callback(void);


#endif /* APPLICATION_INCLUDE_TEST_TIMER0_H_ */
