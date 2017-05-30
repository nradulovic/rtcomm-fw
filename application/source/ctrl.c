/*
 * ctrl.c
 *
 *  Created on: May 29, 2017
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       I2C CTRL driver
 *********************************************************************//** @{ */
/*
 *
 */

/*=========================================================  INCLUDE FILES  ==*/

#include "ctrl.h"
#include "stm32f4xx_hal.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

struct ctrl 					g_ctrl;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void ctrl_start_rx(struct ctrl * ctrl, void * buffer, uint16_t size)
{
	HAL_StatusTypeDef			status = HAL_BUSY;

	if (HAL_I2C_GetState(&ctrl->i2c) == HAL_I2C_STATE_READY) {
		ctrl->buffer = buffer;
		ctrl->size = size;
		status = HAL_I2C_Slave_Receive_IT(&ctrl->i2c, (uint8_t *)buffer, size);
	}

	if (status != HAL_OK) {
		ctrl_error(ctrl, 0u);
	}
}

void ctrl_start_tx(struct ctrl * ctrl, const void * buffer, uint16_t size)
{
	HAL_StatusTypeDef			status = HAL_BUSY;

	if (HAL_I2C_GetState(&ctrl->i2c) == HAL_I2C_STATE_ERROR) {
		ctrl->buffer = (void *)buffer;
		ctrl->size = size;
		status = HAL_I2C_Slave_Transmit_IT(&ctrl->i2c, (uint8_t *)buffer, size);
	}

	if (status != HAL_OK) {
		ctrl_error(ctrl, 0u);
	}
}

void ctrl_isr_rx_complete(struct ctrl * ctrl)
{
	ctrl_rx_complete(ctrl, ctrl->buffer, ctrl->size);
}

void ctrl_isr_tx_complete(struct ctrl * ctrl)
{
	ctrl_tx_complete(ctrl, ctrl->buffer, ctrl->size);
}

void ctrl_isr_error(struct ctrl * ctrl)
{
	ctrl_error(ctrl, HAL_I2C_GetError(&ctrl->i2c));
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of ctrl.c
 ******************************************************************************/
