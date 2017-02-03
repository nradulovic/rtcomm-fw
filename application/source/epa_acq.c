/*
 * epa_main.c
 *
 *  Created on: May 25, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Event Processing Agent for Acquisition ADC implementation
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include <data_process.h>
#include <stddef.h>
#include <string.h>

#include "cdi/io.h"
#include "epa_acq.h"
#include "app_stat.h"
#include "acq_channels.h"
#include "acq_sync.h"
#include "aux_channels.h"
#include "ms_bus.h"
#include "ctrl_bus.h"
#include "notify.h"
#include "ads1256.h"
#include "ppbuff.h"
#include "hw_config.h"
#include "data_process.h"
#include "sched/deferred.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/

enum epa_acq_local_events
{
	SIG_TIMEOUT_WRITE			= NEVENT_LOCAL_ID,
	SIG_TIMEOUT_RESET,
	SIG_TIMEOUT_START,
	SIG_TIMEOUT_RDC,
	SIG_TIMEOUT_ISR,
	SIG_TIMEOUT_ISR_WAIT,
	SIG_TIMEOUT_IO,
	SIG_TIMEOUT_GAIN_WAIT,
};

struct acq_wspace
{
	struct netimer 				timeout;
	struct nepa *				producer;
	uint32_t					detected_adc_mask;
	uint32_t					idx;
	uint32_t					gain;
};

#define ACQ_CONSUMERS_MAX_COUNT	3
struct acq_consumers
{
	uint32_t					count; 											/* Number of consumers */
	uint32_t					activated;
	struct acq_sample			samples[2];										/* Hold raw and final sample */
	struct acq_consumer
	{
		bool					is_enabled;
		uint32_t				sample_count;									/* Current sample number */
		uint32_t				at_every;
		void 				 (* fn)(const struct acq_sample *);
		struct nsched_deferred  deferred_work;
	}							consumer[ACQ_CONSUMERS_MAX_COUNT];
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static naction state_init       			(struct nsm *, const struct nevent *);
static naction state_reset_adcs				(struct nsm *, const struct nevent *);
static naction state_dormant				(struct nsm *, const struct nevent *);
static naction state_idle       			(struct nsm *, const struct nevent *);
static naction state_write_regs 			(struct nsm *, const struct nevent *);
static naction state_rdc_start_setup_rdc	(struct nsm *, const struct nevent *);
static naction state_rdc_start_setup_isr	(struct nsm *, const struct nevent *);
static naction state_rdc_running      		(struct nsm *, const struct nevent *);
static naction state_rdc_stop_destroy_isr 	(struct nsm *, const struct nevent *);
static naction state_rdc_stop_destroy_rdc 	(struct nsm *, const struct nevent *);
static naction state_rdc_gain_destroy_isr	(struct nsm *, const struct nevent *);
static naction state_rdc_gain_destroy_rdc	(struct nsm *, const struct nevent *);
static naction state_rdc_gain_set_io		(struct nsm *, const struct nevent *);
static naction state_rdc_gain_wait_period	(struct nsm *, const struct nevent *);
static naction state_rd_start				(struct nsm *, const struct nevent *);
static naction state_rd_running				(struct nsm *, const struct nevent *);
static naction state_rd_stop				(struct nsm *, const struct nevent *);

static bool acq_apply_config(struct acq_wspace * ws, struct acq_config * old, const struct acq_config * new);
static void acq_apply_gain(uint32_t gain);
static bool trigger_rdc_single_shot(void);
static bool trigger_rdc_continious(void);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct nevent *          g_event_queue_storage[EPA_ACQ_QUEUE_SIZE];
static struct acq_wspace 		g_acq_wspace;
static struct ppbuff            g_pp_buff;
static struct acq_consumers 	g_consumers;

/*======================================================  GLOBAL VARIABLES  ==*/

struct nepa                     g_epa_acq;

const struct nepa_define        g_epa_acq_define = NEPA_DEF_INIT(
		&g_acq_wspace,
		state_init,
		NSM_TYPE_FSM,
		g_event_queue_storage,
		sizeof(g_event_queue_storage),
		"acq",
		EPA_ACQ_PRIO);

struct acq_config 				g_acq_pending_config;

