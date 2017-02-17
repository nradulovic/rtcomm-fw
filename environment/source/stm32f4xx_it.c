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

#include "status.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
#include "rtcomm.h"
#include "hwcon.h"

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
    status_panic(0);
}

void HardFault_Handler(void)
{
	status_panic(0);
}

void MemManage_Handler(void)
{
	status_panic(0);
}

void BusFault_Handler(void)
{
	status_panic(0);
}

void UsageFault_Handler(void)
{
	status_panic(0);
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


void HWCON_RTCOMM_SPI_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_rtcomm.dma_tx);
}

#if defined(TEST_MS_BUS_INCS)
void HWCON_TEST_TIMER0_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&g_test_timer0);
}
#endif


/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of stm32f4xx_it.c
 ******************************************************************************/
