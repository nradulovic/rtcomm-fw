/*
 * epa_i2c.c
 *
 *  Created on: Mar 16, 2016
 *      Author: nenad
 */


/*=========================================================  INCLUDE FILES  ==*/

#include "epa_i2c_master.h"
#include "ctrl_bus.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define I2C_BUS_ERROR_WAIT				NTIMER_MS(20)
#define I2C_ARBITRATION_ERROR_WAIT		NTIMER_MS(40)
#define I2C_ACKNOWLEDGE_ERROR_WAIT		NTIMER_MS(2)

#define I2C_ERROR_ABORT					(0x1u << 8)

/*======================================================  LOCAL DATA TYPES  ==*/

struct wspace
{
    struct netimer              timeout;
    struct nequeue				defer;
    struct nepa *				producer;
    bool						is_emitting_data;
    uint8_t *					buffer;
    uint16_t					size;
    uint16_t					dev_id;
    uint32_t					timeout_ms;
    uint16_t					error;
	uint16_t					transferred;
};

struct event_transfer_complete
{
	struct nevent 				super;
	uint16_t					error;
	uint16_t					transferred;
};

enum i2c_local_events
{
	SIG_TIME_READ_OP 			= NEVENT_LOCAL_ID,
	SIG_TIME_READ_RETRY,
	SIG_TIME_WRITE_OP,
	SIG_TIME_WRITE_RETRY,
	EVENT_TRANSFER_ERROR,
	EVENT_TRANSFER_COMPLETE,
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static void i2c_error(uint16_t error, uint16_t transferred);
static void i2c_complete(uint16_t);

static naction state_init 				(struct nsm *, const struct nevent *);
static naction state_sleeping			(struct nsm *, const struct nevent *);
static naction state_idle				(struct nsm *, const struct nevent *);
static naction state_transfer 			(struct nsm *, const struct nevent *);

/*=======================================================  LOCAL VARIABLES  ==*/

/*-- Heart beat EPA ----------------------------------------------------------*/
static struct nevent *          g_i2c_queue_storage[EPA_I2C_QUEUE_SIZE];
static struct nevent *			g_i2c_deferred_queue_storage[EPA_I2C_QUEUE_SIZE];
static struct wspace         	g_i2c_workspace;
static const struct ctrl_bus_client g_ctrl_bus =
{
	i2c_error,
	i2c_complete
};

/*======================================================  GLOBAL VARIABLES  ==*/
/*-- Heart beat EPA ----------------------------------------------------------*/
struct nepa                     g_epa_i2c_master;
const struct nepa_define        g_epa_i2c_master_define = NEPA_DEF_INIT(
		&g_i2c_workspace,
		&state_init,
		NSM_TYPE_FSM,
		g_i2c_queue_storage,
		sizeof(g_i2c_queue_storage),
		"i2c",
		EPA_I2C_PRIO);

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/


static void i2c_error(uint16_t error, uint16_t transferred)
{
	static struct event_transfer_complete i2c_transferred = {
		NEVENT_INITIALIZER(EVENT_TRANSFER_ERROR, NULL, 0),
		0,
		0
	};
	i2c_transferred.error       = error;
	i2c_transferred.transferred = transferred;

	nepa_send_event_i(&g_epa_i2c_master, &i2c_transferred.super);
}


static void i2c_complete(uint16_t transferred)
{
	static struct event_transfer_complete i2c_transferred = {
		NEVENT_INITIALIZER(EVENT_TRANSFER_COMPLETE, NULL, 0),
		0,
		0
	};
	i2c_transferred.error 		= 0;
	i2c_transferred.transferred = transferred;

	nepa_send_event_i(&g_epa_i2c_master, &i2c_transferred.super);
}

/*-- FSM states --------------------------------------------------------------*/
static naction state_init(struct nsm * sm, const struct nevent * event)
{
    struct wspace *          ws = nsm_wspace(sm);

    switch (event->id) {
        case NSM_INIT: {
        	static const struct nequeue_define deferred_queue_def =
				NEQUEUE_DEF_INIT(g_i2c_deferred_queue_storage, sizeof(g_i2c_deferred_queue_storage));
            netimer_init(&ws->timeout);
            nequeue_init(&ws->defer, &deferred_queue_def);

            return (naction_transit_to(sm, state_sleeping));
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_sleeping(struct nsm * sm, const struct nevent * event)
{
	switch (event->id) {
		case SIG_I2C_MASTER_INIT: {
			ctrl_bus_init(&g_ctrl_bus);

			nepa_send_signal(event->producer, SIG_I2C_MASTER_READY);

			return (naction_transit_to(sm, state_idle));
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_idle(struct nsm * sm, const struct nevent * event)
{
	struct wspace *          ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			netimer_cancel(&ws->timeout);
			nepa_defer_fetch_all(&ws->defer);

			return (naction_handled());
		}
		case EVENT_I2C_MASTER_READ: {
			const struct event_i2c_master_transfer * transfer = nevent_data(event);

			ws->producer 	= event->producer;
			ws->buffer   	= transfer->buffer;
			ws->size     	= transfer->size;
			ws->dev_id	 	= (uint16_t)(transfer->dev_id * 2u);
			ws->timeout_ms	= transfer->timeout_ms;
			ws->error		= 0;
			ws->transferred = 0;
			ws->is_emitting_data = false;

			return (naction_transit_to(sm, state_transfer));
		}
		case EVENT_I2C_MASTER_WRITE: {
			const struct event_i2c_master_transfer * transfer = nevent_data(event);

			ws->producer 	= event->producer;
			ws->buffer   	= transfer->buffer;
			ws->size     	= transfer->size;
			ws->dev_id	 	= (uint16_t)(transfer->dev_id * 2u);
			ws->timeout_ms	= transfer->timeout_ms;
			ws->error		= 0;
			ws->transferred = 0;
			ws->is_emitting_data = true;

			return (naction_transit_to(sm, state_transfer));
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_transfer(struct nsm * sm, const struct nevent * event)
{
	struct wspace *          ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			if (ws->is_emitting_data) {
				ctrl_master_tx_start(ws->buffer, ws->size, ws->dev_id);
			} else {
				ctrl_master_rx_start(ws->buffer, ws->size, ws->dev_id);
			}

			return (naction_handled());
		}
		case NSM_EXIT: {
			struct event_i2c_master_complete * complete;

			complete = NEVENT_CREATE(struct event_i2c_master_complete, EVENT_I2C_MASTER_COMPLETE);

			if (complete) {

				if (ws->error == 0) {
					complete->error = NERROR_NONE;
				} else if (ws->error & CTRL_BUS_ERROR_BUS) {
					complete->error = NERROR_DEVICE_NO_COMM;
				} else if (ws->error & CTRL_BUS_ERROR_ACKNOWLEDGE) {
					complete->error = NERROR_NO_DEVICE;
				} else if (ws->error & CTRL_BUS_ERROR_ARBITRATION) {
					complete->error = NERROR_DEVICE_BUSY;
				} else if (ws->error & I2C_ERROR_ABORT) {
					complete->error = NERROR_OP_ABORT;
				} else {
					complete->error = NERROR_DEVICE_FAIL;
				}
				complete->transferred = ws->transferred;

				nepa_send_event(ws->producer, &complete->super);
			}

			return (naction_handled());
		}
		case EVENT_TRANSFER_ERROR: {
			const struct event_transfer_complete * local_complete = nevent_data(event);

			ctrl_bus_cancel();

			ws->error      |= local_complete->error;
			ws->transferred = local_complete->transferred;

			return (naction_transit_to(sm, state_idle));
		}
		case SIG_I2C_MASTER_CANCEL: {
			ctrl_bus_cancel();

			ws->error |= I2C_ERROR_ABORT;

			return (naction_handled());
		}
		case EVENT_TRANSFER_COMPLETE: {
			const struct event_transfer_complete * local_complete = nevent_data(event);

			ws->transferred = local_complete->transferred;

			return (naction_transit_to(sm, state_idle));
		}
		case EVENT_I2C_MASTER_READ:
		case EVENT_I2C_MASTER_WRITE: {
			nepa_defer_event(&ws->defer, event);

			return (naction_handled());
		}
		default: {

			return (naction_ignored());
		}
	}
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of epa_i2c.c
 ******************************************************************************/