struct acq_config				g_acq_current_config;

const struct acq_config			g_acq_default_config =
{
#if defined(PORT_C_GCC)
	.adc_regs[ADS_REG_MUX]   = ADS_MUX(4, 5),
	.adc_regs[ADS_REG_ADCON] = 0,
	.adc_regs[ADS_REG_DRATE] = ADS_DRATE_1000SPS,
	.adc_regs[ADS_REG_IO]    = 0xfu,
	.write_flags             = ACQ_SET_ADC_MUX   | ACQ_SET_ADC_ADCON |
	   	   	   	   	   	   	   ACQ_SET_ADC_DRATE | ACQ_SET_ADC_IO,
	.enabled_adc_mask		 = ACQ_CHANNEL_X_MASK | ACQ_CHANNEL_Y_MASK |
							   ACQ_CHANNEL_Z_MASK,
	.buff_size				 = MAX_RING_BUFF_SIZE,
	.trigger_mode			 = TRIG_MODE_OUT,
	.acq_mode                = ACQ_MODE_CONTINUOUS,
	.data_process_flags		 = DATA_PROCESS_ENABLE_MATH |
							   DATA_PROCESS_MATH_FLAG_TE |
							   DATA_PROCESS_MATH_FLAG_TCO
#elif defined(PORT_C_ARMCC)
	/* .adc_regs[11] */
	{
		0,								/* 0  - STATUS */
		ADS_MUX(2 ,3),					/* 1  - MUX*/
		0,								/* 2  - ADCON */
		ADS_DRATE_1000SPS,      		/* 3  - DRATE */
		0x0fu,							/* 4  - IO */
		0,								/* 5  - OFC0 */
		0,								/* 6  - OFC1 */
		0,								/* 7  - OFC2 */
		0,								/* 8  - FSC0 */
		0,								/* 9  - FSC1 */
		0,								/* 10 - FSC2 */
	},
	/* .write_flags */
	ACQ_SET_ADC_MUX | ACQ_SET_ADC_ADCON | ACQ_SET_ADC_DRATE | ACQ_SET_ADC_IO,
	/* .enabled_adc_mask */
	ACQ_CHANNEL_X_MASK | ACQ_CHANNEL_Y_MASK | ACQ_CHANNEL_Z_MASK,
	/* .buff_size */
	MAX_RING_BUFF_SIZE,
	/* .trigger_mode */
	TRIG_MODE_OUT,
	/* .acq_mode */
	ACQ_MODE_CONTINUOUS,
	/* range_hi[4] */
	{
		0,
		0,
		0,
		0,
	},
	/* range_lo[4] */
	{
		0,
		0,
		0,
		0,
	},
	/* .attack_time */
	0,
	/* .pullback_time */
	0,
	/* .data_process_flags */
	DATA_PROCESS_ENABLE_MATH | DATA_PROCESS_MATH_FLAG_TE |
	DATA_PROCESS_MATH_FLAG_TCO
#else
#error "Unsupported compiler in default configuration initialization"
#endif
};

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

/* -------------------------------------------------------------------------- *
 * MS bus callback
 * -------------------------------------------------------------------------- */


static void ms_bus_tx_complete(void)
{
    ppbuff_unlock_consumer(&g_pp_buff);
    notify_disable();
}



static void ms_bus_tx_error(void)
{
    ppbuff_unlock_consumer(&g_pp_buff);
}

/* -------------------------------------------------------------------------- *
 * ACQ bus callback
 * -------------------------------------------------------------------------- */


void acq_transfer_finished_0(struct spi_transfer * transfer)
{
    struct acq_sample * 		sample;
    uint32_t 					value;

    value = __REV(*(uint32_t *)&transfer->buff[0]) >> 8u;
    sample = ppbuff_current_sample(&g_pp_buff);
#if defined(T_MATH_SIMULATION)
    sample_set_int(samples, -1263, ACQ_CHANNEL_X);
#else
    sample_set_int(sample, io_raw_adc_to_int(value), ACQ_CHANNEL_X);
#endif
    ppbuff_push_sample(&g_pp_buff, ACQ_CHANNEL_X_MASK);
}



