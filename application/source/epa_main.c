/*
 * epa_main.c
 *
 *  Created on: May 25, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Main FSM implementation
 * @addtogroup  epa_main
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include <stddef.h>
#include <string.h>

#include "epa_main.h"
#include "epa_acq.h"
#include "epa_aux.h"
#include "epa_heartbeat.h"
#include "epa_calibration.h"
#include "epa_adt7410.h"
#include "epa_eeprom.h"
#include "epa_i2c.h"
#include "cdi/io.h"
#include "app_stat.h"
#include "ppbuff.h"
#include "fram_storage.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/

struct wspace
{
    struct nepa	*				client;
    struct nevent *				deferred_storage[EPA_MAIN_QUEUE_SIZE];
    struct nequeue				deferred;
    const struct nevent *		locked_event;
    struct acq_consumer *		acq_consumer;
};

enum local_epa_main_events
{
	TIMEOUT_GENERIC 			= NEVENT_LOCAL_ID,
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static naction state_init        		(struct nsm *, const struct nevent *);
static naction state_config      		(struct nsm *, const struct nevent *);
static naction state_master_init_eeprom (struct nsm *, const struct nevent *);
static naction state_master_init_fram	(struct nsm *, const struct nevent *);
static naction state_init_calibration	(struct nsm *, const struct nevent *);
static naction state_init_adt7410		(struct nsm *, const struct nevent *);
static naction state_setup_adt7410      (struct nsm *, const struct nevent *);
static naction state_init_aux	 		(struct nsm *, const struct nevent *);
static naction state_setup_aux	 		(struct nsm *, const struct nevent *);
static naction state_init_acq    		(struct nsm *, const struct nevent *);
static naction state_setup_acq   		(struct nsm *, const struct nevent *);
static naction state_normal 			(struct nsm *, const struct nevent *);
static naction state_idle   			(struct nsm *, const struct nevent *);
static naction state_broadcast_sample	(struct nsm *, const struct nevent *);
static naction state_trigger			(struct nsm *, const struct nevent *);
static naction state_trigger_stop_acq	(struct nsm *, const struct nevent *);
static naction state_trigger_set_config (struct nsm *, const struct nevent *);
static naction state_update_config		(struct nsm *, const struct nevent *);
static naction state_update_config_try	(struct nsm *, const struct nevent *);
static naction state_update_config_stop_acq	(struct nsm *, const struct nevent *);
static naction state_update_config_apply(struct nsm *, const struct nevent *);
static naction state_trigger_running	(struct nsm *, const struct nevent *);
static naction state_trigger_running_fetch	(struct nsm *, const struct nevent *);
static naction state_trigger_running_save	(struct nsm *, const struct nevent *);
static naction state_trigger_running_save_data	(struct nsm *, const struct nevent *);
static naction state_trigger_running_save_counter	(struct nsm *, const struct nevent *);
static naction state_reset_acq			(struct nsm *, const struct nevent *);
static naction state_reset_acq_stop		(struct nsm *, const struct nevent *);
static naction state_reset_acq_defaults (struct nsm *, const struct nevent *);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct nevent *          g_event_queue_storage[EPA_MAIN_QUEUE_SIZE];
static struct wspace       		g_wspace;

/*======================================================  GLOBAL VARIABLES  ==*/

struct nepa                     g_epa_main;

const struct nepa_define        g_epa_main_define = NEPA_DEF_INIT(
		&g_wspace,
		state_init,
		NSM_TYPE_HSM,
		g_event_queue_storage,
		sizeof(g_event_queue_storage),
		"main",
		EPA_MAIN_PRIO);

uint32_t 						g_trigg_samples;

uint32_t						g_trigg_last_state = MAIN_TRIGG_STOP;

uint32_t						g_trigg_current_no;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/


static void deferred_sample_notify(const struct acq_sample * sample)
{
	struct event_main_sample * 	new_sample;

	new_sample = NEVENT_CREATE_I(struct event_main_sample, EVENT_MAIN_SAMPLE);

	if (new_sample) {
		new_sample->data = sample[1];
		nepa_send_event_i(&g_epa_main, &new_sample->super);
	}
}

/******************************************************************************
 * State machine
 ******************************************************************************/


static naction state_init (struct nsm * sm, const struct nevent * event)
{
    struct wspace *        ws = nsm_wspace(sm);

    switch (event->id) {
        case NSM_INIT: {
        	struct nequeue_define deferred_config;

			deferred_config.storage = ws->deferred_storage;
			deferred_config.size = sizeof(ws->deferred_storage);

            nequeue_init(&ws->deferred, &deferred_config);

            ws->acq_consumer = acq_consumer_add(deferred_sample_notify, 1);

            return (naction_transit_to(sm, state_config));
        }
        default: {
            return (naction_super(sm, ntop_state));
        }
    }
}



