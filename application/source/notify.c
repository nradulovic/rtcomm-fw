/*
 * notify.c
 *
 *  Created on: Mar 12, 2015
 *      Author: nenad
 */

#include "notify.h"
#include "stm32f4xx.h"
#include "hw_config.h"

/* -------------------------------------------------------------------------- *
 * Notify GPIO
 * -------------------------------------------------------------------------- */

void notify_init_as_output(void)
{
	GPIO_InitTypeDef 			pin_config;

	NOTIFY_GPIO_CLK_ENABLE();
	pin_config.Mode  = GPIO_MODE_OUTPUT_PP;
	pin_config.Pin   = NOTIFY_PIN;
	pin_config.Pull  = GPIO_NOPULL;
	pin_config.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(NOTIFY_PORT, &pin_config);
	HAL_GPIO_WritePin(NOTIFY_PORT, NOTIFY_PIN, GPIO_PIN_RESET);
}



void notify_init_as_input(void)
{
	GPIO_InitTypeDef 			pin_config;

	NOTIFY_GPIO_CLK_ENABLE();
	pin_config.Mode  = GPIO_MODE_INPUT;
	pin_config.Pin   = NOTIFY_PIN;
	pin_config.Pull  = GPIO_PULLDOWN;
	pin_config.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(NOTIFY_PORT, &pin_config);
}



bool notify_state(void)
{
	if (HAL_GPIO_ReadPin(NOTIFY_PORT, NOTIFY_PIN) == GPIO_PIN_SET) {
		return (true);
	} else {
		return (false);
	}
}
