/*
 * epa_adt7410.c
 *
 *  Created on: Mar 3, 2016
 *      Author: nenad
 */


/*=========================================================  INCLUDE FILES  ==*/

#include <epa_i2c_master.h>
#include "epa_eeprom.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define EEPROM_I2C_ID					0x50
#define FRAM_I2C_ID						0x52

#define EEPROM_I2C_TIMEOUT_MS			100
#define EEPROM_I2C_WAIT_MS				50
#define EEPROM_I2C_ACK_WAIT_MS			2
#define EEPROM_I2C_RETRY_TIMES			5

/*======================================================  LOCAL DATA TYPES  ==*/

enum eeprom_local_events
{
	SIG_TIME_I2C_WRITE_ACK_WAIT			= NEVENT_LOCAL_ID,
	SIG_TIME_I2C_READ_ACK_WAIT,
	SIG_TIME_ERR_WRITE_DATA,
	SIG_TIME_ERR_READ_DATA_WR,
	SIG_TIME_ERR_READ_DATA_RD
};

struct wspace
{
    struct netimer              period;
    struct nequeue				deferred;
    struct nevent *				deferred_queue_storage[EPA_EEPROM_QUEUE_SIZE];
    struct nepa *				producer;
    struct event_i2c_master_transfer	transfer;
    uint8_t						i2c_retry;
    uint8_t						i2c_buffer[3];
    uint32_t					address;
    uint8_t *					raw_buffer;
    uint32_t					size;
    uint32_t					idx;
    uint32_t					max_size;
    uint8_t						dev_id;
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static naction state_init_eeprom    (struct nsm *, const struct nevent *);
static naction state_init_fram      (struct nsm *, const struct nevent *);
static naction state_idle    		(struct nsm *, const struct nevent *);
static naction state_initialized	(struct nsm *, const struct nevent *);
static naction state_write_data		(struct nsm *, const struct nevent *);
static naction state_read_data_wr	(struct nsm *, const struct nevent *);
static naction state_read_data_rd	(struct nsm *, const struct nevent *);

/*=======================================================  LOCAL VARIABLES  ==*/

/*-- EEPROM EPA --------------------------------------------------------------*/
static struct nevent *          g_eeprom_queue_storage[EPA_EEPROM_QUEUE_SIZE];
static struct wspace         	  g_eeprom_workspace;

/*-- FRAM EPA ----------------------------------------------------------------*/
static struct nevent *          g_fram_queue_storage[EPA_FRAM_QUEUE_SIZE];
static struct wspace         	g_fram_workspace;

/*======================================================  GLOBAL VARIABLES  ==*/

/*-- EEPROM EPA --------------------------------------------------------------*/
struct nepa                     g_epa_eeprom;
const struct nepa_define        g_epa_eeprom_define = NEPA_DEF_INIT(
		&g_eeprom_workspace,
		&state_init_eeprom,
		NSM_TYPE_FSM,
		g_eeprom_queue_storage,
		sizeof(g_eeprom_queue_storage),
		"EEPROM",
		EPA_EEPROM_PRIO);

/*-- FRAM EPA ----------------------------------------------------------------*/
struct nepa                     g_epa_fram;
const struct nepa_define        g_epa_fram_define = NEPA_DEF_INIT(
		&g_fram_workspace,
		&state_init_fram,
		NSM_TYPE_FSM,
		g_fram_queue_storage,
		sizeof(g_fram_queue_storage),
		"FRAM",
		EPA_FRAM_PRIO);

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

/* -------------------------------------------------------------------------- *
 * CTRL bus callback
 * -------------------------------------------------------------------------- */


static naction state_init_eeprom(struct nsm * sm, const struct nevent * event)
{
	struct wspace *				ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_INIT : {		
			struct nequeue_define deferred_queue;

			deferred_queue.storage = ws->deferred_queue_storage;
			deferred_queue.size = sizeof(ws->deferred_queue_storage);

	    	netimer_init(&ws->period);
			nequeue_init(&ws->deferred, &deferred_queue);
			ws->transfer.super			= g_default_event;
			ws->transfer.super.producer	= nepa_get_current();
			ws->transfer.dev_id 		= EEPROM_I2C_ID;
			ws->transfer.timeout_ms 	= EEPROM_I2C_TIMEOUT_MS;
			ws->max_size 		        = 8 * 1024;
			ws->dev_id			= EEPROM_I2C_ID;

			return (naction_transit_to(sm, state_idle));
		}
		default : {
			return (naction_ignored());
		}
	}
}



