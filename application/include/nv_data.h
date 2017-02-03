/*
 * nv_data.h
 *
 *  Created on: Mar 3, 2016
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_NV_DATA_H_
#define APPLICATION_INCLUDE_NV_DATA_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>
#include <string.h>

#include "port/compiler.h"
#include "base/error.h"

/*===============================================================  MACRO's  ==*/

#define NV_DATA_START                   (0x08000000 + 0x4000)
#define NV_DATA_SIZE			        0x4000

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

/* NOTE:
 * These variables are defined by linker script
 */
#if defined(PORT_C_GCC)
extern uint8_t _snvdata[NV_DATA_SIZE] __attribute__((section(".nv_data")));
#elif defined(PORT_C_ARMCC)
extern uint8_t _snvdata[NV_DATA_SIZE] __attribute__((section(".nv_data")));
#endif

/*===================================================  FUNCTION PROTOTYPES  ==*/


void nv_data_init(void);



PORT_C_INLINE
void * nv_data_get(uint32_t address)
{
	return (&_snvdata[address]);
}



PORT_C_INLINE
void nv_data_fast_read_u8(uint8_t * data_u8, uint32_t address)
{
	*data_u8 = _snvdata[address];
}



PORT_C_INLINE
void nv_data_fast_read_block(void * data, uint32_t size, uint32_t address)
{
	memcpy(data, &_snvdata[address], size);
}



nerror nv_data_read_u8(uint8_t * data_u8, uint32_t address);



nerror nv_data_read_block(uint8_t * data_u8, uint32_t size, uint32_t address);



nerror nv_data_write_u8(uint8_t data_u8, uint32_t address);



nerror nv_data_write_block(const void * data, uint32_t size, uint32_t address);



nerror nv_data_erase_all(void);



nerror nv_data_write_block_raw(const void * data, uint32_t size, uint32_t address);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of nv_data.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_NV_DATA_H_ */