static naction state_config(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
		case NSM_ENTRY: {

			nepa_send_signal(&g_epa_i2c, SIG_I2C_INIT);

			return (naction_handled());
		}
		case SIG_I2C_READY: {

			return (naction_transit_to(sm, state_master_init_eeprom));

		}
		case SIG_I2C_NOT_READY: {

			return (naction_transit_to(sm, state_init_aux));
		}
		default: {
			return (naction_super(sm, ntop_state));
		}
	}
}



static naction
state_master_init_eeprom(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
		case NSM_ENTRY: {
			nepa_send_signal(&g_epa_eeprom, SIG_EEPROM_INIT);

			return (naction_handled());
		}
		case SIG_EEPROM_READY: {

			return (naction_transit_to(sm, state_master_init_fram));
		}
		case SIG_EEPROM_NOT_READY: {

			return (naction_transit_to(sm, state_master_init_fram));
		}
		default: {
			return (naction_super(sm, ntop_state));
		}
	}
}



static naction
state_master_init_fram(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
        
		case NSM_ENTRY: {
			nepa_send_signal(&g_epa_fram, SIG_EEPROM_INIT);

			return (naction_handled());
		}
		case SIG_EEPROM_READY: {

			return (naction_transit_to(sm, state_init_calibration));
		}
		case SIG_EEPROM_NOT_READY: {

			return (naction_transit_to(sm, state_init_calibration));
		}
		default: {
			return (naction_super(sm, ntop_state));
		}
	}
}



static naction
state_init_calibration(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
		case NSM_ENTRY: {
			nepa_send_signal(&g_epa_calibration, SIG_CALIBRATION_INIT);

			return (naction_handled());
		}
		case SIG_CALIBRATION_READY: {

			return (naction_transit_to(sm, state_init_adt7410));
		}
		case SIG_CALIBRATION_NOT_READY: {

			return (naction_transit_to(sm, state_init_adt7410));
		}
		default: {
			return (naction_super(sm, ntop_state));
		}
	}
}



static naction state_init_adt7410(struct nsm * sm, const struct nevent * event)
{
	(void)sm;

	switch (event->id) {
		case NSM_ENTRY:{
			nepa_send_signal(&g_epa_adt7410, SIG_ADT7410_INIT);
			return (naction_handled());
		}
		case SIG_ADT7410_READY:{
			return (naction_transit_to(sm, state_setup_adt7410));
		}
		case SIG_ADT7410_NOT_READY: {

			return (naction_transit_to(sm, state_init_aux));
		}
		default: {
			return (naction_super(sm, ntop_state));
		}
	}
}



static naction state_setup_adt7410(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
		case NSM_ENTRY: {
			struct event_adt7410_set_config * config;

			config = NEVENT_CREATE(struct event_adt7410_set_config, EVENT_ADT7410_SET_CONFIG);

			if (config) {
				config->period_ms    = 1000u;

				nepa_send_event(&g_epa_adt7410, &config->super);
			}

			return (naction_handled());
		}
		case SIG_ADT7410_READY: {
			nepa_send_signal(&g_epa_adt7410, SIG_ADT7410_START);

			return (naction_transit_to(sm, state_init_aux));
		}
		case SIG_ADT7410_NOT_READY: {

			return (naction_transit_to(sm, state_init_aux));
		}
		default: {

			return (naction_super(sm, ntop_state));
		}
	}
}



static naction state_init_aux(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
		case NSM_ENTRY: {
			nepa_send_signal(&g_epa_aux, SIG_AUX_INIT);

			return (naction_handled());
		}
		case SIG_AUX_READY: {

			return (naction_transit_to(sm, state_setup_aux));
		}
		case SIG_AUX_NOT_READY: {

			return (naction_transit_to(sm, state_init_acq));
		}
		default: {
			return (naction_super(sm, ntop_state));
		}
	}
}



static naction state_setup_aux(struct nsm * sm, const struct nevent * event)
{
	(void)sm;

	switch (event->id) {
		case NSM_ENTRY: {
			g_aux_pending_config = g_aux_default_config;

			nepa_send_signal(&g_epa_aux, SIG_AUX_SET_CONFIG);

			return (naction_handled());
		}
		case SIG_AUX_READY: {
			nepa_send_signal(&g_epa_aux, SIG_AUX_START);

			return (naction_transit_to(sm, state_init_acq));
		}
		case SIG_AUX_NOT_READY: {

			return (naction_transit_to(sm, state_init_acq));
		}
		default: {

			return (naction_super(sm, ntop_state));
		}
	}
}



