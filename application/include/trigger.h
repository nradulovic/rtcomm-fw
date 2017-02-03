/*
 * trigger.h
 *
 *  Created on: Sep 15, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_TRIGGER_H_
#define APPLICATION_INCLUDE_TRIGGER_H_

#include <stdint.h>

#define TRIGGER_IN_RISE                 (0x1u << 0)
#define TRIGGER_IN_FALL                 (0x1u << 1)

void trigger_out_init(void);
void trigger_out_set_speed(uint32_t speed);
void trigger_out_enable(void);
void trigger_out_conditional_disable(void);
void trigger_out_disable(void);

void trigger_in_init(void);
void trigger_in_enable(bool (* fn)(void));
void trigger_in_disable(void);


void TRIGGER_IN_EXTI_Handler(void);
void TRIGGER_IN_TIM_IRQHandler(void);
void TRIGGER_OUT_TIM_IRQHandler(void);

#endif /* APPLICATION_INCLUDE_TRIGGER_H_ */
