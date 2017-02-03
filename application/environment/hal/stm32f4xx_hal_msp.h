/*
 * stm32f4xx_hal_msp.h
 *
 *  Created on: May 27, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_ENVIRONMENT_HAL_STM32F4XX_HAL_MSP_H_
#define APPLICATION_ENVIRONMENT_HAL_STM32F4XX_HAL_MSP_H_

#include "stm32f4xx_hal.h"

extern SPI_HandleTypeDef        g_ms_spi_handle;
extern DMA_HandleTypeDef        g_ms_spi_dma_tx_handle;

#endif /* APPLICATION_ENVIRONMENT_HAL_STM32F4XX_HAL_MSP_H_ */
