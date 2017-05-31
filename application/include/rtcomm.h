/*
 * rtcomm.h
 *
 *  Created on: May 27, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_RTCOMM_H_
#define APPLICATION_INCLUDE_RTCOMM_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>
#include <stdbool.h>

#include "cdi/cdi_rtcomm.h"
#include "stm32f4xx_hal.h"
#include "neon_eds.h"

/*===============================================================  MACRO's  ==*/

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================  DATA TYPES  ==*/

enum rtcomm_state
{
	STATE_INIT,
	STATE_IDLE,
	STATE_PREP_DATA,
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
	struct rtcomm_stats         counters;
	struct ntask				sending_task;
	struct nsched_deferred		invoke_sending_task;
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct rtcomm_handle		g_rtcomm;

/*===================================================  FUNCTION PROTOTYPES  ==*/

void rtcomm_init(struct rtcomm_handle * handle, void * storage_a,
		void * storage_b, uint16_t size);

static inline
void * rtcomm_request_new(struct rtcomm_handle * handle)
{
	return (handle->storage_a);
}

/*
 * At this point a producer needs new buffer to write to. It is RTCOMMs
 * responsibility to obtain a new buffer while sending the current one to the
 * consumer.
 */
void rtcomm_release_new(struct rtcomm_handle * handle);

extern void rtcomm_pre_send(void * buffer);

void rtcomm_emit(struct rtcomm_handle * handle);
void rtcomm_isr_complete(struct rtcomm_handle * handle);
void rtcomm_isr_error(struct rtcomm_handle * handle);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of rtcomm.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_RTCOMM_H_ */
