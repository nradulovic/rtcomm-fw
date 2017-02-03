/*
 * hartbeat.c
 *
 *  Created on: May 28, 2015
 *      Author: nenad
 */


/*
 * This file is part of Neon EDS Example.
 *
 * Scheduler ping pong FSM
 */

/*=========================================================  INCLUDE FILES  ==*/

#include "epa_heartbeat.h"
#include "hw_config.h"
#include "prim_gpio.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define PERIOD_PAUSE                    NTIMER_MS(1000)
#define PERIOD_SHORT                    NTIMER_MS(50)
#define PERIOD_DELAY                    NTIMER_MS(100)
#define PERIOD_LONG                     NTIMER_MS(120)

#define led_heartbeat_on()                                                      \
    gpio_set(LED_HARTBEAT_PORT, LED_HARTBEAT_PIN)

#define led_heartbeat_off()                                                     \
    gpio_clr(LED_HARTBEAT_PORT, LED_HARTBEAT_PIN)

/*======================================================  LOCAL DATA TYPES  ==*/

struct workspace
{
    struct netimer              period;
    struct netimer              wait;
};

enum hart_beat_local_events
{
    PERIOD_ELAPSED              = NEVENT_LOCAL_ID,
    PERIOD_WAIT,
    BLINK
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static naction state_heart_beat_init (struct nsm *, const struct nevent *);
static naction state_sync_beat		 (struct nsm *, const struct nevent *);
static naction state_heart_beat_pause(struct nsm *, const struct nevent *);
static naction state_heart_beat_short(struct nsm *, const struct nevent *);
static naction state_heart_beat_delay(struct nsm *, const struct nevent *);
static naction state_heart_beat_long (struct nsm *, const struct nevent *);

/*=======================================================  LOCAL VARIABLES  ==*/

/*-- Heart beat EPA ----------------------------------------------------------*/
static struct nevent *          g_heart_beat_queue_storage[EPA_HEARTBEAT_QUEUE_SIZE];
static struct workspace         g_heart_beat_workspace;

/*======================================================  GLOBAL VARIABLES  ==*/
/*-- Heart beat EPA ----------------------------------------------------------*/
struct nepa                     g_epa_heartbeat;
const struct nepa_define        g_epa_heartbeat_define = NEPA_DEF_INIT(
		&g_heart_beat_workspace,
		&state_heart_beat_init,
		NSM_TYPE_FSM,
		g_heart_beat_queue_storage,
		sizeof(g_heart_beat_queue_storage),
		"heart_beat",
		EPA_HEARTBEAT_PRIO);

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static void led_heartbeat_init(void)
{
    GPIO_InitTypeDef            gpio_init;

    /* LED HEARTBEAT */
    LED_HARTBEAT_CLK_ENABLE();
    gpio_init.Pin   = LED_HARTBEAT_PIN;
    gpio_init.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull  = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_Init(LED_HARTBEAT_PORT, &gpio_init);
    gpio_clr(LED_HARTBEAT_PORT, LED_HARTBEAT_PIN);
}

/******************************************************************************
 * State machine
 ******************************************************************************/

static naction state_heart_beat_init(struct nsm * sm, const struct nevent * event)
{
    struct workspace *          ws = nsm_wspace(sm);

    switch (event->id) {
        case NSM_INIT: {
            led_heartbeat_init();
            netimer_init(&ws->period);
            netimer_init(&ws->wait);

            return (naction_transit_to(sm, state_sync_beat));
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_sync_beat(struct nsm * sm, const struct nevent * event)
{
	struct workspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			netimer_every(&ws->period, PERIOD_SHORT, PERIOD_ELAPSED);

			return (naction_handled());
		}
		case NSM_EXIT: {
			netimer_every(&ws->period, PERIOD_PAUSE, PERIOD_ELAPSED);

			return (naction_handled());
		}
		case PERIOD_ELAPSED: {
			static uint32_t		once_tick;

			if (++once_tick & 0x1u) {
				led_heartbeat_on();
			} else {
				led_heartbeat_off();
			}

			return (naction_handled());
		}
		case SIG_HEARTBEAT_INIT: {
			return (naction_transit_to(sm, state_heart_beat_pause));
		}
		default: {
			return (naction_ignored());
		}
	}
}



static naction state_heart_beat_pause(struct nsm * sm, const struct nevent * event)
{
    switch (event->id) {
        case PERIOD_ELAPSED: {

            return (naction_transit_to(sm, state_heart_beat_short));
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_heart_beat_short(struct nsm * sm, const struct nevent * event)
{
    struct workspace *          ws = nsm_wspace(sm);

    switch (event->id) {
        case NSM_ENTRY: {
            netimer_after(&ws->wait, PERIOD_SHORT, BLINK);
            led_heartbeat_on();

            return (naction_handled());
        }
        case NSM_EXIT: {
            led_heartbeat_off();

            return (naction_handled());
        }
        case BLINK: {
            return (naction_transit_to(sm, state_heart_beat_delay));
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_heart_beat_delay(struct nsm * sm, const struct nevent * event)
{
    struct workspace *          ws = nsm_wspace(sm);

    switch (event->id) {
        case NSM_ENTRY: {
            netimer_after(&ws->wait, PERIOD_DELAY, PERIOD_WAIT);

            return (naction_handled());
        }
        case PERIOD_WAIT: {
            return (naction_transit_to(sm, state_heart_beat_long));
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_heart_beat_long(struct nsm * sm, const struct nevent * event)
{
    struct workspace *          ws = nsm_wspace(sm);

    switch (event->id) {
        case NSM_ENTRY: {
            netimer_after(&ws->wait, PERIOD_LONG, BLINK);
            led_heartbeat_on();

            return (naction_handled());
        }
        case NSM_EXIT: {
            led_heartbeat_off();

            return (naction_handled());
        }
        case BLINK: {
            return (naction_transit_to(sm, state_heart_beat_pause));
        }
        default: {
            return (naction_ignored());
        }
    }
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of epa_heart_beat.c
 ******************************************************************************/
