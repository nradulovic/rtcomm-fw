/*
 * stm32f4xx_hal_callback.c
 *
 *  Created on: Feb 15, 2017
 *      Author: nenad
 */


#include "epa_ctrl.h"
#include "config/hwcon.h"
#include "rtcomm.h"
#include "stm32f4xx_hal_callback.h"

#if defined(HWCON_TEST_TIMER0_ENABLE)
#include "test_timer0.h"
#endif



void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef * hspi)
{
    if (hspi == &g_rtcomm.spi) {
        rtcomm_isr_complete(&g_rtcomm);
    }
}



void HAL_SPI_ErrorCallback(SPI_HandleTypeDef * hspi)
{
    if (hspi == &g_rtcomm.spi) {
        rtcomm_isr_error(&g_rtcomm);
    }
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef * hi2c)
{
    if (hi2c == &g_ctrl.i2c) {
        ctrl_isr_tx_complete(&g_ctrl);
    }
}



void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef * hi2c)
{
    if (hi2c == &g_ctrl.i2c) {
        ctrl_isr_rx_complete(&g_ctrl);
    }
}



void HAL_I2C_ErrorCallback(I2C_HandleTypeDef * hi2c)
{
    if (hi2c == &g_ctrl.i2c) {
        ctrl_isr_error(&g_ctrl);
    }
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim)
{
#if defined(HWCON_TEST_TIMER0_ENABLE)
    if (htim == &g_test_timer0) {
        test_timer0_callback();
    }
#else
    (void)htim;
#endif
}
