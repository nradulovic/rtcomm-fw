/*
 * trigger.c
 *
 *  Created on: Mar 12, 2015
 *      Author: nenad
 */

#include "acq_sync.h"
#include "prim_gpio.h"
#include "stm32f4xx_hal.h"
#include "hw_config.h"

void acq_sync_init(void)
{
    GPIO_InitTypeDef            pin_config;

    SYNC_CLK_ENABLE();
    pin_config.Mode  = GPIO_MODE_OUTPUT_PP;
    pin_config.Pin   = SYNC_PIN;
    pin_config.Pull  = GPIO_NOPULL;
    pin_config.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(SYNC_PORT, &pin_config);
    gpio_clr(SYNC_PORT, SYNC_PIN);
    HAL_Delay(10);
    gpio_set(SYNC_PORT, SYNC_PIN);
}