static naction state_init_fram(struct nsm * sm, const struct nevent * event)
{
	struct wspace *				ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_INIT : {
			struct nequeue_define deferred_queue;

			deferred_queue.storage = ws->deferred_queue_storage;
			deferred_queue.size = sizeof(ws->deferred_queue_storage);
			
			netimer_init(&ws->period);
			nequeue_init(&ws->deferred, &deferred_queue);
			ws->transfer.super			= g_default_event;
			ws->transfer.super.producer	= nepa_get_current();
			ws->transfer.dev_id 		= FRAM_I2C_ID;
			ws->transfer.timeout_ms 	= EEPROM_I2C_TIMEOUT_MS;
			ws->max_size 				= 128 * 1024;
			ws->dev_id					= FRAM_I2C_ID;

			return (naction_transit_to(sm, state_idle));
		}
		default : {
			return (naction_ignored());
		}
	}
}



static naction state_idle(struct nsm * sm, const struct nevent * event)
{
	struct wspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case SIG_EEPROM_INIT: {
			/*
			 * Start a write operation. This operation will write to address
			 * registers and not impact any data stored on device.
			 */
			ws->producer 			= event->producer;
			ws->transfer.super.id 	= EVENT_I2C_MASTER_WRITE;
			ws->transfer.buffer   	= &ws->i2c_buffer[0];
			ws->transfer.size     	= 2;
			ws->i2c_buffer[0] 		= 0;
			ws->i2c_buffer[1] 		= 0;

			nepa_send_event(&g_epa_i2c_master, &ws->transfer.super);

			return (naction_handled());
		}
		case EVENT_I2C_MASTER_COMPLETE: {
			const struct event_i2c_master_complete * complete = nevent_data(event);

			if (complete->error) {
			    /*
			     * TODO: set that EEPROM/FRAM is NOT active
			     */
				nepa_send_signal(ws->producer, SIG_EEPROM_NOT_READY);

				return (naction_handled());
			} else {
			    /*
                 * TODO: set that EEPROM/FRAM is active
                 */
				nepa_send_signal(ws->producer, SIG_EEPROM_READY);

				return (naction_transit_to(sm, state_initialized));
			}
		}
		case EVENT_EEPROM_READ:
		case EVENT_EEPROM_WRITE: {
            
			nepa_send_signal(event->producer, SIG_EEPROM_NOT_READY);

			return (naction_handled());
		}
		default : {
			return (naction_ignored());
		}
	}
}



static naction state_initialized(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 		ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			nepa_defer_fetch_all(&ws->deferred);

			ws->i2c_retry = 0;

			return (naction_handled());
		}
		case EVENT_EEPROM_WRITE: {
			const struct event_eeprom_transfer * transfer = nevent_data(event);

			if ( (transfer->address >= 8192) ||
			    ((transfer->size + transfer->address) > ws->max_size)) {
				nepa_send_signal(event->producer, SIG_EEPROM_INVAL);

				return (naction_handled());
			} else {
				ws->producer 	= event->producer;
				ws->address  	= transfer->address;
				ws->raw_buffer	= transfer->buffer;
				ws->size     	= transfer->size;
				ws->idx		 	= 0;

				return (naction_transit_to(sm, state_write_data));
			}
		}
		case EVENT_EEPROM_READ: {
			const struct event_eeprom_transfer * transfer = nevent_data(event);

			if ( (transfer->address >= ws->max_size) ||
				((transfer->size + transfer->address) > ws->max_size)) {
				nepa_send_signal(event->producer, SIG_EEPROM_INVAL);

				return (naction_handled());
			} else {
				ws->producer 	= event->producer;
				ws->address  	= transfer->address;
				ws->raw_buffer	= transfer->buffer;
				ws->size     	= transfer->size;
				ws->idx      	= 0;

				return (naction_transit_to(sm, state_read_data_wr));
			}
		}
		default : {
			return (naction_ignored());
		}
	}
}



