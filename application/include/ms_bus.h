/*
 * ms_bus.h
 *
 *  Created on: May 27, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_MS_BUS_H_
#define APPLICATION_INCLUDE_MS_BUS_H_

#include <stdint.h>

void ms_bus_init(void);
void ms_bus_start_tx(const void * data, uint16_t size);

extern void ms_bus_complete_callback(void);
extern void ms_bus_error_callback(void);

#endif /* APPLICATION_INCLUDE_MS_BUS_H_ */
