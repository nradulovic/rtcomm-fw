/*
 * ctrl_bus.c
 *
 *  Created on: May 26, 2015
 *      Author: nenad
 */

#include <string.h>

#include "ctrl_bus.h"
#include "hw_config.h"
#include "cdi/io.h"
#include "port/core.h"

struct ctrl_bus_ctx
{
    const struct ctrl_bus_client * client;
};


static I2C_HandleTypeDef        g_ctrl_i2c_handle;
static struct ctrl_bus_ctx      g_context;



static uint16_t parse_error(I2C_HandleTypeDef * i2c)
{
	uint16_t					retval;

	retval = 0;

	if (i2c->ErrorCode & HAL_I2C_ERROR_BERR) {
		retval |= CTRL_BUS_ERROR_BUS;
	}

	if (i2c->ErrorCode & HAL_I2C_ERROR_AF) {
		retval |= CTRL_BUS_ERROR_ACKNOWLEDGE;
	}

	if (i2c->ErrorCode & HAL_I2C_ERROR_ARLO) {
		retval |= CTRL_BUS_ERROR_ARBITRATION;
	}

	if (retval == 0) {
		retval = CTRL_BUS_ERROR_BUS;
	}

	return (retval);
}



void ctrl_bus_init(const struct ctrl_bus_client * client)
{
	g_context.client = client;

	g_ctrl_i2c_handle.Instance              = I2C_CTRL;
	g_ctrl_i2c_handle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
	g_ctrl_i2c_handle.Init.ClockSpeed       = 100000;
	g_ctrl_i2c_handle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLED;
	g_ctrl_i2c_handle.Init.DutyCycle        = I2C_DUTYCYCLE_2;
	g_ctrl_i2c_handle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
	g_ctrl_i2c_handle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
	g_ctrl_i2c_handle.Init.OwnAddress1      = ACQUNITY_ID;
	g_ctrl_i2c_handle.Init.OwnAddress2      = 0xFE;

	HAL_I2C_Init(&g_ctrl_i2c_handle);
}



void ctrl_bus_term(void)
{
	HAL_I2C_DeInit(&g_ctrl_i2c_handle);
}



void ctrl_master_rx_start(
    uint8_t *                   buff,
    uint16_t                    size,
	uint16_t 					dev_id)
{
	if (HAL_I2C_Master_Receive_IT(&g_ctrl_i2c_handle, dev_id, buff, size) != HAL_OK) {
		ncore_lock 				lock;

		ncore_lock_enter(&lock);
		g_context.client->error_i(parse_error(&g_ctrl_i2c_handle), 0);
		ncore_lock_exit(&lock);
	}
}



void ctrl_master_tx_start(
    uint8_t *                   buff,
    uint16_t                    size,
	uint16_t 					dev_id)
{
	HAL_StatusTypeDef			error;

	(error = HAL_I2C_Master_Transmit_IT(&g_ctrl_i2c_handle, dev_id, buff, size));

	if (error != HAL_OK) {
		ncore_lock 				lock;

		ncore_lock_enter(&lock);
		g_context.client->error_i(parse_error(&g_ctrl_i2c_handle), 0);
		ncore_lock_exit(&lock);
	}
}



void ctrl_bus_cancel(void)
{
	ncore_lock 					lock;
	uint16_t                    transferred;

	transferred = (uint16_t)(g_ctrl_i2c_handle.XferSize - g_ctrl_i2c_handle.XferCount);

	HAL_I2C_Master_Abort_IT(&g_ctrl_i2c_handle);
	HAL_I2C_DeInit(&g_ctrl_i2c_handle);
	HAL_I2C_Init(&g_ctrl_i2c_handle);

	ncore_lock_enter(&lock);
	g_context.client->complete_i(transferred);
	ncore_lock_exit(&lock);
}


/* -------------------------------------------------------------------------- *
 * I2C IRQ handlers
 * -------------------------------------------------------------------------- */

void I2C_CTRL_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&g_ctrl_i2c_handle);
}



void I2C_CTRL_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&g_ctrl_i2c_handle);
}


/* -------------------------------------------------------------------------- *
 * I2C callbacks
 * -------------------------------------------------------------------------- */

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	uint16_t                    transferred;

	transferred = (uint16_t)(hi2c->XferSize - hi2c->XferCount);

    g_context.client->complete_i(transferred);
}



void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	uint16_t                    transferred;

	transferred = (uint16_t)(hi2c->XferSize - hi2c->XferCount);

	g_context.client->complete_i(transferred);
}



void HAL_I2C_ErrorCallback(I2C_HandleTypeDef * hi2c)
{
	uint16_t                    transferred;

	transferred = (uint16_t)(hi2c->XferSize - hi2c->XferCount);

    g_context.client->error_i(parse_error(hi2c), transferred);
}