static naction state_write_data(struct nsm * sm, const struct nevent * event)
{
	struct wspace *				ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			ws->transfer.super.id 	= EVENT_I2C_MASTER_WRITE;
			ws->transfer.buffer   	= &ws->i2c_buffer[0];
			ws->transfer.size     	= 3;
			ws->transfer.dev_id     = ws->dev_id |
					(uint8_t)(((ws->address + ws->idx) >> 16u) & 0xffu);
			ws->i2c_buffer[0] 		= ((ws->address + ws->idx) >> 8u) & 0xffu;
			ws->i2c_buffer[1]		= ((ws->address + ws->idx) >> 0u) & 0xffu;
			ws->i2c_buffer[2]		= ws->raw_buffer[ws->idx];

			nepa_send_event(&g_epa_i2c_master, &ws->transfer.super);

			return (naction_handled());
		}
		case NSM_EXIT: {
			netimer_cancel(&ws->period);

			return (naction_handled());
		}
		case EVENT_I2C_MASTER_COMPLETE: {
			const struct event_i2c_master_complete * complete = nevent_data(event);

			if (complete->error == NERROR_NO_DEVICE) {
				/* We got no ACK here. Previous write operation is still
				 * pending;
				 */
				netimer_after(&ws->period, NTIMER_MS(EEPROM_I2C_ACK_WAIT_MS),
						SIG_TIME_I2C_WRITE_ACK_WAIT);

				return (naction_handled());
			} else if (complete->error) {
				ws->i2c_retry++;

				if (ws->i2c_retry < EEPROM_I2C_RETRY_TIMES) {
					netimer_after(&ws->period, NTIMER_MS(EEPROM_I2C_WAIT_MS),
							SIG_TIME_ERR_WRITE_DATA);

					return (naction_handled());
				} else {
				    /*
				     * TODO: set that EEPROM/FRAM is NOT active
				     */
					nepa_send_signal(ws->producer, SIG_EEPROM_NOT_READY);

					return (naction_transit_to(sm, state_initialized));
				}
			} else {
				ws->idx++;

				if (ws->idx < ws->size) {
					return (naction_transit_to(sm, state_write_data));
				} else {
				    /*
				     * TODO: set that EEPROM/FRAM is active
				     */
					nepa_send_signal(ws->producer, SIG_EEPROM_READY);

					return (naction_transit_to(sm, state_initialized));
				}
			}
		}
		case SIG_TIME_ERR_WRITE_DATA:
		case SIG_TIME_I2C_WRITE_ACK_WAIT: {

			return (naction_transit_to(sm, state_write_data));
		}
		case EVENT_EEPROM_WRITE:
		case EVENT_EEPROM_READ: {
			nepa_defer_event(&ws->deferred, event);

			return (naction_handled());
		}
		default : {
			return (naction_ignored());
		}
	}
}



