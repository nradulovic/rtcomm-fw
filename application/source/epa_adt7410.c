/*
 * epa_adt7410.c
 *
 *  Created on: Mar 3, 2016
 *      Author: nenad
 */


/*=========================================================  INCLUDE FILES  ==*/

#include <epa_i2c_master.h>
#include "epa_adt7410.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define ADT7410_REG_TEMP_MSB            0x00
#define ADT7410_REG_TEMP_LSB            0x01
#define ADT7410_REG_STATUS              0x02
#define ADT7410_REG_CONFIGURATION       0x03
#define ADT7410_REG_ID                  0x0b
#define ADT7410_REG_RESET               0x2f

#define REG_STATUS_NRDY                 (0x1u << 7u)
#define REG_CONFIGURATION_RESOLUTION    (0x1u << 7u)

#define ADT7410_I2C_ID                  0x48

#define ADT7410_ID                      0xc8

#define ADT7410_I2C_TIMEOUT_MS  		100
#define ADT7410_I2C_WAIT_MS				50
#define ADT7410_I2C_RETRY_TIMES			5

#define ADT7410_STATUS_NRDY_MS			100
#define ADT7410_RETRY_TIMES				5


/*======================================================  LOCAL DATA TYPES  ==*/

enum adt7410_local_events
{
	SIG_TIME_ERR_RESET			= NEVENT_LOCAL_ID,
	SIG_TIME_ERR_WRITE_REG_CONF,
	SIG_TIME_ERR_I2C,
	SIG_TIME_READ_STATUS_NRDY,
	SIG_WAIT,
	SIG_ACQUISITION_PERIOD
};

struct workspace
{
    struct netimer              period;
    struct netimer              wait;
    struct nepa *				producer;
    struct event_i2c_master_transfer 	transfer;
    uint32_t					i2c_retry;
    uint8_t						i2c_buffer[2];
    uint32_t					period_ms;
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static naction state_init    			(struct nsm *, const struct nevent *);
static naction state_idle    			(struct nsm *, const struct nevent *);
static naction state_reset				(struct nsm *, const struct nevent *);
static naction state_write_reg_conf		(struct nsm *, const struct nevent *);
static naction state_initialized		(struct nsm *, const struct nevent *);
static naction state_i2c_error			(struct nsm *, const struct nevent *);
static naction state_read_status_wr		(struct nsm *, const struct nevent *);
static naction state_read_status_rd		(struct nsm *, const struct nevent *);
static naction state_read_temp_wr		(struct nsm *, const struct nevent *);
static naction state_read_temp_rd		(struct nsm *, const struct nevent *);

/*=======================================================  LOCAL VARIABLES  ==*/

/*-- ADT7410 EPA -------------------------------------------------------------*/
static struct nevent *          g_adt7410_queue_storage[EPA_ADT7410_QUEUE_SIZE];
static struct workspace         g_adt7410_workspace;

/*======================================================  GLOBAL VARIABLES  ==*/

/*-- ADT7410 EPA -------------------------------------------------------------*/
struct nepa                     g_epa_adt7410;
const struct nepa_define        g_epa_adt7410_define = NEPA_DEF_INIT(
		&g_adt7410_workspace,
		&state_init,
		NSM_TYPE_FSM,
		g_adt7410_queue_storage,
		sizeof(g_adt7410_queue_storage),
		"ADT7410",
		EPA_ADT7410_PRIO);

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static void
set_activity_adt7410(bool is_active)
{
    (void)is_active;
    /*
     * TODO: Set if adt7410 is detected here
     */
}

/******************************************************************************
 * State machine
 ******************************************************************************/



static naction state_init(struct nsm * sm, const struct nevent * event)
{
	struct workspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_INIT : {
			netimer_init(&ws->period);
			netimer_init(&ws->wait);
			ws->transfer.super			= g_default_event;
			ws->transfer.super.producer	= nepa_get_current();
			ws->transfer.dev_id 		= ADT7410_I2C_ID;
			ws->transfer.timeout_ms 	= ADT7410_I2C_TIMEOUT_MS;

			return (naction_transit_to(sm, state_idle));
		}
		default : {
			return (naction_ignored());
		}
	}
}



static naction state_idle(struct nsm * sm, const struct nevent * event)
{
	struct workspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case SIG_ADT7410_INIT: {
			ws->producer = event->producer;
			ws->i2c_retry = 0;

			return (naction_transit_to(sm, state_reset));
		}
		default : {
			return (naction_ignored());
		}
	}
}



