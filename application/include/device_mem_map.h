/*
 * device_mem_map.h
 *
 *  Created on: May 27, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Device memory map
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_DEVICE_MEM_MAP_H_
#define APPLICATION_INCLUDE_DEVICE_MEM_MAP_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>

#include "aux_channels.h"
#include "port/compiler.h"
#include "cdi/io.h"

/*===============================================================  MACRO's  ==*/

#define DEV_ACTIVE_EEPROM			(0u)
#define DEV_ACTIVE_FRAM				(1u)
#define DEV_ACTIVE_TED				(2u)
#define DEV_ACTIVE_AUX				(3u)
#define DEV_ACTIVE_ACQ_X			(4u)
#define DEV_ACTIVE_ACQ_Y			(5u)
#define DEV_ACTIVE_ACQ_Z			(6u)
#define DEV_ACTIVE_PROTOCOL			(7u)
#define DEV_ACTIVE_TFT				(8u)
#define DEV_ACTIVE_CALIBRATION		(9u)

#define DEV_AUX_ARRAY_DIMENSION		8

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/
    
struct mem_map
{
	volatile uint32_t			_guard;
	bool 						active[16];
	struct ted_values
	{
		int16_t						raw;
		float						final;
	}	 ted;
	struct aux_values
	{
		int32_t						raw[DEV_AUX_ARRAY_DIMENSION];
		float						volt[DEV_AUX_ARRAY_DIMENSION];
		float						final[DEV_AUX_ARRAY_DIMENSION];
	}							    aux;
	struct acq_values
	{
		struct acq_sample			raw;
		struct acq_sample			volt;
		struct acq_sample			final;
	}			acq;
};

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

/**@brief		Access mem_map structure members.
 * @details		To get a value:
 *              float a = mem_map()->aux.volt[0];
 * 				To set a value:
 * 				mem_map()->active[DEV_ACTIVE_TFT] = true;
 *
 * @return
 */
PORT_C_INLINE
struct mem_map * mem_map(void)
{
	extern struct mem_map g_dev_mem_map;

	return (&g_dev_mem_map);
}



PORT_C_INLINE
void mem_map_put(void)
{
	extern struct mem_map g_dev_mem_map;

	g_dev_mem_map._guard++;
}


/**@brief		Use this function when you want to get multiple values at once
 * @details		If you access mem_map structure directly there is a possibility
 * 				that you will get values from multiple, different sampling
 * 				periods.
 *
 * 				This function will ensure that you'll get values from single
 * 				sampling period.
 *
 * @param		member - Is the member of mem_map structure
 * @param		ptr - pointer to variable where to store values
 * @example		float final_values[DEV_AUX_ARRAY_DIMENSION];
 *              mem_map_get(aux.final, final_values);
 */
#define mem_map_get(member, ptr) 												\
	do {																		\
		extern struct mem_map 	g_dev_mem_map;  								\
		uint32_t 				first;											\
																				\
		do {																	\
			first = g_dev_mem_map._guard;										\
			memcpy((ptr), &g_dev_mem_map.member, sizeof(*(ptr)));				\
		} while (first != g_dev_mem_map._guard);								\
	} while (0)



/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of device_mem_map.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_DEVICE_MEM_MAP_H_ */