static naction state_init_acq(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
		case NSM_ENTRY: {
			nepa_send_signal(&g_epa_acq, SIG_ACQ_INIT);

			return (naction_handled());
		}
		case SIG_ACQ_READY: {

			return (naction_transit_to(sm, state_setup_acq));
		}
		case SIG_ACQ_NOT_READY: {

			return (naction_transit_to(sm, state_setup_acq));
		}
		default: {
			return (naction_super(sm, ntop_state));
		}
	}
}



static naction state_setup_acq(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
		case NSM_ENTRY: {
			g_acq_pending_config = g_acq_default_config;

			nepa_send_signal(&g_epa_acq, SIG_ACQ_SET_CONFIG);

			return (naction_handled());
		}
		case SIG_ACQ_READY: {
			nepa_send_signal(&g_epa_acq, SIG_ACQ_START);

			return (naction_transit_to(sm, state_normal));
		}
		case SIG_ACQ_NOT_READY: {

			return (naction_transit_to(sm, state_normal));
		}
		default: {
			return (naction_super(sm, ntop_state));
		}
	}
}



static naction state_normal(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			nepa_defer_fetch_all(&ws->deferred);

			return (naction_handled());
		}
		case NSM_INIT: {

			return (naction_transit_to(sm, state_idle));
		}
		case SIG_MAIN_UPDATE_CONFIG: {
			ws->client = event->producer;

			switch (g_acq_pending_config.trigger_mode) {
				case TRIG_MODE_IN_CONTINUOUS:
				case TRIG_MODE_IN_SINGLE_SHOT:
					return (naction_transit_to(sm, state_trigger));
				default:
					break;
			}
			return (naction_transit_to(sm, state_update_config));
		}
		default: {

			return (naction_super(sm, ntop_state));
		}
	}
}



static naction state_idle(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case SIG_MAIN_START_SAMPLING: {
			ws->client = event->producer;

			return (naction_transit_to(sm, state_broadcast_sample));
		}
		default: {

			return (naction_super(sm, state_normal));
		}
	}
}



static naction state_broadcast_sample(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			/**TODO:
			 * Take into account current sampling frequency and set the right
			 * n-th sample number.
			 */
			acq_consumer_at_every(ws->acq_consumer, 1);
			acq_consumer_enable(ws->acq_consumer);

			return (naction_handled());
		}
		case NSM_EXIT: {
			acq_consumer_disable(ws->acq_consumer);

			return (naction_handled());
		}
		case EVENT_MAIN_SAMPLE: {
			nepa_send_event(ws->client, event);

			return (naction_handled());
		}
		case SIG_MAIN_STOP_SAMPLING: {

			return (naction_transit_to(sm, state_idle));
		}
		default: {

			return (naction_super(sm, state_normal));
		}
	}
}



static naction state_update_config(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
		case NSM_INIT: {

			return (naction_transit_to(sm, state_update_config_try));
		}
		default: {

			return (naction_super(sm, ntop_state));
		}
	}
}



/*
 * Level 2 state :: update_config :: .
 */
static naction state_update_config_try(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			nepa_send_signal(&g_epa_acq, SIG_ACQ_UPDATE_CONFIG);

			return (naction_handled());
		}
		case SIG_ACQ_READY: {
			nepa_send_signal(ws->client, SIG_MAIN_OK);

			return (naction_transit_to(sm, state_normal));
		}
		case SIG_ACQ_NOT_READY: {
			nepa_send_signal(ws->client, SIG_MAIN_NOT_OK);

			return (naction_transit_to(sm, state_reset_acq));
		}
		case SIG_ACQ_BUSY: {

			return (naction_transit_to(sm, state_update_config_stop_acq));
		}
		default: {

			return (naction_super(sm, state_update_config));
		}
	}
}



/*
 * Level 2 state :: update_config :: .
 */
static naction state_update_config_stop_acq(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			nepa_send_signal(&g_epa_acq, SIG_ACQ_STOP);

			return (naction_handled());
		}
		case SIG_ACQ_READY: {

			return (naction_transit_to(sm, state_update_config_apply));
		}
		case SIG_ACQ_NOT_READY: {
			nepa_send_signal(ws->client, SIG_MAIN_NOT_OK);

			return (naction_transit_to(sm, state_reset_acq));
		}
		default: {

			return (naction_super(sm, state_update_config));
		}
	}
}



/*
 * Level 2 state :: update_config :: .
 */
