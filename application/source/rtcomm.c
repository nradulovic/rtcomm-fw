/*
 *  teslameter_3mhx-fw - 2017
 *
 *  rtcomm.c
 *
 *  Created on: May 27, 2015
 * ----------------------------------------------------------------------------
 *  This file is part of teslameter_3mhx-fw.
 *
 *  teslameter_3mhx-fw is free software: you can redistribute it and/or modify
 *  it under the terms of the Lesser GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  teslameter_3mhx-fw is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with teslameter_3mhx-fw.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------- *//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Real-time communication
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "config/hwcon.h"
#include "prim_gpio.h"
#include "status.h"
#include "rtcomm.h"
#include "status.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

struct rtcomm_handle			g_rtcomm;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static void notify_set(void)
{
	gpio_set(HWCON_RTCOMM_RRQ_PORT, HWCON_RTCOMM_RRQ_PIN);
}

static void notify_reset(void)
{
	gpio_clr(HWCON_RTCOMM_RRQ_PORT, HWCON_RTCOMM_RRQ_PIN);
}

static void unlock_dma(struct rtcomm_handle * handle)
{
	handle->state = STATE_RESET_DMA;

	/* NOTE:
	 * HAL_SPI_DMAStop() always returns HAL_OK: no need to check return value.
	 */
	HAL_SPI_DMAStop(&handle->spi);
	__HAL_UNLOCK(&handle->spi);
	handle->state = STATE_IDLE;
}

static void setup_dma(struct rtcomm_handle * handle)
{
	HAL_StatusTypeDef error;

	handle->state = STATE_SETUP_DMA;

	error = HAL_SPI_Transmit_DMA(&handle->spi, handle->storage_b, handle->size);

	if (error) {
		status_error(STATUS_RUNTIME_CHECK_FAILED);
	}
	handle->state = STATE_SENDING;
}

static void * peek_buffer(struct rtcomm_handle * handle)
{
    if (handle->state == STATE_IDLE) {
        handle->state = STATE_PREP_DATA;

        return (handle->storage_b);
    } else {
    	status_warn(STATUS_RTCOMM_SKIPPED_ERROR);

        return (NULL);
    }
}

static void invoke_sending_task_fn(void * arg)
{
	struct rtcomm_handle *		rtcomm = arg;

	ntask_ready_i(&rtcomm->sending_task);
}

static void emit_buffer(struct rtcomm_handle * handle)
{
	notify_reset();

	if (handle->state == STATE_PREP_DATA) {
		setup_dma(handle);
	} else {
		status_warn(STATUS_RTCOMM_SKIPPED_ERROR);
        /*
         * We are trying to send new buffer to consumer but the consumer has not
         * read us yet. In this case do the following:
         * - do NOT reset DMA
         * - do NOT setup DMA since it is already setup
         */
	}
	notify_set();
}

static void sending_task_fn(struct ntask * task, void * arg)
{
	struct rtcomm_handle *		rtcomm = arg;
    void * 						buffer = peek_buffer(rtcomm);

    if (buffer) {
    	rtcomm_pre_send(buffer);
    }
    emit_buffer(rtcomm);
    ntask_block(task);
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void rtcomm_init(struct rtcomm_handle * handle, void * storage_a,
		void * storage_b, uint16_t size)
{
	static const struct ntask_define sending_task_define =
	{
		.name = "sending task",
		.priority = 31,
		.vf_task = sending_task_fn
	};
	handle->storage_a = storage_a;
	handle->storage_b = storage_b;
	handle->size = size;
	handle->state = STATE_IDLE;
	ntask_init(&handle->sending_task, &sending_task_define, handle);
	nsched_deferred_init(&handle->invoke_sending_task, invoke_sending_task_fn,
			handle);
}



void rtcomm_clear(struct rtcomm_handle * handle)
{
	memset(handle->storage_a, 0, handle->size);
	memset(handle->storage_b, 0, handle->size);
}



void rtcomm_release_new(struct rtcomm_handle * handle)
{
    if (handle->state == STATE_IDLE) {
        /*
         * Swap buffers
         */
        register void * tmp;

        tmp = handle->storage_a;
        handle->storage_a = handle->storage_b;
        handle->storage_b = tmp;
    } else {
    	status_warn(STATUS_RTCOMM_SKIPPED_ERROR);
        /*
         * We are trying to send new buffer to consumer but the consumer has not
         * read us yet. In this case do the following:
         * - return the pointer to the same buffer (no swap)
         */
    }
    nsched_deferred_do(&handle->invoke_sending_task);
}



void rtcomm_isr_complete(struct rtcomm_handle * handle)
{
	if (handle->state != STATE_SENDING) {
		status_warn(STATUS_RTCOMM_COMPLETE_ERROR);
	}
	notify_reset();
	handle->state = STATE_IDLE;
}



void rtcomm_isr_error(struct rtcomm_handle * handle)
{
	if (handle->state != STATE_IDLE) {
		status_warn(STATUS_RTCOMM_TRANSFER_ERROR);

		if (handle->state == STATE_SENDING) {
			unlock_dma(handle);
		}
	}
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of rtcomm.c
 ******************************************************************************/