static naction state_reset(struct nsm * sm, const struct nevent * event)
{
	struct workspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			ws->transfer.super.id 	= EVENT_I2C_MASTER_WRITE;
			ws->transfer.buffer   	= &ws->i2c_buffer[0];
			ws->transfer.size     	= 1;
			ws->i2c_buffer[0]    	= ADT7410_REG_RESET;

			nepa_send_event(&g_epa_i2c_master, &ws->transfer.super);

			return (naction_handled());
		}
		case EVENT_I2C_MASTER_COMPLETE: {
			const struct event_i2c_master_complete * complete = nevent_data(event);

			if (complete->error) {
				ws->i2c_retry++;

				if (ws->i2c_retry < ADT7410_I2C_RETRY_TIMES) {
					netimer_after(&ws->period, NTIMER_MS(ADT7410_STATUS_NRDY_MS),
							SIG_TIME_ERR_RESET);

					return (naction_handled());
				} else {
					nepa_send_signal(ws->producer, SIG_ADT7410_NOT_READY);

					return (naction_transit_to(sm, state_idle));
				}
			} else {
				netimer_after(&ws->wait, NTIMER_MS(10), SIG_WAIT);

				return (naction_handled());
			}
		}
		case SIG_TIME_ERR_RESET: {

			return (naction_transit_to(sm, state_reset));
		}
		case SIG_WAIT: {
			ws->i2c_retry = 0;

			return (naction_transit_to(sm, state_write_reg_conf));
		}
		default : {
			return (naction_ignored());
		}
	}
}



static naction state_write_reg_conf(struct nsm * sm, const struct nevent * event)
{
	struct workspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			ws->transfer.super.id 	= EVENT_I2C_MASTER_WRITE;
			ws->transfer.buffer   	= &ws->i2c_buffer[0];
			ws->transfer.size     	= 2;
			ws->i2c_buffer[0] 		= ADT7410_REG_CONFIGURATION;
			ws->i2c_buffer[1] 		= REG_CONFIGURATION_RESOLUTION;

			nepa_send_event(&g_epa_i2c_master, &ws->transfer.super);

			return (naction_handled());
		}
		case EVENT_I2C_MASTER_COMPLETE: {
			const struct event_i2c_master_complete * complete = nevent_data(event);

			if (complete->error) {
				ws->i2c_retry++;

				if (ws->i2c_retry < ADT7410_I2C_RETRY_TIMES) {
					netimer_after(&ws->period, NTIMER_MS(ADT7410_I2C_WAIT_MS),
							SIG_TIME_ERR_WRITE_REG_CONF);

					return (naction_handled());
				} else {
					nepa_send_signal(ws->producer, SIG_ADT7410_NOT_READY);

					return (naction_transit_to(sm, state_idle));
				}
			} else {
				nepa_send_signal(ws->producer, SIG_ADT7410_READY);

				return (naction_transit_to(sm, state_initialized));
			}
		}
		case SIG_TIME_ERR_WRITE_REG_CONF: {

			return (naction_transit_to(sm, state_write_reg_conf));
		}
		default : {

			return (naction_ignored());
		}
	}
}



static naction state_initialized(struct nsm * sm, const struct nevent * event)
{
	struct workspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			ws->i2c_retry = 0;

			return (naction_handled());
		}
		case EVENT_ADT7410_SET_CONFIG: {
			const struct event_adt7410_set_config * config = nevent_data(event);

			ws->period_ms    = config->period_ms;

			nepa_send_signal(event->producer, SIG_ADT7410_READY);

			return (naction_handled());
		}
		case SIG_ADT7410_START: {
			netimer_every(&ws->period, NTIMER_MS(ws->period_ms),
					SIG_ACQUISITION_PERIOD);

			return (naction_handled());
		}
		case SIG_ADT7410_STOP: {
			netimer_cancel(&ws->period);

			set_activity_adt7410(false);

			return (naction_handled());
		}
		case SIG_ACQUISITION_PERIOD: {

			return (naction_transit_to(sm, state_read_status_wr));
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_i2c_error(struct nsm * sm, const struct nevent * event)
{
	struct workspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			netimer_after(&ws->period, NTIMER_MS(ADT7410_I2C_WAIT_MS),
					SIG_TIME_ERR_I2C);

			return (naction_handled());
		}
		case SIG_TIME_ERR_I2C: {
			ws->i2c_retry++;

			if (ws->i2c_retry < ADT7410_I2C_RETRY_TIMES) {
				return (naction_transit_to(sm, state_read_status_wr));
			} else {
				set_activity_adt7410(false);

				return (naction_transit_to(sm, state_initialized));
			}
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_read_status_wr(struct nsm * sm, const struct nevent * event)
{
	struct workspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			ws->transfer.super.id 	= EVENT_I2C_MASTER_WRITE;
			ws->transfer.buffer   	= &ws->i2c_buffer[0];
			ws->transfer.size     	= 1;
			ws->i2c_buffer[0] 		= ADT7410_REG_STATUS;

			nepa_send_event(&g_epa_i2c_master, &ws->transfer.super);

			return (naction_handled());
		}
		case EVENT_I2C_MASTER_COMPLETE: {
			const struct event_i2c_master_complete * complete = nevent_data(event);

			if (complete->error) {

				return (naction_transit_to(sm, state_i2c_error));
			} else {

				return (naction_transit_to(sm, state_read_status_rd));
			}
		}
		case SIG_TIME_ERR_I2C: {

			return (naction_transit_to(sm, state_read_status_wr));
		}
		case SIG_ADT7410_STOP: {
			netimer_cancel(&ws->period);
			set_activity_adt7410(false);

			return (naction_transit_to(sm, state_initialized));
		}
		default : {

			return (naction_ignored());
		}
	}
}