static naction state_update_config_apply(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			nepa_send_signal(&g_epa_acq, SIG_ACQ_SET_CONFIG);

			return (naction_handled());
		}
		case SIG_ACQ_READY: {
			nepa_send_signal(&g_epa_acq, SIG_ACQ_START);
			nepa_send_signal(ws->client, SIG_MAIN_OK);

			return (naction_transit_to(sm, state_normal));
		}
		case SIG_ACQ_NOT_READY: {
			nepa_send_signal(ws->client, SIG_MAIN_NOT_OK);

			return (naction_transit_to(sm, state_reset_acq));
		}
		default: {

			return (naction_super(sm, state_update_config));
		}
	}
}



static naction state_trigger(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_INIT: {
			return (naction_transit_to(sm, state_trigger_stop_acq));
		}
		case SIG_MAIN_UPDATE_CONFIG: {
			ws->client = event->producer;

			return (naction_transit_to(sm, state_update_config));
		}
		default: {

			return (naction_super(sm, ntop_state));
		}
	}
}



/*
 * Level 2 state :: trigger :: .
 */
static naction state_trigger_stop_acq(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			nepa_send_signal(&g_epa_acq, SIG_ACQ_STOP);

			return (naction_handled());
		}
		case SIG_ACQ_READY: {

			return (naction_transit_to(sm, state_trigger_set_config));
		}
		case SIG_ACQ_NOT_READY: {
			nepa_send_signal(ws->client, SIG_MAIN_NOT_OK);

			return (naction_transit_to(sm, state_reset_acq));
		}
		default: {

			return (naction_super(sm , state_trigger));
		}
	}
}



/*
 * Level 2 state :: trigger :: .
 */
static naction state_trigger_set_config(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			nepa_send_signal(&g_epa_acq, SIG_ACQ_SET_CONFIG);

			return (naction_handled());
		}
		case SIG_ACQ_READY: {
			nepa_send_signal(&g_epa_acq, SIG_ACQ_START);
			nepa_send_signal(ws->client, SIG_MAIN_OK);

			return (naction_transit_to(sm, state_trigger_running));
		}
		case SIG_ACQ_NOT_READY: {
			nepa_send_signal(ws->client, SIG_MAIN_NOT_OK);

			return (naction_transit_to(sm, state_reset_acq));
		}
		default: {

			return (naction_super(sm, state_trigger));
		}
	}
}



/*
 * Level 2 state :: trigger :: .
 */
static naction state_trigger_running(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			g_trigg_last_state = MAIN_TRIGG_RUNNING;
			g_trigg_current_no = 0;
			/**TODO:
			 * Take into account current sampling frequency and set the right
			 * n-th sample number.
			 */
			acq_consumer_at_every(ws->acq_consumer, 1);
			acq_consumer_enable(ws->acq_consumer);

			return (naction_handled());
		}
		case NSM_EXIT: {
			acq_consumer_disable(ws->acq_consumer);

			return (naction_handled());
		}
		case NSM_INIT: {

			 return (naction_transit_to(sm, state_trigger_running_fetch));
		}
		default: {

			return (naction_super(sm, state_trigger));
		}
	}
}



/*
 * Level 3 state :: trigger :: running :: .
 */
static naction state_trigger_running_fetch(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			nepa_defer_fetch_one(&ws->deferred);

			return (naction_handled());
		}
		case EVENT_MAIN_SAMPLE: {
			ws->locked_event = event;
			nevent_lock(event);

			return (naction_transit_to(sm, state_trigger_running_save));
		}
		default: {
			return (naction_super(sm, state_trigger_running));
		}
	}
}



/* NOTE:
 * Sledece funkcije state_trigger_* rade direktno sa FRAM prostorom, odnosno,
 * ne postoji sloj koji menadzuje snimanje i citanje podataka sa FRAM-a. Ako se
 * ispostavi da ovo resenje ne zadovoljava zahteve i mora nesto slozenije da se
 * pravi onda ce morati da se doda novi EPA koji ce da menadzira pristup.
 */
/*
 * Level 3 state :: trigger :: running :: .
 */
static naction state_trigger_running_save(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_INIT: {

			return (naction_transit_to(sm, state_trigger_running_save_data));
		}
		case EVENT_MAIN_SAMPLE: {
			nepa_defer_event(&ws->deferred, event);

			return (naction_handled());
		}
		default: {
			return (naction_super(sm, state_trigger_running));
		}
	}
}



/*
 * Level 4 state :: trigger :: running :: save :: .
 */
