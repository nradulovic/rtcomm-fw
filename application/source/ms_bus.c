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

#include <string.h>

#include "hw_config.h"
#include "ms_bus.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/

static struct ms_bus_config     g_config;

/*======================================================  GLOBAL VARIABLES  ==*/

SPI_HandleTypeDef               g_ms_spi_handle;
DMA_HandleTypeDef               g_ms_spi_dma_tx_handle;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/


void ms_bus_init(const struct ms_bus_config * config)
{
    memcpy(&g_config, config, sizeof(g_config));
    g_ms_spi_handle.Instance = SPI_MS;
    g_ms_spi_handle.Init.BaudRatePrescaler = SPI_MS_BAUD_CLOCK;
    g_ms_spi_handle.Init.Direction         = SPI_DIRECTION_2LINES;
    g_ms_spi_handle.Init.CLKPhase          = SPI_MS_CLK_PHASE;
    g_ms_spi_handle.Init.CLKPolarity       = SPI_MS_CLK_POL;
    g_ms_spi_handle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLED;
    g_ms_spi_handle.Init.CRCPolynomial     = 7;
    g_ms_spi_handle.Init.DataSize          = SPI_DATASIZE_8BIT;
    g_ms_spi_handle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    g_ms_spi_handle.Init.NSS               = SPI_NSS_HARD_INPUT;
    g_ms_spi_handle.Init.TIMode            = SPI_TIMODE_DISABLED;
    g_ms_spi_handle.Init.Mode              = SPI_MODE_SLAVE;
    HAL_SPI_Init(&g_ms_spi_handle);
}



void ms_bus_start_tx(const void * data, uint16_t size)
{
    HAL_SPI_Transmit_DMA(&g_ms_spi_handle, (uint8_t *)data, size);
}



/******************************************************************************
 * DMA IRQ handlers
 ******************************************************************************/

void SPI_MS_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_ms_spi_dma_tx_handle);
}

/******************************************************************************
 * SPI callbacks
 ******************************************************************************/


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef * hspi)
{
    /* NOTE:
     * Since this is the only SPI we ignore the handle
     */
    (void)hspi;
    g_config.tx_complete();
}



void HAL_SPI_ErrorCallback(SPI_HandleTypeDef * hspi)
{
    /* NOTE:
     * Since this is the only SPI we ignore the handle
     */
    (void)hspi;

    /* NOTE:
     * If get here it probably means that the master didn't read us.
     * In that case reset the DMA & SPI and buffers
     */
    hspi->hdmatx->ErrorCode = HAL_DMA_ERROR_NONE;
    hspi->ErrorCode         = HAL_SPI_ERROR_NONE;
    g_config.tx_error();
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of ms_bus.c
 ******************************************************************************/


