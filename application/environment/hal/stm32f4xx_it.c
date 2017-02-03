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

/**
 * @brief   This function handles NMI exception.
 */
void NMI_Handler(void)
{
    status_panic(0);
}

/**
 * @brief  This function handles Hard Fault exception.
 */
void HardFault_Handler(void)
{
    status_panic(1);
}

/**
 * @brief  This function handles Memory Manage exception.
 */
void MemManage_Handler(void)
{
    status_panic(2);
}

/**
 * @brief  This function handles Bus Fault exception.
 */
void BusFault_Handler(void)
{
    status_panic(3);
}

/**
 * @brief  This function handles Usage Fault exception.
 */
void UsageFault_Handler(void)
{
    status_panic(4);
}

/**
 * @brief  This function handles SVCall exception.
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 */
void DebugMon_Handler(void)
{
}

/*
 * Used by Neon
 */
#if 0
/**
 * @brief  This function handles PendSVC exception.
 */
void PendSV_Handler(void)
{
}
#endif

/**
 * @brief  This function handles SysTick Handler.
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}



/* -------------------------------------------------------------------------- *
 * EXTI IRQ handler
 * -------------------------------------------------------------------------- */
#include "aux_channels.h"
#include "acq_channels.h"
#include "prim_spi.h"


void ACQ_AUX_DRDY_EXTI_Handler(void)
{
	acq_aux_drdy_isr();
}

/* -------------------------------------------------------------------------- *
 * SPI IRQ handler
 * -------------------------------------------------------------------------- */


void ACQ_AUX_SPI_IRQ_Handler(void)
{
    spi_host_isr(&ACQ_AUX_SPI);
}

/* -------------------------------------------------------------------------- *
 * EXTI IRQ handlers
 * -------------------------------------------------------------------------- */


void ACQ_0_DRDY_EXTI_Handler(void)
{
	acq_x_drdy_isr();
	__HAL_GPIO_EXTI_CLEAR_IT(ACQ_0_DRDY_PIN);
}



void ACQ_1_DRDY_EXTI_Handler(void)
{
	acq_x_drdy_isr();
	__HAL_GPIO_EXTI_CLEAR_IT(ACQ_1_DRDY_PIN);
}



void ACQ_2_DRDY_EXTI_Handler(void)
{
	acq_x_drdy_isr();
	__HAL_GPIO_EXTI_CLEAR_IT(ACQ_2_DRDY_PIN);
}

/* -------------------------------------------------------------------------- *
 * SPI IRQ handlers
 * -------------------------------------------------------------------------- */


void ACQ_0_SPI_IRQ_Handler(void)
{
    spi_host_isr(&ACQ_0_SPI);
}



void ACQ_1_SPI_IRQ_Handler(void)
{
    spi_host_isr(&ACQ_1_SPI);
}



void ACQ_2_SPI_IRQ_Handler(void)
{
    spi_host_isr(&ACQ_2_SPI);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of stm32f4xx_it.c
 ******************************************************************************/
