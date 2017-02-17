/*
 * stm32f4xx_hal_callback.c
 *
 *  Created on: Feb 15, 2017
 *      Author: nenad
 */


#include "stm32f4xx_hal_callback.h"
#include "hwcon.h"
#include "rtcomm.h"

#if defined(HWCON_TEST_TIMER0_ENABLE)
#include "test_timer0.h"
#endif


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
#if defined(HWCON_TEST_TIMER0_ENABLE)
	if (htim == &g_test_timer0) {
		test_timer0_callback();
	}
#endif
}



void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef * hspi)
{
	extern SPI_HandleTypeDef g_rtcomm_spi;

    if (hspi == &g_rtcomm_spi) {
    	extern void ms_bus_complete_callback();

    	ms_bus_complete_callback();
    }
}



void HAL_SPI_ErrorCallback(SPI_HandleTypeDef * hspi)
{
	extern SPI_HandleTypeDef g_rtcomm_spi;

	if (hspi == &g_rtcomm_spi) {
		extern void ms_bus_error_callback();

		ms_bus_error_callback();
	}
}
