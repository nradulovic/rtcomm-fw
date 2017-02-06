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

#include <epa_i2c_master.h>
#include <stddef.h>
#include <string.h>

#include "epa_main.h"
#include "epa_acq.h"
#include "epa_aux.h"
#include "epa_heartbeat.h"
#include "epa_calibration.h"
#include "epa_adt7410.h"
#include "epa_eeprom.h"
#include "cdi/io.h"
#include "app_stat.h"
#include "ppbuff.h"
#include "fram_storage.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/

struct wspace
{
    struct nepa	*				client;
};

enum local_epa_main_events
{
	TIMEOUT_GENERIC 			= NEVENT_LOCAL_ID,
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static naction state_init        		(struct nsm *, const struct nevent *);
static naction state_config      		(struct nsm *, const struct nevent *);
static naction state_init_aux	 		(struct nsm *, const struct nevent *);
static naction state_setup_aux	 		(struct nsm *, const struct nevent *);
static naction state_init_acq    		(struct nsm *, const struct nevent *);
static naction state_setup_acq   		(struct nsm *, const struct nevent *);
static naction state_normal 			(struct nsm *, const struct nevent *);
static naction state_update_config		(struct nsm *, const struct nevent *);
static naction state_update_config_try	(struct nsm *, const struct nevent *);
static naction state_update_config_stop_acq	(struct nsm *, const struct nevent *);
static naction state_update_config_apply(struct nsm *, const struct nevent *);
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

/******************************************************************************
 * State machine
 ******************************************************************************/


static naction state_init (struct nsm * sm, const struct nevent * event)
{
    switch (event->id) {
        case NSM_INIT: {

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

			return (naction_handled());
		}
		case NSM_INIT: {

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

			return (naction_handled());
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


