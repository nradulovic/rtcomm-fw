/**@file
 * @author 		Nenad Radulovic
 * @date		Jan 29, 2016
 */

/*=========================================================  INCLUDE FILES  ==*/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "port/compiler.h"
#include "data_process.h"
#include "cdi/io.h"
#include "epa_calibration.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/

#if defined(OPT_USE_DOUBLE)
typedef double fdata;
#else
typedef float fdata;
#endif
typedef int32_t idata;

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

struct process_handle			g_global_process;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/


PORT_C_INLINE bool
gain_pend_lower(struct range_data * range)
{
    if (range->pend_gain_level) {
        range->pend_gain_level--;
        range->pend_gain_handler(range->pend_gain_level);

        return (true);
    } else {

        return (false);
    }
}



PORT_C_INLINE bool
gain_pend_raise(struct range_data * range)
{
    if (range->pend_gain_level < (DATA_PROCESS_GAIN_LEVELS - 1)) {
        range->pend_gain_level++;
        range->pend_gain_handler(range->pend_gain_level);

        return (true);
    } else {

        return (false);
    }
}



static void
range_init(struct process_handle * handle)
{
    uint32_t                    idx;

    for (idx = 0; idx < DATA_PROCESS_GAIN_LEVELS; idx++)
    {
        handle->config.range.gain[idx].hi = INT32_MAX;
        handle->config.range.gain[idx].lo = 0;
    }
    handle->config.range.attack_time   = UINT32_MAX;
    handle->config.range.pullback_time = UINT32_MAX;
    handle->data.range.pend_gain_level = 3;
    handle->data.range.curr_gain_level = 3;
    handle->data.range.state           = STATE_INIT;
}



static void
range_process_sample(struct process_handle * handle,
		const struct acq_sample * sample)
{
    uint32_t                    channel;
    int32_t                     amplitude;
    struct range_data *         range = &handle->data.range;
    struct range_config *       range_cfg = &handle->config.range;

    /* NOTE:
     * Calculate max amplitude from all 3 channels
     */
    amplitude = 0;

    for (channel = 0; channel < ACQUNITY_ACQ_CHANNELS; channel++) {
        int32_t                 abs_val;

        abs_val = abs(sample_get_int(sample, channel));

        if (amplitude < abs_val) {
            amplitude = abs_val;
        }
    }

    switch (range->state) {
        case STATE_INIT:
        {
            range->state = STATE_STABLE;
            range->attack_counter   = 0;
            range->pullback_counter = 0;
            break;
        }
        case STATE_STABLE:
        {
            if (amplitude > range_cfg->gain[range->curr_gain_level].hi) {
                range->pullback_counter = 0;
                range->attack_counter++;

                if (range->attack_counter >= range_cfg->attack_time) {

                    if (gain_pend_lower(range)) {
                        range->state = STATE_TRANSIT;
                    }
                }
            } else if (amplitude < range_cfg->gain[range->curr_gain_level].lo) {
                range->attack_counter = 0;
                range->pullback_counter++;

                if (range->pullback_counter >= range_cfg->pullback_time) {

                    if (gain_pend_raise(range)) {
                        range->state = STATE_TRANSIT;
                    }
                }
            } else {
                range->attack_counter   = 0;
                range->pullback_counter = 0;
            }
            break;
        }
        case STATE_TRANSIT:
        {
            range->attack_counter   = 0;
            range->pullback_counter = 0;

            if (range->curr_gain_level == range->pend_gain_level) {
                range->state = STATE_STABLE;
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/



void data_process_init(struct process_handle * handle)
{
	range_init(handle);
}



void data_process_acq(struct process_handle * handle, struct acq_sample * sample)
{
	sample_set_gain(sample, handle->data.range.curr_gain_level);

	if (handle->config.flags & DATA_PROCESS_ENABLE_RANGE) {
		range_process_sample(handle, sample);
	}
}



void data_process_adt7410(struct process_handle * handle, int16_t sample)
{
	(void)handle;

}



void data_process_set_range_hi_limit(struct process_handle * handle,
		int32_t limit, uint32_t level)
{
    handle->config.range.gain[level].hi = limit;
}



void data_process_set_range_lo_limit(struct process_handle * handle,
		int32_t limit, uint32_t level)
{
    handle->config.range.gain[level].lo = limit;
}



void data_process_set_range_attack_time(struct process_handle * handle,
		uint32_t attack_time)
{
    handle->config.range.attack_time = attack_time;
}



void data_process_set_range_pullback_time(struct process_handle * handle,
		uint32_t pullback_time)
{
    handle->config.range.pullback_time = pullback_time;
}



void data_process_set_gain_handler(struct process_handle * handle,
		void (* gain_handler)(uint32_t gain))
{
    handle->data.range.pend_gain_handler = gain_handler;
}



void data_process_set_flags(struct process_handle * handle, uint32_t flags)
{
	handle->config.flags = flags;
}



uint32_t data_process_get_gain_level(struct process_handle * handle)
{
    return (handle->data.range.curr_gain_level);
}



void data_process_gain_changed(struct process_handle * handle,
		uint32_t new_gain)
{
    handle->data.range.curr_gain_level = new_gain;
}

/** @endcond *//***************************************************************
 * END of data_process.c
 ******************************************************************************/
