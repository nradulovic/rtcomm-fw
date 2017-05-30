/*
 * ctrl.h
 *
 *  Created on: May 30, 2017
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_CTRL_H_
#define APPLICATION_INCLUDE_CTRL_H_

#include "stm32f4xx_hal.h"

struct ctrl
{
	I2C_HandleTypeDef			i2c;
	void *						buffer;
	uint16_t					size;
};

extern struct ctrl 				g_ctrl;

void ctrl_start_rx(struct ctrl * ctrl, void * buffer, uint16_t size);

void ctrl_start_tx(struct ctrl * ctrl, const void * buffer, uint16_t size);

/* -- Methods to be used by ISRs -------------------------------------------- */

void ctrl_isr_rx_complete(struct ctrl * ctrl);

void ctrl_isr_tx_complete(struct ctrl * ctrl);

void ctrl_isr_error(struct ctrl * ctrl);

/* -- Callbacks ------------------------------------------------------------- */

extern void ctrl_rx_complete(struct ctrl * ctrl, void * buffer, uint16_t size);

extern void ctrl_tx_complete(struct ctrl * ctrl, void * buffer, uint16_t size);

extern void ctrl_error(struct ctrl * ctrl, uint32_t error);

#endif /* APPLICATION_INCLUDE_CTRL_H_ */