void acq_transfer_finished_1(struct spi_transfer * transfer)
{
    struct acq_sample * 		sample;
    uint32_t					value;

    value = __REV(*(uint32_t *)&transfer->buff[0]) >> 8u;
    sample = ppbuff_current_sample(&g_pp_buff);
#if defined(T_MATH_SIMULATION)
    sample_set_int(samples, -6539, ACQ_CHANNEL_Y);
#else
    sample_set_int(sample, io_raw_adc_to_int(value), ACQ_CHANNEL_Y);
#endif
    ppbuff_push_sample(&g_pp_buff, ACQ_CHANNEL_Y_MASK);
}



void acq_transfer_finished_2(struct spi_transfer * transfer)
{
    struct acq_sample * 		sample;
    uint32_t					value;

    value = __REV(*(uint32_t *)&transfer->buff[0]) >> 8u;
	sample = ppbuff_current_sample(&g_pp_buff);
#if defined(T_MATH_SIMULATION)
	sample_set_int(samples, -11018, ACQ_CHANNEL_Z);
#else
    sample_set_int(sample, io_raw_adc_to_int(value), ACQ_CHANNEL_Z);
#endif
    ppbuff_push_sample(&g_pp_buff, ACQ_CHANNEL_Z_MASK);
}

/* -------------------------------------------------------------------------- *
 * ADC Gain handling
 * -------------------------------------------------------------------------- */

static
void gain_handler(uint32_t new_gain)
{
    static struct event_acq_set_gain evt_gain =
    {
    	NEVENT_INITIALIZER(EVENT_ACQ_SET_GAIN, NULL, 0),
		0
    };
	evt_gain.gain = new_gain;

	nepa_send_event(&g_epa_acq, &evt_gain.super);
}


/* -------------------------------------------------------------------------- *
 * Command handling
 * -------------------------------------------------------------------------- */


static bool acq_update_config(struct acq_wspace * ws, struct acq_config * old, const struct acq_config * new)
{
	uint32_t					idx;

	(void)ws;

	for (idx = 0; idx < sizeof(old->adc_regs); idx++) {
		if (old->adc_regs[idx] != new->adc_regs[idx]) {
			return (false);
		}
	}

	if (old->write_flags != new->write_flags) {
		return (false);
	}

	if (old->buff_size != new->buff_size) {
		return (false);
	}

	if (old->trigger_mode != new->trigger_mode) {
		return (false);
	}

	if (old->acq_mode != new->acq_mode) {
		return (false);
	}

	for (idx = 0; idx < DATA_PROCESS_GAIN_LEVELS; idx++) {
		if (old->range_hi[idx] != new->range_hi[idx]) {
			return (false);
		}

		if (old->range_lo[idx] != new->range_lo[idx]) {
			return (false);
		}
	}

	if (old->attack_time != new->attack_time) {
		return (false);
	}

	if (old->pullback_time != new->pullback_time) {
		return (false);
	}
	data_process_set_flags(&g_global_process, new->data_process_flags);

	memcpy(old, new, sizeof(*old));

	return (true);
}



static bool acq_apply_config(struct acq_wspace * ws, struct acq_config * old, const struct acq_config * new)
{
	uint32_t                    idx;

	if (ws->detected_adc_mask != new->enabled_adc_mask) {
		return (false);
	}
    
	if (new->enabled_adc_mask & ACQ_CHANNEL_X_MASK) {
		acq_x_channel_enable(ACQ_CHANNEL_X);
	}

	if (new->enabled_adc_mask & ACQ_CHANNEL_Y_MASK) {
		acq_x_channel_enable(ACQ_CHANNEL_Y);
	}

	if (new->enabled_adc_mask & ACQ_CHANNEL_Z_MASK) {
		acq_x_channel_enable(ACQ_CHANNEL_Z);
	}

	for (idx = 0; idx < DATA_PROCESS_GAIN_LEVELS; idx++) {
		data_process_set_range_hi_limit(&g_global_process, new->range_hi[idx], idx);
		data_process_set_range_lo_limit(&g_global_process, new->range_lo[idx], idx);
	}
	data_process_set_range_attack_time(&g_global_process, new->attack_time);
	data_process_set_range_pullback_time(&g_global_process, new->pullback_time);
	data_process_set_flags(&g_global_process, new->data_process_flags);

	ppbuff_init(&g_pp_buff, new->buff_size, new->enabled_adc_mask);

	acq_x_trigger_mode(new->trigger_mode);
	acq_x_mode(new->acq_mode);

	memcpy(old, new, sizeof(*old));

    return (true);
}



