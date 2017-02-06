/*
 * test_timer.h
 *
 *  Created on: Feb 6, 2017
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_TEST_TIMER_H_
#define APPLICATION_INCLUDE_TEST_TIMER_H_

HAL_StatusTypeDef test_timer_init(uint32_t TickPriority, void (* callback)(void));
void test_timer_disable(void);
void test_timer_enable(void);


#endif /* APPLICATION_INCLUDE_TEST_TIMER_H_ */
