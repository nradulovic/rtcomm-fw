/*
 * epa_acq.h
 *
 *  Created on: May 28, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Event Processing Agent for Acquisition ADC
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_EPA_ACQ_H_
#define APPLICATION_INCLUDE_EPA_ACQ_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "config_epa.h"
#include "neon_eds.h"

/*===============================================================  MACRO's  ==*/

#define ACQ_SET_ADC_STATUS				(0x1u << 0)
#define ACQ_SET_ADC_MUX					(0x1u << 1)
#define ACQ_SET_ADC_ADCON				(0x1u << 2)
#define ACQ_SET_ADC_DRATE				(0x1u << 3)
#define ACQ_SET_ADC_IO					(0x1u << 4)
#define ACQ_SET_ADC_OFC_0				(0x1u << 5)
#define ACQ_SET_ADC_OFC_1				(0x1u << 6)
#define ACQ_SET_ADC_OFC_2				(0x1u << 7)
#define ACQ_SET_ADC_FSC_0				(0x1u << 8)
#define ACQ_SET_ADC_FSC_1				(0x1u << 9)
#define ACQ_SET_ADC_FSC_2				(0x1u << 10)

#if !defined(EPA_ACQ_EVENT_BASE_ID)
# error "Define EPA_ADT7410_EVENT_BASE_ID event ID base"
#endif

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

enum epa_acq_events
{
	SIG_ACQ_INIT      			= EPA_ACQ_EVENT_BASE_ID,
	SIG_ACQ_READY,
	SIG_ACQ_NOT_READY,
	SIG_ACQ_BUSY,
	SIG_ACQ_START,
        SIG_ACQ_STOP,
	SIG_ACQ_UPDATE_CONFIG,
	SIG_ACQ_SET_CONFIG,
        EVENT_ACQ_SET_GAIN
};



struct event_acq_set_gain
{
    struct nevent               super;
    uint32_t                    gain;
};

struct acq_config
{
	uint8_t       				adc_regs[11];
	uint32_t                    write_flags;

	/*
	 * ACQ_CHANNEL_X_MASK - X channel mask
	 * ACQ_CHANNEL_Y_MASK - Y channel mask
	 * ACQ_CHANNEL_Z_MASK - Z channel mask
	 */
	uint32_t					enabled_adc_mask;
	uint32_t                    buff_size;

	/*
	 * TRIG_MODE_OUT,
	 * TRIG_MODE_IN_CONTINUOUS,
	 * TRIG_MODE_IN_SINGLE_SHOT
	 */
	uint32_t   					trigger_mode;

	/*
	 * ACQ_MODE_CONTINUOUS - ADC mode with RDATAC command,
	 * ACQ_MODE_REQUEST - ADC mode with RDATA command
	 */
	uint32_t					acq_mode;
	int32_t                     range_hi[4];
	int32_t                     range_lo[4];
	uint32_t                    attack_time;
	uint32_t                    pullback_time;
	uint32_t					data_process_flags;
};

struct acq_consumer;
struct acq_sample;

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct nepa              g_epa_acq;

extern const struct nepa_define g_epa_acq_define;

extern struct acq_config 		g_acq_pending_config;

extern const struct acq_config  g_acq_default_config;

/*===================================================  FUNCTION PROTOTYPES  ==*/

PORT_C_INLINE
const struct acq_config *		acq_get_config(void)
{
	extern struct acq_config	g_acq_current_config;

	return (&g_acq_current_config);
}

struct acq_consumer * acq_consumer_add(void (*fn)(const struct acq_sample *),
		uint32_t at_every);

void acq_consumer_at_every(struct acq_consumer * consumer, uint32_t at_every);

void acq_consumer_enable(struct acq_consumer * consumer);

void acq_consumer_disable(struct acq_consumer * consumer);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of epa_acq.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_EPA_ACQ_H_ */
