/*
 * ms_bus.h
 *
 *  Created on: May 27, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_RTCOMM_H_
#define APPLICATION_INCLUDE_RTCOMM_H_

#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"

enum rtcomm_state
{
	STATE_INIT,
	STATE_IDLE,
	STATE_RESET_DMA,
	STATE_SETUP_DMA,
	STATE_SENDING,
};

struct rtcomm_handle
{
	void *						storage_a;
	void *						storage_b;
	uint16_t					size;
	enum rtcomm_state			state;
	SPI_HandleTypeDef			spi;
	DMA_HandleTypeDef			dma_tx;
	struct rtcomm_stats
	{
		uint32_t					complete_err;
		uint32_t					transfer_err;
		uint32_t					skipped_err;
	}							counter;
};

extern struct rtcomm_handle		g_rtcomm;

void rtcomm_init(struct rtcomm_handle * handle, void * storage_a,
		void * storage_b, uint16_t size);

static inline
void * rtcomm_request(struct rtcomm_handle * handle)
{
	return (handle->storage_a);
}

/*
 * At this point a producer needs new buffer to write to. It is RTCOMMs
 * responsibility to obtain a new buffer while sending the current one to the
 * consumer.
 */
void rtcomm_push(struct rtcomm_handle * handle);
void rtcomm_isr_complete(struct rtcomm_handle * handle);
void rtcomm_isr_error(struct rtcomm_handle * handle);

#endif /* APPLICATION_INCLUDE_RTCOMM_H_ */
