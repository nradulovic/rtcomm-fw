/**@file
 * @brief		Short description
 * @author		Nenad Radulovic
 * @date		Jan 29, 2016
 */

#ifndef APPLICATION_INCLUDE_DATA_PROCESS_H_
#define APPLICATION_INCLUDE_DATA_PROCESS_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>
#include <stdbool.h>

#include "cdi/io.h"

/*===============================================================  MACRO's  ==*/

#define DATA_PROCESS_ENABLE_RANGE			(0x1u << 0)
#define DATA_PROCESS_ENABLE_MATH			(0x1u << 1)
#define DATA_PROCESS_MATH_FLAG_TE			(0x1u << 2)
#define DATA_PROCESS_MATH_FLAG_TCO			(0x1u << 3)
#define DATA_PROCESS_MATH_FLAG_S			(0x1u << 4)

#define DATA_PROCESS_GAIN_LEVELS        4u

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

struct acq_sample;

struct process_handle
{
    struct process_data
    {
        struct range_data
        {
            uint32_t                    attack_counter;
            uint32_t                    pullback_counter;
            uint32_t                    pend_gain_level;
            uint32_t                    curr_gain_level;
            void                     (* pend_gain_handler)(uint32_t pend_gain);
			enum fsm_range_state
			{
				STATE_INIT,
				STATE_STABLE,
				STATE_TRANSIT,
			}                           state;
        }                           range;
    }                           data;
    struct process_runtime_config
    {
        struct range_config
        {
            struct limits
            {
                int32_t                     lo;
                int32_t                     hi;
            }                           gain[DATA_PROCESS_GAIN_LEVELS];
            uint32_t                    attack_time;
            uint32_t                    pullback_time;
        }                           range;
        uint32_t					flags;
    }                           config;
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct process_handle	g_global_process;

/*===================================================  FUNCTION PROTOTYPES  ==*/

/*------------------------------------------------------------------------*//**
 * @name        Data processing input functions
 * @{ *//*--------------------------------------------------------------------*/

void data_process_init(struct process_handle * handle);

/**@} *//*----------------------------------------------------------------*//**
 * @name        Data processing input functions
 * @{ *//*--------------------------------------------------------------------*/

void data_process_acq(struct process_handle * handle, struct acq_sample * sample);
void data_process_aux(struct process_handle * handle, int32_t sample, uint32_t idx);
void data_process_adt7410(struct process_handle * handle, int16_t sample);

/**@} *//*----------------------------------------------------------------*//**
 * @name        Data processing config functions
 * @{ *//*--------------------------------------------------------------------*/

void data_process_set_range_hi_limit(struct process_handle * handle, int32_t limit, uint32_t level);
void data_process_set_range_lo_limit(struct process_handle * handle, int32_t limit, uint32_t level);
void data_process_set_range_attack_time(struct process_handle * handle, uint32_t attack_time);
void data_process_set_range_pullback_time(struct process_handle * handle, uint32_t pullback_time);
void data_process_set_gain_handler(struct process_handle * handle, void (* gain_handler)(uint32_t gain));
void data_process_set_flags(struct process_handle * handle, uint32_t flags);

/**@} *//*----------------------------------------------------------------*//**
 * @name        Data processing get functions
 * @{ *//*--------------------------------------------------------------------*/

uint32_t data_process_get_gain_level(struct process_handle * handle);

void data_process_gain_changed(struct process_handle * handle, uint32_t new_gain);

/**@} *//*------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of process_data.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_DATA_PROCESS_H_ */
