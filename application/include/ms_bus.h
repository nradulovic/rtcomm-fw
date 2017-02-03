/*
 * ms_bus.h
 *
 *  Created on: May 27, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_MS_BUS_H_
#define APPLICATION_INCLUDE_MS_BUS_H_

#include "hw_config.h"

struct ms_bus_config
{
    void                     (* tx_complete)(void);
    void                     (* tx_error)(void);
};

void ms_bus_init(const struct ms_bus_config * config);
void ms_bus_start_tx(const void * data, uint16_t size);

void SPI_MS_DMA_TX_IRQHandler(void);

#endif /* APPLICATION_INCLUDE_MS_BUS_H_ */
