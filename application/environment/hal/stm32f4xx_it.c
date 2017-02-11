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

#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
#include "app_stat.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void NMI_Handler(void)
{
    status_panic(0);
}

void HardFault_Handler(void)
{
    status_panic(1);
}

void MemManage_Handler(void)
{
    status_panic(2);
}

void BusFault_Handler(void)
{
    status_panic(3);
}

void UsageFault_Handler(void)
{
    status_panic(4);
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}


void SPI_MS_DMA_TX_IRQHandler(void)
{
	extern DMA_HandleTypeDef g_ms_spi_dma_tx_handle;

    HAL_DMA_IRQHandler(&g_ms_spi_dma_tx_handle);
}

void TIM2_IRQHandler(void)
{
	extern TIM_HandleTypeDef g_test_timer;

	HAL_TIM_IRQHandler(&g_test_timer);
}


/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of stm32f4xx_it.c
 ******************************************************************************/
