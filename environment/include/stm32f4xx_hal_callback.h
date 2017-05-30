/*
 * stm32f4xx_hal_callback.h
 *
 *  Created on: Feb 15, 2017
 *      Author: nenad
 */

#ifndef ENVIRONMENT_INCLUDE_STM32F4XX_HAL_CALLBACK_H_
#define ENVIRONMENT_INCLUDE_STM32F4XX_HAL_CALLBACK_H_

#include "stm32f4xx_hal.h"

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef * hspi);
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef * hspi);

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim);

#endif /* ENVIRONMENT_INCLUDE_STM32F4XX_HAL_CALLBACK_H_ */
