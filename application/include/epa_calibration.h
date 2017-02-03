/*
 * epa_main.h
 *
 *  Created on: July 14, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Event Processing Agent for Calibration process
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_EPA_CALIBRATION_H_
#define APPLICATION_INCLUDE_EPA_CALIBRATION_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "base/bitop.h"
#include "config_epa.h"
#include "neon_eds.h"
#include "nv_data.h"
#include "stm32f4xx.h"

/*===============================================================  MACRO's  ==*/

#define calib_u8(address)														\
	*(uint8_t *)nv_data_get(address)

#define calib_u16(address)														\
	(uint16_t)__REV16(*(uint32_t *)nv_data_get(address))

#define calib_u32(address)														\
	__REV(*(uint32_t *)nv_data_get(address))

#define calib_f_a(address, idx)													\
	calib_f((idx) * sizeof(float) + (address))

#define calib_u8_a(address, idx)												\
	calib_u8((idx) * sizeof(uint8_t) + (address))

#define calib_u16_a(address, idx)												\
	calib_u16((idx) * sizeof(uint16_t) + (address))

#define calib_u32_a(address, idx)												\
	calib_u32((idx) * sizeof(uint32_t) + (address))

#if !defined(EPA_CALIBRATION_EVENT_BASE_ID)
# error "Define EPA_CALIBRATION_EVENT_BASE_ID event ID base"
#endif

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

enum epa_calibration_events
{
	/* Input */
	SIG_CALIBRATION_INIT = EPA_CALIBRATION_EVENT_BASE_ID,

	/* Output */
	SIG_CALIBRATION_READY,
	SIG_CALIBRATION_NOT_READY,
};

/*======================================================  GLOBAL VARIABLES  ==*/

extern struct nepa              g_epa_calibration;

extern const struct nepa_define g_epa_calibration_define;

/*===================================================  FUNCTION PROTOTYPES  ==*/


PORT_C_INLINE
float calib_f(uint32_t address)
{
	uint32_t 					data = calib_u32(address);

	return (n_u32_to_float(data));
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of epa_calibration.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_EPA_CALIBRATION_H_ */