static naction state_read_status_rd(struct nsm * sm, const struct nevent * event)
{
	struct workspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			ws->transfer.super.id 	= EVENT_I2C_MASTER_READ;
			ws->transfer.buffer   	= &ws->i2c_buffer[0];
			ws->transfer.size     	= 1;

			nepa_send_event(&g_epa_i2c_master, &ws->transfer.super);

			return (naction_handled());
		}
		case EVENT_I2C_MASTER_COMPLETE: {
			const struct event_i2c_master_complete * complete = nevent_data(event);

			if (complete->error) {

				return (naction_transit_to(sm, state_i2c_error));
			} else {

				if (ws->i2c_buffer[0] & REG_STATUS_NRDY) {
					netimer_after(&ws->period, NTIMER_MS(ADT7410_STATUS_NRDY_MS),
							SIG_TIME_READ_STATUS_NRDY);

					return (naction_handled());
				} else {

					return (naction_transit_to(sm, state_read_temp_wr));
				}
			}
		}
		case SIG_TIME_READ_STATUS_NRDY: {

			return (naction_transit_to(sm, state_read_status_rd));
		}
		case SIG_ADT7410_STOP: {
			netimer_cancel(&ws->period);
			set_activity_adt7410(false);

			return (naction_transit_to(sm, state_initialized));
		}
		default : {

			return (naction_ignored());
		}
	}
}



static naction state_read_temp_wr(struct nsm * sm, const struct nevent * event)
{
	struct workspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			ws->transfer.super.id 	= EVENT_I2C_MASTER_WRITE;
			ws->transfer.buffer   	= &ws->i2c_buffer[0];
			ws->transfer.size     	= 1;
			ws->i2c_buffer[0] 		= ADT7410_REG_TEMP_MSB;

			nepa_send_event(&g_epa_i2c_master, &ws->transfer.super);

			return (naction_handled());
		}
		case EVENT_I2C_MASTER_COMPLETE: {
			const struct event_i2c_master_complete * complete = nevent_data(event);

			if (complete->error) {

				return (naction_transit_to(sm, state_i2c_error));
			} else {

				return (naction_transit_to(sm, state_read_temp_rd));
			}
		}
		case SIG_ADT7410_STOP: {
			netimer_cancel(&ws->period);
			set_activity_adt7410(false);

			return (naction_transit_to(sm, state_initialized));
		}
		default : {

			return (naction_ignored());
		}
	}
}



static naction state_read_temp_rd(struct nsm * sm, const struct nevent * event)
{
	struct workspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			ws->transfer.super.id 	= EVENT_I2C_MASTER_READ;
			ws->transfer.buffer   	= &ws->i2c_buffer[0];
			ws->transfer.size     	= 2;

			nepa_send_event(&g_epa_i2c_master, &ws->transfer.super);

			return (naction_handled());
		}
		case EVENT_I2C_MASTER_COMPLETE: {
			const struct event_i2c_master_complete * complete = nevent_data(event);

			if (complete->error) {

				return (naction_transit_to(sm, state_i2c_error));
			} else {
				int16_t			raw_data;

				raw_data = (int16_t)(((uint16_t)ws->i2c_buffer[0] << 8u) |
						             ((uint16_t)ws->i2c_buffer[1] << 0u));

#if defined(T_MATH_SIMULATION)
				raw_data = 4016;
#endif
				/*
				 * TODO: do something with raw_data
				 * TODO: set that adt7410 is active
				 */
				set_activity_adt7410(true);

				return (naction_transit_to(sm, state_initialized));
			}
		}
		case SIG_ADT7410_STOP: {
			netimer_cancel(&ws->period);
			set_activity_adt7410(false);

			return (naction_transit_to(sm, state_initialized));
		}
		default : {

			return (naction_ignored());
		}
	}
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of epa_heart_beat.c
 ******************************************************************************/
