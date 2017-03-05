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
    status_panic(STATUS_UNHANDLED_EXCP);
}

void HardFault_Handler(void)
{
	status_panic(STATUS_UNHANDLED_EXCP);
}

void MemManage_Handler(void)
{
	status_panic(STATUS_UNHANDLED_EXCP);
}

void BusFault_Handler(void)
{
	status_panic(STATUS_UNHANDLED_EXCP);
}

void UsageFault_Handler(void)
{
	status_panic(STATUS_UNHANDLED_EXCP);
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

/*
 * This interrupt is defined inside Neon
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


/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of stm32f4xx_it.c
 ******************************************************************************/
