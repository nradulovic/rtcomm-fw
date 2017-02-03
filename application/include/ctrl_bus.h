/*
 * ctrl_bus.h
 *
 *  Created on: May 26, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_CTRL_BUS_H_
#define APPLICATION_INCLUDE_CTRL_BUS_H_

#include "hw_config.h"
#include <stdint.h>

#define CTRL_BUS_ERROR_ACKNOWLEDGE 		(0x1u << 0)
#define CTRL_BUS_ERROR_BUS		        (0x1u << 1)
#define CTRL_BUS_ERROR_ARBITRATION		(0x1u << 2)

struct ctrl_bus_client
{
	void                     (* error_i)(uint16_t error, uint16_t transferred);
	void					 (* complete_i)(uint16_t);
};

void ctrl_bus_init(const struct ctrl_bus_client * client);



void ctrl_bus_term(void);



void ctrl_master_rx_start(
    uint8_t *                   buff,
    uint16_t                    size,
	uint16_t 					dev_id);



void ctrl_master_tx_start(
    uint8_t *                   buff,
    uint16_t                    size,
	uint16_t 					dev_id);


void ctrl_bus_cancel(void);

void I2C_CTRL_EV_IRQHandler(void);
void I2C_CTRL_ER_IRQHandler(void);

#endif /* APPLICATION_INCLUDE_CTRL_BUS_H_ */
