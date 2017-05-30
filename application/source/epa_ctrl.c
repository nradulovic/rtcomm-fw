/*
 * epa_ctrl.c
 *
 *  Created on: May 30, 2017
 *      Author: nenad
 */

/*=========================================================  INCLUDE FILES  ==*/

#include "epa_ctrl.h"
#include "neon_eds.h"
#include "cdi/io.h"
#include "status.h"
#include "ctrl.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/

struct ctrl_wspace
{
	struct netimer				timeout;
	union ctrl_var_buffer {
		struct io_ctrl_config   	config;
		struct io_ctrl_param		param;
	}							buffer;
};

enum ctrl_local_events
{
	SIG_ERR_BUSY = NEVENT_LOCAL_ID,
	SIG_RETRY,
	SIG_XFER_COMPLETE
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static naction state_init(struct nsm *, const nevent *);
static naction state_wait_config(struct nsm *, const nevent *);
static naction state_wait_param(struct nsm *, const nevent *);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct ctrl_wspace     	g_ctrl_wspace;
static struct nevent *          g_ctrl_event_queue[EPA_CTRL_QUEUE_SIZE];

/*======================================================  GLOBAL VARIABLES  ==*/

struct nepa_define              g_ctrl_epa_define = NEPA_DEF_INIT(
        &g_ctrl_wspace, state_init, NSM_TYPE_FSM, g_ctrl_event_queue,
        sizeof(g_ctrl_event_queue), "ctrl", EPA_CTRL_PRIO);

struct nepa                     g_ctrl_epa;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static naction state_init(struct nsm * sm, const nevent * event)
{
    struct ctrl_wspace *      	ws = nsm_wspace(sm);

    switch (nevent_id(event)) {
        case NSM_INIT: {
        	netimer_init(&ws->timeout);

        	return (naction_transit_to(sm, state_wait_config));
        }
        default: {
            return (naction_handled());
        }
    }
}

static naction state_wait_config(struct nsm * sm, const nevent * event)
{
	struct ctrl_wspace *      	ws = nsm_wspace(sm);

	switch (nevent_id(event)) {
		case NSM_ENTRY: {
			ctrl_start_rx(&g_ctrl, &ws->buffer.config, sizeof(ws->buffer.config));

			return (naction_handled());
		}
		case NSM_EXIT: {
			netimer_cancel(&ws->timeout);

			return (naction_handled());
		}
		case SIG_XFER_COMPLETE: {

			return (naction_transit_to(sm, state_wait_param));
		}
		case SIG_ERR_BUSY: {
			netimer_after(&ws->timeout, NTIMER_MS(10), SIG_RETRY);

			return (naction_handled());
		}
		case SIG_RETRY: {
			return (naction_transit_to(sm, state_wait_config));
		}
		default: {
			return (naction_handled());
		}
	}
}

static naction state_wait_param(struct nsm * sm, const nevent * event)
{
	struct ctrl_wspace *      	ws = nsm_wspace(sm);

	switch (nevent_id(event)) {
		case NSM_ENTRY: {
			ctrl_start_rx(&g_ctrl, &ws->buffer.param, sizeof(ws->buffer.param));

			return (naction_handled());
		}
		case NSM_EXIT: {
			netimer_cancel(&ws->timeout);

			return (naction_handled());
		}
		case SIG_XFER_COMPLETE: {

			return (naction_transit_to(sm, state_wait_param));
		}
		case SIG_ERR_BUSY: {
			netimer_after(&ws->timeout, NTIMER_MS(10), SIG_RETRY);

			return (naction_handled());
		}
		case SIG_RETRY: {
			return (naction_transit_to(sm, state_wait_config));
		}
		default: {
			return (naction_handled());
		}
	}
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void ctrl_rx_complete(struct ctrl * ctrl, void * buffer, uint16_t size)
{
	static const struct nevent sig_xfer_complete = NEVENT_INITIALIZER(
			SIG_XFER_COMPLETE, NULL, 0);
	nerror						error;

	(void)ctrl;
	(void)buffer;
	(void)size;

	error = nepa_send_event_i(&g_ctrl_epa, &sig_xfer_complete);

	if (error) {
		status_warn(STATUS_RESOURCE_FAILED);
	}
}

void ctrl_tx_complete(struct ctrl * ctrl, void * buffer, uint16_t size)
{
	(void)ctrl;
	(void)buffer;
	(void)size;
}

void ctrl_error(struct ctrl * ctrl, uint32_t error)
{
	static const struct nevent sig_err_busy = NEVENT_INITIALIZER(SIG_ERR_BUSY,
			NULL, 0);

	(void)ctrl;
	(void)error;

	if (error == 0) {
		nerror					neon_error;

		neon_error = nepa_send_event_i(&g_ctrl_epa, &sig_err_busy);

		if (neon_error) {
			status_warn(STATUS_RESOURCE_FAILED);
		}
	}
	status_warn(STATUS_CTRL_FAILED);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of epa_ctrl.c
 ******************************************************************************/