static naction state_trigger_running_save_data(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			struct event_eeprom_transfer * eeprom_transfer;
			struct event_main_sample * main_sample =
					(struct event_main_sample *)ws->locked_event;

			eeprom_transfer = NEVENT_CREATE(struct event_eeprom_transfer, EVENT_EEPROM_WRITE);

			if (eeprom_transfer) {
				eeprom_transfer->address =
						FRAM_DATA_BASE + (g_trigg_current_no * sizeof(struct acq_sample));
				eeprom_transfer->buffer  = (void *)&main_sample->data;
				eeprom_transfer->size    = sizeof(struct acq_sample);

				nepa_send_event(&g_epa_fram, &eeprom_transfer->super);
			}

			return (naction_handled());
		}
		case NSM_EXIT: {
			nevent_unlock(ws->locked_event);

			return (naction_handled());
		}
		case SIG_EEPROM_INVAL: {
			g_trigg_last_state = MAIN_TRIGG_BUFFER_FULL;

			return (naction_transit_to(sm, state_normal));
		}
		case SIG_EEPROM_READY: {

			return (naction_transit_to(sm, state_trigger_running_save_counter));
		}
		case SIG_EEPROM_NOT_READY: {
			g_trigg_last_state = MAIN_TRIGG_WR_FAILED;
			nepa_send_signal(ws->client, SIG_MAIN_NOT_OK);

			return (naction_transit_to(sm, state_normal));
		}
		default: {
			return (naction_super(sm, state_trigger_running_save));
		}
	}
}



/*
 * Level 4 state :: trigger :: running :: save :: .
 */
static naction state_trigger_running_save_counter(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			struct event_eeprom_transfer * eeprom_transfer;

			eeprom_transfer = NEVENT_CREATE(struct event_eeprom_transfer, EVENT_EEPROM_WRITE);

			if (eeprom_transfer) {
				eeprom_transfer->address = FRAM_HEADER_BASE;
				eeprom_transfer->buffer  = &g_trigg_current_no;
				eeprom_transfer->size    = sizeof(g_trigg_current_no);

				nepa_send_event(&g_epa_fram, &eeprom_transfer->super);
			}

			return (naction_handled());
		}
		case NSM_EXIT: {

			return (naction_handled());
		}
		case SIG_EEPROM_INVAL: {
			g_trigg_last_state = MAIN_TRIGG_WR_FAILED;

			return (naction_transit_to(sm, state_normal));
		}
		case SIG_EEPROM_READY: {
			g_trigg_current_no++;

			if (g_trigg_samples == UINT32_MAX) {
				/*
				 * Infinite loop
				 */

				return (naction_transit_to(sm, state_trigger_running_fetch));
			} else if (g_trigg_current_no == g_trigg_samples) {
				g_trigg_last_state = MAIN_TRIGG_STOP;

				return (naction_transit_to(sm, state_normal));
			} else {

				return (naction_transit_to(sm, state_trigger_running_fetch));
			}
		}
		case SIG_EEPROM_NOT_READY: {
			g_trigg_last_state = MAIN_TRIGG_WR_FAILED;
			nepa_send_signal(ws->client, SIG_MAIN_NOT_OK);

			return (naction_transit_to(sm, state_normal));
		}
		default: {

			return (naction_super(sm, state_trigger_running_save));
		}
	}
}



static naction state_reset_acq(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
		case NSM_INIT: {

			return (naction_transit_to(sm, state_reset_acq_stop));
		}
		default: {

			return (naction_super(sm, ntop_state));
		}
	}
}



/*
 * Level 2 state :: reset_acq :: .
 */
static naction state_reset_acq_stop(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
		case NSM_ENTRY: {
			nepa_send_signal(&g_epa_acq, SIG_ACQ_STOP);

			return (naction_handled());
		}
		case SIG_ACQ_NOT_READY:
		case SIG_ACQ_READY: {

			return (naction_transit_to(sm, state_reset_acq_defaults));
		}
		default: {

			return (naction_super(sm, state_reset_acq));
		}
	}
}



/*
 * Level 2 state :: reset_acq :: .
 */
static naction state_reset_acq_defaults(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
		case NSM_ENTRY: {
			g_acq_pending_config = g_acq_default_config;

			nepa_send_signal(&g_epa_acq, SIG_ACQ_SET_CONFIG);

			return (naction_handled());
		}
		case SIG_ACQ_NOT_READY: {

			return (naction_transit_to(sm, state_normal));
		}
		case SIG_ACQ_READY: {
			nepa_send_signal(&g_epa_acq, SIG_ACQ_START);

			return (naction_transit_to(sm, state_normal));
		}
		default: {

			return (naction_super(sm, state_reset_acq));
		}
	}
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of epa_main.c
 ******************************************************************************/


