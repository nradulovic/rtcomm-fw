/*
 * stm32f4xx_it.c
 *
 *  Created on: May 25, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Interrupt routines
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "epa_ctrl.h"
#include "config/hwcon.h"
#include "status.h"
#include "rtcomm.h"
#include "acquisition.h"
#include "prim_spi.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"

#if defined(HWCON_TEST_TIMER0_ENABLE)
#include "test_timer0.h"
#endif

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void NMI_Handler(void)
{
    status_error(STATUS_FATAL_UNHANDLED_EXCP);
}

void HardFault_Handler(void)
{
	status_error(STATUS_FATAL_UNHANDLED_EXCP);
}

void MemManage_Handler(void)
{
	status_error(STATUS_FATAL_UNHANDLED_EXCP);
}

void BusFault_Handler(void)
{
	status_error(STATUS_FATAL_UNHANDLED_EXCP);
}

void UsageFault_Handler(void)
{
	status_error(STATUS_FATAL_UNHANDLED_EXCP);
}

void SVC_Handler(void)
{
	status_error(STATUS_FATAL_UNHANDLED_EXCP);
}

void DebugMon_Handler(void)
{
	/* NOTE:
	 * Leave this ISR free.
	 */
}

/* NOTE:
 * This interrupt is defined inside Neon so leave it undefined here.
 */
#if 0
void PendSV_Handler(void)
{
}
#endif

void SysTick_Handler(void)
{
    HAL_IncTick();
}


void HWCON_RTCOMM_SPI_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_rtcomm.dma_tx);
}

#if defined(HWCON_TEST_TIMER0_ENABLE)
void HWCON_TEST_TIMER0_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&g_test_timer0);
}
#endif

void HWCON_PROBE_X_DRDY_EXTI_Handler(void)
{
	acquisition_probe_drdy_isr(&g_probe);
	__HAL_GPIO_EXTI_CLEAR_IT(HWCON_PROBE_X_DRDY_PIN);
}

void HWCON_PROBE_X_SPI_IRQ_Handler(void)
{
    spi_bus_isr(&HWCON_PROBE_X_SPI);
}

void HWCON_PROBE_Y_DRDY_EXTI_Handler(void)
{
	acquisition_probe_drdy_isr(&g_probe);
	__HAL_GPIO_EXTI_CLEAR_IT(HWCON_PROBE_Y_DRDY_PIN);
}

void HWCON_PROBE_Y_SPI_IRQ_Handler(void)
{
    spi_bus_isr(&HWCON_PROBE_Y_SPI);
}

void HWCON_PROBE_Z_DRDY_EXTI_Handler(void)
{
	acquisition_probe_drdy_isr(&g_probe);
	__HAL_GPIO_EXTI_CLEAR_IT(HWCON_PROBE_Z_DRDY_PIN);
}

void HWCON_PROBE_Z_IRQ_Handler(void)
{
    spi_bus_isr(&HWCON_PROBE_Z_SPI);
}

void HWCON_AUX_DRDY_EXTI_Handler(void)
{
	/*
	 * TODO: Fill this with appropriate ISR
	 */
}

void HWCON_AUX_SPI_IRQ_Handler(void)
{
	/*
	 * TODO: Fill this with appropriate ISR
	 */
}

void HWCON_CTRL_I2C_EV_IRQHandler(void)
{
	HAL_I2C_EV_IRQHandler(&g_ctrl.i2c);
}

void HWCON_CTRL_I2C_ER_IRQHandler(void)
{
	HAL_I2C_ER_IRQHandler(&g_ctrl.i2c);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of stm32f4xx_it.c
 ******************************************************************************/
