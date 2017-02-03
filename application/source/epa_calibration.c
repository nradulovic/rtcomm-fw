/*
 * epa_calibration.c
 *
 *  Created on: July 14, 2016
 *      Author: nenad
 */


/*=========================================================  INCLUDE FILES  ==*/

#include "epa_calibration.h"
#include "device_mem_map.h"
#include "epa_eeprom.h"
#include "epa_i2c.h"
#include "nv_data.h"
#include "main.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define CALIB_HEADER_BASE				8192
#define CALIB_HEADER_MAGICK				"Calibration 1.0"
#define CALIB_DATA_SIZE					8192

/*======================================================  LOCAL DATA TYPES  ==*/

struct wspace
{
	void * 						local_buff;
	struct nepa *				producer;
    uint32_t   					calib;
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static naction state_init           (struct nsm *, const struct nevent *);
static naction state_idle    		(struct nsm *, const struct nevent *);
static naction state_fetch_data		(struct nsm *, const struct nevent *);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct nevent *          g_calibration_queue_storage[EPA_CALIBRATION_QUEUE_SIZE];
static struct wspace         	g_calibration_workspace;

/*======================================================  GLOBAL VARIABLES  ==*/

struct nepa                     g_epa_calibration;
const struct nepa_define        g_epa_calibration_define = NEPA_DEF_INIT(
		&g_calibration_workspace,
		&state_init,
		NSM_TYPE_FSM,
		g_calibration_queue_storage,
		sizeof(g_calibration_queue_storage),
		"Calibration",
		EPA_CALIBRATION_PRIO);

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/


static bool
is_calibration_valid(void)
{
	char 						header[sizeof(CALIB_HEADER_MAGICK)];

	nv_data_fast_read_block(header, sizeof(CALIB_HEADER_MAGICK), CALIB_HEADER_BASE);

	if (strcmp(header, CALIB_HEADER_MAGICK) == 0) {
		return (true);
	} else {
		return (false);
	}
}

/******************************************************************************
 * State machine
 ******************************************************************************/

static naction state_init(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
		case NSM_INIT : {		

			return (naction_transit_to(sm, state_idle));
		}
		default : {
			return (naction_ignored());
		}
	}
}



static naction state_idle(struct nsm * sm, const struct nevent * event)
{
	struct wspace *				ws = nsm_wspace(sm);

	switch (event->id) {
		case SIG_CALIBRATION_INIT: {

			if (is_calibration_valid()) {
				nepa_send_signal(event->producer, SIG_CALIBRATION_READY);

				mem_map()->active[DEV_ACTIVE_CALIBRATION] = true;

				return (naction_handled());
			} else {
				ws->producer = event->producer;

				return (naction_transit_to(sm, state_fetch_data));
			}
		}
		default : {
			return (naction_ignored());
		}
	}
}



static naction state_fetch_data(struct nsm * sm, const struct nevent * event)
{
	struct wspace *				ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_INIT: {
			struct event_eeprom_transfer * transfer;

			ws->local_buff = nheap_alloc(&g_generic_heap, CALIB_DATA_SIZE);

			if (!ws->local_buff) {
				nepa_send_signal(ws->producer, SIG_CALIBRATION_NOT_READY);

				return (naction_transit_to(sm, state_idle));
			}
			transfer = NEVENT_CREATE(struct event_eeprom_transfer,
					EVENT_EEPROM_READ);

			if (!transfer) {
				nepa_send_signal(ws->producer, SIG_CALIBRATION_NOT_READY);

				return (naction_transit_to(sm, state_idle));
			}
			transfer->address = 0;
			transfer->buffer = ws->local_buff;
			transfer->size = CALIB_DATA_SIZE;
			nepa_send_event(&g_epa_eeprom, &transfer->super);

			return (naction_handled());
		}
		case NSM_EXIT: {
			if (ws->local_buff) {
				nheap_free(&g_generic_heap, ws->local_buff);
			}

			return (naction_handled());
		}
		case SIG_EEPROM_READY: {
			nerror 				error;

			error = nv_data_erase_all();

			if (error) {
				nepa_send_signal(ws->producer, SIG_CALIBRATION_NOT_READY);

				return (naction_transit_to(sm, state_idle));
			}
			error = nv_data_write_block_raw(ws->local_buff, CALIB_DATA_SIZE, 0);

			if (error) {
				nepa_send_signal(ws->producer, SIG_CALIBRATION_NOT_READY);

				return (naction_transit_to(sm, state_idle));
			}
			error = nv_data_write_block_raw(CALIB_HEADER_MAGICK,
					sizeof(CALIB_HEADER_MAGICK), CALIB_HEADER_BASE);

			if (error) {
				nepa_send_signal(ws->producer, SIG_CALIBRATION_NOT_READY);

				return (naction_transit_to(sm, state_idle));
			}
			nepa_send_signal(ws->producer, SIG_CALIBRATION_READY);

			mem_map()->active[DEV_ACTIVE_CALIBRATION] = true;

			return (naction_transit_to(sm, state_idle));
		}
		case SIG_EEPROM_NOT_READY: {
			nepa_send_signal(ws->producer, SIG_CALIBRATION_NOT_READY);

			return (naction_transit_to(sm, state_idle));
		}
		default : {
			return (naction_ignored());
		}
	}
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of epa_calibration.c
 ******************************************************************************/