static void acq_apply_gain(uint32_t gain)
{
    uint32_t                    idx;
    uint8_t						data;

    /* NOTE:
     * ADS_IO_DIR0 is CLOCKOUT function => must be zero.
     * ADS_IO_DIR3 and ADS_IO_DIR2 are gain pins => must be zero.
     */
    switch (gain) {
        case 0: {
            data = 0u;
            break;
        }
        case 1: {
            data = 2u;
            break;
        }
        case 2: {
            data = 4u;
            break;
        }
        default: {
            data = 6u;
            break;
        }
    }

    for (idx = 0; idx < ACQUNITY_ACQ_CHANNELS; idx++) {

    	acq_x_write_reg(idx, ADS_REG_IO, data);
    }
}



static bool trigger_rdc_single_shot(void)
{
	acq_x_trigger_enable();

	return (false);
}



static bool trigger_rdc_continious(void)
{
	acq_x_trigger_enable();

	return (true);
}



static bool trigger_rd_continuous(void)
{
	acq_sync_set_low();
	acq_x_trigger_sync_min_delay();
	acq_x_trigger_enable();
	acq_sync_set_high();

	return (true);
}

/* -------------------------------------------------------------------------- *
 * ACQ Consumer handling
 *
 * Consumers are attached to ACQ module to process the data out of main ISR.
 *
 * Each consumer is specified by:
 *  - is_enabled - consumers may be enabled or disabled
 *  - at_every - specifies for how many samples to wait before calling it
 *  - fn - is the consumer function which will be called sample specified by
 *         at every variable.
 *
 * For example, say that you have a module called TFT and it wants to get
 * every hundredth sample:
 * acq_consumer_add(tft_proto, 100)
 *
 * this will call function tft_proto() at every hundredth sample.
 * -------------------------------------------------------------------------- */


static bool
acq_consumer_activate(void)
{
	bool						retval;
	uint32_t					idx;

	retval = false;

	for (idx = 0; idx < g_consumers.count; idx++) {
		struct acq_consumer *   consumer = &g_consumers.consumer[idx];

		if (consumer->is_enabled) {
			consumer->sample_count++;

			if (consumer->sample_count == consumer->at_every) {
				consumer->sample_count = 0u;
				g_consumers.activated++;
				nsched_deferred_do(&g_consumers.consumer[idx].deferred_work);
				retval = true;
			}
		}
	}

	return (retval);
}



static void
acq_consumer_worker(void * arg)
{
	struct acq_consumer *		consumer = arg;

	consumer->fn(&g_consumers.samples[0]);
	g_consumers.activated--;
}



static void
acq_consumer_save_raw(const struct acq_sample * sample)
{
	g_consumers.samples[0] = *sample;
}



static void
acq_consumer_save_final(const struct acq_sample * sample)
{
	g_consumers.samples[1] = *sample;
}

/******************************************************************************
 * State machine
 ******************************************************************************/


