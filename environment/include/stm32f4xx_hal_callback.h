/*
 * stm32f4xx_hal_callback.h
 *
 *  Created on: Feb 15, 2017
 *      Author: nenad
 */

#ifndef ENVIRONMENT_INCLUDE_STM32F4XX_HAL_CALLBACK_H_
#define ENVIRONMENT_INCLUDE_STM32F4XX_HAL_CALLBACK_H_

#include "stm32f4xx_hal.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim);
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef * hspi);
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef * hspi);

#endif /* ENVIRONMENT_INCLUDE_STM32F4XX_HAL_CALLBACK_H_ */
