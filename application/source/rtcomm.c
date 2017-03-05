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

#include "hwcon.h"
#include "rtcomm.h"
#include "status.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

struct rtcomm_handle			g_rtcomm;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static
void notify_set(void)
{
	HAL_GPIO_WritePin(HWCON_NOTIFY_PORT, HWCON_NOTIFY_PIN, GPIO_PIN_SET);
}

static
void notify_reset(void)
{
	HAL_GPIO_WritePin(HWCON_NOTIFY_PORT, HWCON_NOTIFY_PIN, GPIO_PIN_RESET);
}

static
void reset_dma(struct rtcomm_handle * handle)
{
	handle->state = STATE_RESET_DMA;

	/* NOTE:
	 * HAL_SPI_DMAStop() always returns HAL_OK: no need to check return value.
	 */
	HAL_SPI_DMAStop(&handle->spi);
	__HAL_UNLOCK(&handle->spi);
	handle->state = STATE_IDLE;
}

static
void setup_dma(struct rtcomm_handle * handle)
{
	HAL_StatusTypeDef error;

	handle->state = STATE_SETUP_DMA;

	error = HAL_SPI_Transmit_DMA(&handle->spi, handle->storage_b, handle->size);

	if (error) {
		status_panic(STATUS_RUNTIME_CHECK_FAILED);
	}
	handle->state = STATE_SENDING;
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void rtcomm_init(struct rtcomm_handle * handle, void * storage_a,
		void * storage_b, uint16_t size)
{
	handle->storage_a = storage_a;
	handle->storage_b = storage_b;
	handle->size = size;
	handle->state = STATE_IDLE;
	handle->counter.complete_err = 0u;
	handle->counter.transfer_err = 0u;
	handle->counter.skipped_err = 0u;
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
        handle->counter.skipped_err++;
        /*
         * We are trying to send new buffer to consumer but the consumer has not
         * read us yet. In this case do the following:
         * - return the pointer to the same buffer (no swap)
         */
    }
}


void rtcomm_emit(struct rtcomm_handle * handle)
{
	notify_reset();

	if (handle->state == STATE_PREP_DATA) {
		setup_dma(handle);
	} else {
		handle->counter.skipped_err++;
        /*
         * We are trying to send new buffer to consumer but the consumer has not
         * read us yet. In this case do the following:
         * - do NOT reset DMA
         * - do NOT setup DMA since it is already setup
         */
	}
	notify_set();
}



void rtcomm_isr_complete(struct rtcomm_handle * handle)
{
	if (handle->state != STATE_SENDING) {
		handle->counter.complete_err++;
	}
	notify_reset();
	handle->state = STATE_IDLE;
}



void rtcomm_isr_error(struct rtcomm_handle * handle)
{
	if (handle->state != STATE_IDLE) {
		handle->counter.transfer_err++;

		if (handle->state == STATE_SENDING) {
			reset_dma(handle);
		}
	}
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of ms_bus.c
 ******************************************************************************/
