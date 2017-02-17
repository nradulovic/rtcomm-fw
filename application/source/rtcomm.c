/*
 * ms_bus.c
 *
 *  Created on: May 27, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Master Bus implementation
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "hwcon.h"
#include "rtcomm.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

SPI_HandleTypeDef               g_rtcomm_spi;
DMA_HandleTypeDef               g_rtcomm_spi_dma_tx;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/


void ms_bus_start_tx(const void * data, uint16_t size)
{
    HAL_SPI_Transmit_DMA(&g_rtcomm_spi, (uint8_t *)data, size);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of ms_bus.c
 ******************************************************************************/
