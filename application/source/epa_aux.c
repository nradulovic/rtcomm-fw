/*
 * epa_aux.c
 *
 *  Created on: May 26, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Event Processing Agent for AUX ADC
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include <string.h>

#include "epa_main.h"
#include "epa_aux.h"
#include "aux_channels.h"
#include "app_stat.h"
#include "ads1256.h"
#include "cdi/io.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define WRITE_WAIT_PERIOD               NTIMER_MS(1)

#define mux_positive(mux)               ((mux) >> 4u)

#define mux_negative(mux)               ((mux) & 0xfu)

/*======================================================  LOCAL DATA TYPES  ==*/

struct wspace
{
    struct netimer              timeout;
    struct nepa *				producer;
	uint32_t					idx;
};

enum epa_aux_signals
{
    SIG_DRDY_SIGNAL             = NEVENT_LOCAL_ID,
    SIG_TIMEOUT_START,
    SIG_TIMEOUT_SETUP,
    SIG_TIMEOUT_WAKEUP,
    SIG_TIMEOUT_MUX,
    SIG_TIMEOUT_SYNC,
    SIG_TIMEOUT_RDATA,
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static naction state_init    		(struct nsm *, const struct nevent *);
static naction state_initialized    (struct nsm *, const struct nevent *);
static naction state_setup			(struct nsm *, const struct nevent *);
static naction state_idle    		(struct nsm *, const struct nevent *);
static naction state_wait_drdy   	(struct nsm *, const struct nevent *);
static naction state_set_mux 		(struct nsm *, const struct nevent *);
static naction state_sync    		(struct nsm *, const struct nevent *);
static naction state_wakeup  		(struct nsm *, const struct nevent *);
static naction state_rdata   		(struct nsm *, const struct nevent *);
static naction state_wait_drdy		(struct nsm *, const struct nevent *);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct wspace    		g_aux_wspace;
static struct nevent *          g_aux_working_queue_storage[EPA_AUX_QUEUE_SIZE];

/*======================================================  GLOBAL VARIABLES  ==*/

struct nepa                     g_epa_aux;

const struct nepa_define        g_epa_aux_define = NEPA_DEF_INIT(
		&g_aux_wspace,
		state_init,
		NSM_TYPE_FSM,
		g_aux_working_queue_storage,
		sizeof(g_aux_working_queue_storage),
		"aux",
		EPA_AUX_PRIO);

struct aux_config				g_aux_current_config;

struct aux_config				g_aux_pending_config;

const struct aux_config			g_aux_default_config =
{
#if defined(PORT_C_GCC)
	.mux_channels  				= 2,
	.mux_configs[0]				= ADS_MUX(2, 3),
	.mux_configs[1]				= ADS_MUX(0, 1)
#elif defined(PORT_C_ARMCC)
	/* .mux_channels */
	2,
	/* .mux_configs[8] */
	{
		ADS_MUX(2, 3),					/* Channel 0 */
		ADS_MUX(0, 1),					/* Channel 1 */
		0,   							/* Channel 2 */
		0,   							/* Channel 3 */
		0,   							/* Channel 4 */
		0,   							/* Channel 5 */
		0,   							/* Channel 6 */
		0,   							/* Channel 7 */
	}
#else
#error "Unsupported compiler in default configuration initialization"
#endif
};

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/


static void drdy_signal(void)
{
	static const struct nevent drdy_signal =
			NEVENT_INITIALIZER(SIG_DRDY_SIGNAL, NULL, 0);

    nepa_send_event_i(&g_epa_aux, &drdy_signal);
    aux_signal_disable();
}



static void
set_activity_aux(bool is_active)
{
    (void)is_active;
    /*
     * TODO: set if AUX is active here
     */
}

/******************************************************************************
 * State machine
 ******************************************************************************/



static naction state_init(struct nsm * sm, const struct nevent * event)
{
    struct wspace *     		ws = nsm_wspace(sm);

    switch (event->id) {
        case NSM_INIT: {
			netimer_init(&ws->timeout);
            aux_channel_init();
            aux_set_signal(drdy_signal);

        	return (naction_transit_to(sm, state_initialized));
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_initialized(struct nsm * sm, const struct nevent * event)
{
	struct wspace * 			ws = nsm_wspace(sm);

	switch (event->id) {
		case SIG_AUX_INIT: {
			ws->producer = event->producer;

			return (naction_transit_to(sm, state_setup));
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_setup(struct nsm * sm, const struct nevent * event)
{
	struct wspace *     ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			aux_reset();

			netimer_after(&ws->timeout, NTIMER_MS(10), SIG_TIMEOUT_SETUP);

			return (naction_handled());
		}

		case SIG_TIMEOUT_SETUP: {
			uint32_t            retry;
			uint32_t            error;

			retry = 10;
			error = 1;

			while (retry-- && error) {
				uint8_t			data = ADS_DRATE_10SPS;
				uint8_t 		check_data;

				check_data = aux_write_then_read_reg(ADS_REG_DRATE, data);

				if (check_data == data) {
					error = 0;
				}
			}

			if (error) {
				nepa_send_signal(ws->producer, SIG_AUX_NOT_READY);

				return (naction_transit_to(sm, state_initialized));
			} else {
				nepa_send_signal(ws->producer, SIG_AUX_READY);

				return (naction_transit_to(sm, state_idle));
			}
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_idle(struct nsm * sm, const struct nevent * event)
{
    struct wspace *     		ws = nsm_wspace(sm);

    switch (event->id) {
        case NSM_ENTRY: {
        	aux_signal_disable();

            return (naction_handled());
        }
        case SIG_AUX_SET_CONFIG: {
        	memcpy(&g_aux_current_config, &g_aux_pending_config, sizeof(g_aux_current_config));
        	ws->idx  		 = 0;

        	nepa_send_signal(event->producer, SIG_AUX_READY);

        	return (naction_handled());
        }
        case SIG_AUX_START: {
        	netimer_after(&ws->timeout, NTIMER_MS(20), SIG_TIMEOUT_START);

        	return (naction_handled());
        }
        case SIG_TIMEOUT_START: {

        	return (naction_transit_to(sm, state_wait_drdy));
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_set_mux(struct nsm * sm, const struct nevent * event)
{
    struct wspace *     ws = nsm_wspace(sm);

    switch (event->id) {
        case NSM_INIT: {
            aux_write_reg(ADS_REG_MUX, g_aux_current_config.mux_configs[ws->idx]);

			netimer_after(&ws->timeout, WRITE_WAIT_PERIOD, SIG_TIMEOUT_MUX);

            return (naction_handled());
        }
        case NSM_EXIT: {
            netimer_cancel(&ws->timeout);

            return (naction_handled());
        }
        case SIG_TIMEOUT_MUX: {

            return (naction_transit_to(sm, state_sync));
        }
        case SIG_AUX_STOP: {
        	set_activity_aux(false);

            return (naction_transit_to(sm, state_idle));
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_sync(struct nsm * sm, const struct nevent * event)
{
    struct wspace *     ws = nsm_wspace(sm);

    switch (event->id) {
        case NSM_ENTRY: {
            aux_write_cmd(ADS_CMD_SYNC);
            netimer_after(&ws->timeout, WRITE_WAIT_PERIOD, SIG_TIMEOUT_SYNC);

            return (naction_handled());
        }
        case NSM_EXIT: {
            netimer_cancel(&ws->timeout);

            return (naction_handled());
        }
        case SIG_TIMEOUT_SYNC: {

            return (naction_transit_to(sm, state_wakeup));
        }
        case SIG_AUX_STOP: {
        	set_activity_aux(false);

            return (naction_transit_to(sm, state_idle));
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_wakeup(struct nsm * sm, const struct nevent * event)
{
    struct wspace *     ws = nsm_wspace(sm);

    switch (event->id) {
        case NSM_ENTRY: {
            aux_write_cmd(ADS_CMD_WAKEUP);
            netimer_after(&ws->timeout, WRITE_WAIT_PERIOD, SIG_TIMEOUT_WAKEUP);

            return (naction_handled());
        }
        case SIG_TIMEOUT_WAKEUP: {

			return (naction_transit_to(sm, state_rdata));
        }
        case SIG_AUX_STOP: {
        	set_activity_aux(false);

            return (naction_transit_to(sm, state_idle));
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_rdata(struct nsm * sm, const struct nevent * event)
{
    struct wspace *     ws = nsm_wspace(sm);

    switch (event->id) {
        case NSM_INIT: {
            int32_t             data;
            

            data = io_raw_adc_to_int(aux_read_data());

#if defined(T_MATH_SIMULATION)
            data = 2239345;
#endif
            /*
             * TODO:
             * do something with data
             */

			if (ws->idx == g_aux_current_config.mux_channels) {
				ws->idx = 0;
			}
			set_activity_aux(true);

            return (naction_transit_to(sm, state_wait_drdy));
        }
        case SIG_AUX_STOP: {
        	set_activity_aux(false);

            return (naction_transit_to(sm, state_idle));
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_wait_drdy(struct nsm * sm, const struct nevent * event)
{
    switch (event->id) {
        case NSM_ENTRY: {
            aux_signal_enable();

            return (naction_handled());
        }
        case SIG_DRDY_SIGNAL: {

            return (naction_transit_to(sm, state_set_mux));
        }
        case SIG_AUX_STOP: {
        	set_activity_aux(false);

            return (naction_transit_to(sm, state_idle));
        }
        default: {

            return (naction_ignored());
        }
    }
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of epa_aux.c
 ******************************************************************************/