static naction state_read_data_wr(struct nsm * sm, const struct nevent * event)
{
	struct wspace *				ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			ws->transfer.super.id 	= EVENT_I2C_MASTER_WRITE;
			ws->transfer.buffer   	= &ws->i2c_buffer[0];
			ws->transfer.size     	= 2;
			ws->transfer.dev_id     = ws->dev_id |
					(uint8_t)(((ws->address + ws->idx) >> 16u) & 0xffu);
			ws->i2c_buffer[0] 		= ((ws->address + ws->idx) >> 8u) & 0xffu;
			ws->i2c_buffer[1]		= ((ws->address + ws->idx) >> 0u) & 0xffu;

			nepa_send_event(&g_epa_i2c_master, &ws->transfer.super);

			return (naction_handled());
		}
		case NSM_EXIT: {
			netimer_cancel(&ws->period);

			return (naction_handled());
		}
		case EVENT_I2C_MASTER_COMPLETE: {
			const struct event_i2c_master_complete * complete = nevent_data(event);

			if (complete->error == NERROR_NO_DEVICE) {
				/* We got no ACK here. Previous write operation is still
				 * pending;
				 */
				netimer_after(&ws->period, NTIMER_MS(EEPROM_I2C_ACK_WAIT_MS),
						SIG_TIME_I2C_READ_ACK_WAIT);

				return (naction_handled());
			} else if (complete->error) {
				ws->i2c_retry++;

				if (ws->i2c_retry < EEPROM_I2C_RETRY_TIMES) {
					netimer_after(&ws->period, NTIMER_MS(EEPROM_I2C_WAIT_MS),
							SIG_TIME_ERR_READ_DATA_WR);

					return (naction_handled());
				} else {
				    /*
				     * TODO: set that EEPROM/FRAM is NOT active
				     */
					nepa_send_signal(ws->producer, SIG_EEPROM_NOT_READY);

					return (naction_transit_to(sm, state_initialized));
				}
			} else {

				return (naction_transit_to(sm, state_read_data_rd));
			}
		}
		case SIG_TIME_ERR_READ_DATA_WR:
		case SIG_TIME_I2C_READ_ACK_WAIT: {

			return (naction_transit_to(sm, state_read_data_wr));
		}
		case EVENT_EEPROM_WRITE:
		case EVENT_EEPROM_READ: {
			nepa_defer_event(&ws->deferred, event);

			return (naction_handled());
		}
		default : {
			return (naction_ignored());
		}
	}
}



static naction state_read_data_rd(struct nsm * sm, const struct nevent * event)
{
	struct wspace *				ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			ws->transfer.super.id 	= EVENT_I2C_MASTER_READ;
			ws->transfer.buffer   	= &ws->raw_buffer[ws->idx];
			ws->transfer.size     	= 1;

			nepa_send_event(&g_epa_i2c_master, &ws->transfer.super);

			return (naction_handled());
		}
		case NSM_EXIT: {
			netimer_cancel(&ws->period);

			return (naction_handled());
		}
		case EVENT_I2C_MASTER_COMPLETE: {
			const struct event_i2c_master_complete * complete = nevent_data(event);

			if (complete->error) {
				ws->i2c_retry++;

				if (ws->i2c_retry < EEPROM_I2C_RETRY_TIMES) {
					netimer_after(&ws->period, NTIMER_MS(EEPROM_I2C_WAIT_MS),
							SIG_TIME_ERR_READ_DATA_RD);

					return (naction_handled());
				} else {
				    /*
				     * TODO: set that EEPROM/FRAM is NOT active
				     */
					nepa_send_signal(ws->producer, SIG_EEPROM_NOT_READY);

					return (naction_transit_to(sm, state_initialized));
				}
			} else {
				ws->idx++;

				if (ws->idx < ws->size) {
					return (naction_transit_to(sm, state_read_data_wr));
				} else {
				    /*
				     * TODO: set that EEPROM/FRAM is active
				     */
					nepa_send_signal(ws->producer, SIG_EEPROM_READY);

					return (naction_transit_to(sm, state_initialized));
				}
			}
		}
		case SIG_TIME_ERR_READ_DATA_RD: {

			return (naction_transit_to(sm, state_read_data_wr));
		}
		case EVENT_EEPROM_WRITE:
		case EVENT_EEPROM_READ: {
			nepa_defer_event(&ws->deferred, event);

				return (naction_handled());
		}
		default : {
			return (naction_ignored());
		}
	}
}


/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of epa_eeprom.c
 ******************************************************************************/