static naction state_init (struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace *			ws = nsm_wspace(sm);

    switch (event->id) {
    	case NSM_INIT: {
    		return (naction_handled());
    	}

        case SIG_ACQ_INIT: {
            static const struct ms_bus_config ms_bus_config =
            {
                ms_bus_tx_complete,
                ms_bus_tx_error
            };
            ws->idx = 0;
            ws->producer = event->producer;

            netimer_init(&ws->timeout);

            data_process_init(&g_global_process);

            /* Set gain handler here, not pretty, but it works.
             */
            data_process_set_gain_handler(&g_global_process, gain_handler);

            notify_init_as_output();

            /* NOTE:
             * Initialize trigger input and output pins and associated timers
             */
            trigger_out_init();
            trigger_in_init();

            /* NOTE:
             * Initialize acquisition buses (SPI)
             */
            acq_x_bus_init();

            /* NOTE:
             * Initialize master bus (SPI + DMA)
             */
            ms_bus_init(&ms_bus_config);

            /* -------------------------------------------------------------- *
             * Initialization is finished									  *
             * 																  *
             * At this point all busses and pins are initialized. 			  *
             * -------------------------------------------------------------- */

			return (naction_transit_to(sm, state_reset_adcs));
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_reset_adcs(struct nsm * sm, const nevent * event)
{
	struct acq_wspace * 		ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
            /* NOTE:
             * Reset all ADCs. This will bring the default values to all
             * registers which we can use later to detected attached ADCs
             */
			acq_x_reset(ws->idx);

			netimer_after(&ws->timeout, NTIMER_MS(100), SIG_TIMEOUT_RESET);

			return (naction_handled());
		}
		case SIG_TIMEOUT_RESET : {
			ws->idx++;

			if (ws->idx == ACQUNITY_ACQ_CHANNELS) {
				uint32_t		detected = 0;
				uint32_t 		idx;

				for (idx = 0; idx < ACQUNITY_ACQ_CHANNELS; idx++) {
					uint32_t 	adc_drate;

					adc_drate = acq_x_read_reg(idx, ADS_REG_DRATE);

					if (adc_drate == ADS_DRATE_30KSPS) {
						ws->detected_adc_mask |= (0x1u << idx);
						detected++;
					}
				}

				if (detected) {
					nepa_send_signal(ws->producer, SIG_ACQ_READY);

					return (naction_transit_to(sm, state_idle));
				} else {
					nepa_send_signal(ws->producer, SIG_ACQ_NOT_READY);

					return (naction_transit_to(sm, state_dormant));
				}
			} else {

				return (naction_transit_to(sm, state_reset_adcs));
			}
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_dormant(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace * 		ws = nsm_wspace(sm);

	switch (event->id) {
		case SIG_ACQ_SET_CONFIG:
		case SIG_ACQ_UPDATE_CONFIG:
		case SIG_ACQ_START: {
			nepa_send_signal(event->producer, SIG_ACQ_NOT_READY);

			return (naction_handled());
		}
		case SIG_ACQ_STOP: {
			nepa_send_signal(event->producer, SIG_ACQ_READY);

			return (naction_handled());
		}
		case SIG_ACQ_INIT: {
			ws->producer = event->producer;

			return (naction_transit_to(sm, state_reset_adcs));
		}
		default: {
			return (naction_ignored());
		}
	}
}



static naction state_idle(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace *			ws = nsm_wspace(sm);

    switch (event->id) {
    	case NSM_ENTRY: {
    	    /*
    	     * TODO: Reset ADC status
    	     */

			return (naction_handled());
    	}
    	case NSM_EXIT: {
    	    /*
    	     * TODO: get detected ADCs status
    	     */

        	return (naction_handled());
    	}
    	case SIG_ACQ_SET_CONFIG: {
    		ws->producer = event->producer;

    		ws->idx = 0;

    		if (acq_apply_config(ws, &g_acq_current_config, &g_acq_pending_config)) {
    			if (g_acq_current_config.write_flags) {

    				return (naction_transit_to(sm, state_write_regs));
    			} else {
    				nepa_send_signal(ws->producer, SIG_ACQ_READY);

    				return (naction_handled());
    			}
    		} else {
    			nepa_send_signal(ws->producer, SIG_ACQ_NOT_READY);

    			return (naction_handled());
    		}
    	}
        case SIG_ACQ_START: {
        	ws->producer = event->producer;
        	netimer_after(&ws->timeout, NTIMER_MS(20), SIG_TIMEOUT_START);

        	return (naction_handled());
        }
        case SIG_ACQ_UPDATE_CONFIG:
        case SIG_ACQ_STOP: {
        	nepa_send_signal(event->producer, SIG_ACQ_READY);

			return (naction_handled());
        }
        case SIG_TIMEOUT_START: {
        	ws->idx = 0;

        	switch (g_acq_current_config.acq_mode) {
        		case ACQ_MODE_CONTINUOUS: {
        			return (naction_transit_to(sm, state_rdc_start_setup_rdc));
        		}
        		case ACQ_MODE_REQUEST: {

        			return (naction_transit_to(sm, state_rd_start));
        		}
        	}
        	nepa_send_signal(ws->producer, SIG_ACQ_NOT_READY);

			return (naction_handled());
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_write_regs(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace * 		ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			uint32_t			mask;

			mask = 0x1u << ws->idx;

			if (mask & g_acq_current_config.write_flags) {
				uint32_t		channel;

				for (channel = 0; channel < ACQUNITY_ACQ_CHANNELS; channel++) {
					acq_x_write_reg(channel, ws->idx, g_acq_current_config.adc_regs[ws->idx]);
				}
			}
			netimer_after(&ws->timeout, NTIMER_MS(2), SIG_TIMEOUT_WRITE);

			return (naction_handled());
		}
		case SIG_TIMEOUT_WRITE: {
			ws->idx++;

			if (ws->idx == sizeof(g_acq_current_config.adc_regs)) {
				nepa_send_signal(ws->producer, SIG_ACQ_READY);

				return (naction_transit_to(sm, state_idle));
			} else {

				return (naction_transit_to(sm, state_write_regs));
			}
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_rdc_start_setup_rdc(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			acq_x_start_rdc();
			netimer_after(&ws->timeout, NTIMER_MS(20), SIG_TIMEOUT_RDC);

			return (naction_handled());
		}
		case SIG_TIMEOUT_RDC: {

			return (naction_transit_to(sm, state_rdc_start_setup_isr));
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_rdc_start_setup_isr(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			acq_sync_set_low();  /* Turn off all ADCs, prepare for SYNC event */

			netimer_after(&ws->timeout, NTIMER_MS(200), SIG_TIMEOUT_ISR);

			return (naction_handled());
		}
		case NSM_EXIT: {
			/* NOTE:
			 * After this point all ADCs should be synchronized
			 */
			acq_sync_set_high();

			return (naction_handled());
		}
		case SIG_TIMEOUT_ISR: {
			switch (g_acq_current_config.trigger_mode) {
				case TRIG_MODE_OUT: {
					acq_x_isr_enable(acq_isr_begin_rdc_trigger_out);
					break;
				}
				case TRIG_MODE_IN_CONTINUOUS: {
					acq_x_isr_enable(acq_isr_begin_rdc_trigger_in_c);
					break;
				}
				case TRIG_MODE_IN_SINGLE_SHOT: {
					acq_x_isr_enable(acq_isr_begin_rdc_trigger_in_s);
					break;
				}
			}

			return (naction_transit_to(sm, state_rdc_running));
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_rdc_running(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace * 		ws = nsm_wspace(sm);

    switch (event->id) {
    	case NSM_ENTRY: {
    		switch (g_acq_current_config.trigger_mode) {
    			case TRIG_MODE_OUT: {
    				trigger_out_set_speed(g_acq_current_config.adc_regs[ADS_REG_DRATE]);
    				break;
    			}
    			case TRIG_MODE_IN_SINGLE_SHOT: {
    				trigger_in_enable(trigger_rdc_single_shot);
    				break;
    			}
    			case TRIG_MODE_IN_CONTINUOUS: {
    				trigger_in_enable(trigger_rdc_continious);
    				break;
    			}
    			default: {
    				break;
    			}
    		}

    		return (naction_handled());
    	}
    	case NSM_EXIT: {
    		switch (g_acq_current_config.trigger_mode) {
    			case TRIG_MODE_OUT: {
    				trigger_out_disable();
    				break;
    			}
    			case TRIG_MODE_IN_CONTINUOUS:
    			case TRIG_MODE_IN_SINGLE_SHOT: {
    				trigger_in_disable();
					break;
    			}
    			default: {
    				break;
    			}
    		}

    		return (naction_handled());
    	}
    	case SIG_ACQ_UPDATE_CONFIG: {
    		ws->producer = event->producer;

    		if (!acq_update_config(ws, &g_acq_current_config, &g_acq_pending_config)) {
    			nepa_send_signal(ws->producer, SIG_ACQ_BUSY);
    		} else {
    			nepa_send_signal(ws->producer, SIG_ACQ_READY);
    		}

    		return (naction_handled());
    	}
        case SIG_ACQ_STOP: {
        	ws->producer = event->producer;

        	return (naction_transit_to(sm, state_rdc_stop_destroy_isr));
        }
        case EVENT_ACQ_SET_GAIN: {
            const struct event_acq_set_gain * evt_gain = nevent_data(event);

            ws->gain = evt_gain->gain;

            return (naction_transit_to(sm, state_rdc_gain_destroy_isr));
        }
        default: {
            return (naction_ignored());
        }
    }
}



static naction state_rdc_stop_destroy_isr(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			acq_sync_set_low();						     /* Turn off all ADCs */
			netimer_after(&ws->timeout, NTIMER_MS(10), SIG_TIMEOUT_ISR);
			acq_x_isr_disable();

			return (naction_handled());
		}
		case SIG_TIMEOUT_ISR: {
			acq_sync_set_high();					/* Turn on all ADCS again */
			netimer_after(&ws->timeout, NTIMER_MS(20), SIG_TIMEOUT_ISR_WAIT);
			return (naction_handled());
		}
		case SIG_TIMEOUT_ISR_WAIT: {

			return (naction_transit_to(sm, state_rdc_stop_destroy_rdc));
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_rdc_stop_destroy_rdc(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace * 		ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			acq_x_stop_rdc();
			netimer_after(&ws->timeout, NTIMER_MS(20), SIG_TIMEOUT_RDC);

			return (naction_handled());
		}
		case SIG_TIMEOUT_RDC: {
			nepa_send_signal(ws->producer, SIG_ACQ_READY);

			return (naction_transit_to(sm, state_idle));
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_rdc_gain_destroy_isr(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			acq_sync_set_low();						     /* Turn off all ADCs */
			netimer_after(&ws->timeout, NTIMER_MS(10), SIG_TIMEOUT_ISR);
			acq_x_isr_disable();

			return (naction_handled());
		}
		case SIG_TIMEOUT_ISR: {
			acq_sync_set_high();
			netimer_after(&ws->timeout, NTIMER_MS(20), SIG_TIMEOUT_ISR_WAIT);

			return (naction_handled());
		}
		case SIG_TIMEOUT_ISR_WAIT: {

			return (naction_transit_to(sm, state_rdc_gain_destroy_rdc));
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_rdc_gain_destroy_rdc(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			acq_x_stop_rdc();
			netimer_after(&ws->timeout, NTIMER_MS(20), SIG_TIMEOUT_RDC);

			return (naction_handled());
		}
		case SIG_TIMEOUT_RDC: {

			return (naction_transit_to(sm, state_rdc_gain_set_io));
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_rdc_gain_set_io(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			acq_apply_gain(ws->gain);
			data_process_gain_changed(&g_global_process, ws->gain);
			netimer_after(&ws->timeout, NTIMER_MS(2), SIG_TIMEOUT_IO);

			return (naction_handled());
		}
		case SIG_TIMEOUT_IO: {

			return (naction_transit_to(sm, state_rdc_gain_wait_period));
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_rdc_gain_wait_period(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			netimer_after(&ws->timeout, NTIMER_MS(30), SIG_TIMEOUT_GAIN_WAIT);

			return (naction_handled());
		}
		case SIG_TIMEOUT_GAIN_WAIT: {

			return (naction_transit_to(sm, state_rdc_start_setup_rdc));
		}
		default: {

			return (naction_ignored());
		}
	}
}



static naction state_rd_start(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			acq_sync_set_low();

			netimer_after(&ws->timeout, NTIMER_MS(10), SIG_TIMEOUT_ISR);

			return (naction_handled());
		}
		case NSM_EXIT: {
			acq_sync_set_high();

			return (naction_handled());
		}
		case SIG_TIMEOUT_ISR: {
			acq_x_isr_enable(acq_isr_begin_rc_trigger_in_c);

			return (naction_transit_to(sm, state_rd_running));
		}
		default: {

			return (naction_ignored());
		}
	}
}




static naction state_rd_running(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace * 		ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			switch (g_acq_current_config.trigger_mode) {
    			case TRIG_MODE_OUT: {
    				trigger_out_set_speed(g_acq_current_config.adc_regs[ADS_REG_DRATE]);
    				break;
    			}
				case TRIG_MODE_IN_CONTINUOUS: {
					trigger_in_enable(trigger_rd_continuous);
					break;
				}
				default: {
					break;
				}
			}

			return (naction_handled());
		}
		case NSM_EXIT: {
    		switch (g_acq_current_config.trigger_mode) {
    			case TRIG_MODE_OUT: {
    				trigger_out_disable();
    				break;
    			}
    			case TRIG_MODE_IN_CONTINUOUS:
    			case TRIG_MODE_IN_SINGLE_SHOT: {
    				trigger_in_disable();
					break;
    			}
    			default: {
    				break;
    			}
    		}

			return (naction_handled());
		}
		case SIG_ACQ_UPDATE_CONFIG: {
			ws->producer = event->producer;

			nepa_send_signal(ws->producer, SIG_ACQ_NOT_READY);

			return (naction_handled());
		}
		case SIG_ACQ_STOP: {
			ws->producer = event->producer;

			return (naction_transit_to(sm, state_rd_stop));
		}
		case EVENT_ACQ_SET_GAIN: {
			const struct event_acq_set_gain * evt_gain = nevent_data(event);

			ws->gain = evt_gain->gain;

			return (naction_transit_to(sm, state_rdc_gain_destroy_isr));
		}
		default: {
			return (naction_ignored());
		}
	}
}



static naction state_rd_stop(struct nsm * sm, const struct nevent * event)
{
	struct acq_wspace *			ws = nsm_wspace(sm);

	switch (event->id) {
		case NSM_ENTRY: {
			acq_sync_set_low();
			acq_x_isr_disable();

			netimer_after(&ws->timeout, NTIMER_MS(10), SIG_TIMEOUT_ISR_WAIT);

			return (naction_handled());
		}
		case SIG_TIMEOUT_ISR_WAIT: {
			acq_sync_set_high();
			nepa_send_signal(ws->producer, SIG_ACQ_READY);

			return (naction_transit_to(sm, state_idle));
		}
		default: {

			return (naction_ignored());
		}
	}
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

struct acq_consumer * acq_consumer_add(void (*fn)(const struct acq_sample *),
		uint32_t at_every)
{
	struct acq_consumer *		consumer;

	if (g_consumers.count == NARRAY_DIMENSION(g_consumers.consumer)) {
		return (NULL);
	}
	consumer = &g_consumers.consumer[g_consumers.count];
	consumer->is_enabled 	= false;
	consumer->sample_count	= 0u;
	consumer->at_every 	 	= at_every;
	consumer->fn 			= fn;
	nsched_deferred_init(&consumer->deferred_work, acq_consumer_worker,
			consumer);
	g_consumers.count++;

	return (consumer);
}

void acq_consumer_at_every(struct acq_consumer * consumer, uint32_t at_every)
{
	consumer->sample_count 	= 0u;
	consumer->at_every 		= at_every;
}

void acq_consumer_enable(struct acq_consumer * consumer)
{
	consumer->sample_count 	= 0u;
	consumer->is_enabled 	= true;
}

void acq_consumer_disable(struct acq_consumer * consumer)
{
	consumer->is_enabled = false;
}

/******************************************************************************
 * PPBUF callback
 ******************************************************************************/

void ppbuff_one_sample_callback(struct acq_sample * sample)
{
	bool						consumer_should_activate;

	consumer_should_activate = acq_consumer_activate();

	if (consumer_should_activate) {
		acq_consumer_save_raw(sample);
	}
	data_process_acq(&g_global_process, sample);

	if (consumer_should_activate) {
		acq_consumer_save_final(sample);
	}
}



void ppbuff_full_callback(bool consumer_is_locked)
{
    if (!consumer_is_locked) {
        void *                  buffer;
        uint16_t                size;

        ppbuff_lock_consumer(&g_pp_buff);
        buffer = ppbuff_consumer_base(&g_pp_buff);
        size   = (uint16_t)(ppbuff_size(&g_pp_buff) * sizeof(struct acq_sample));
        ms_bus_start_tx(buffer, size);
    }
    notify_enable();
}



void ppbuff_half_full_callback(void)
{
    notify_disable();
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of mem_class.c
 ******************************************************************************/


