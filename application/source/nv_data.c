/*
 * nv_data.c
 *
 *  Created on: Mar 3, 2016
 *      Author: nenad
 */

/*=========================================================  INCLUDE FILES  ==*/

#include "nv_data.h"
#include "stm32f4xx.h"
#include "base/debug.h"
#include "base/component.h"
#include "main.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define CONFIG_FIRST_SECTOR			1
#define CONFIG_NB_OF_SECTORS		1

/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

#if defined(PORT_C_GCC)
uint8_t _snvdata[NV_DATA_SIZE] __attribute__((section(".nv_data")));
#elif defined(PORT_C_ARMCC)
uint8_t _snvdata[NV_DATA_SIZE] __attribute__((section(".nv_data")));
#endif

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void nv_data_init(void)
{
	/* NOTE:
	 * Check if linker was setup correctly
	 */
	//NASSERT("Linker error", (END_OF_NV_DATA - START_OF_NV_DATA) != ATTRIB_SECTOR_SIZE);
}


nerror nv_data_read_u8(uint8_t * data_u8, uint32_t address)
{
	return (nv_data_read_block(data_u8, 1, address));
}



nerror nv_data_read_block(uint8_t * data_u8, uint32_t size, uint32_t address)
{
	if (((address + size) > NV_DATA_SIZE) || (address == NV_DATA_SIZE)) {
		return (NERROR_ARG_INVALID);
	}
	nv_data_fast_read_block(data_u8, size, address);

	return (NERROR_NONE);
}



nerror nv_data_write_u8(uint8_t data_u8, uint32_t address)
{
	return (nv_data_write_block(&data_u8, 1, address));
}



nerror nv_data_erase_all(void)
{
	nerror						error = NERROR_NONE;
	uint32_t					SectorError;
	HAL_StatusTypeDef 			status;
	FLASH_EraseInitTypeDef		EraseInitStruct;

	status = HAL_FLASH_Unlock();

	if (status != HAL_OK) {
		error = NERROR_DEVICE_BUSY;
		goto ERR_FLASH_LOCK;
	}

	/*
	 * Erase the flash
	 */
	EraseInitStruct.TypeErase    = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector       = CONFIG_FIRST_SECTOR;
	EraseInitStruct.NbSectors    = CONFIG_NB_OF_SECTORS;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
		/*
		 *  Try a second time
		 */
		HAL_Delay(100);

		if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
			error = NERROR_DEVICE_FAIL;
			goto ERR_SECTOR_ERASE;
		}
	}
	/*
	 * NOTE:
	 * If an erase operation in Flash memory also concerns data in the data or instruction cache,
	 * you have to make sure that these data are rewritten before they are accessed during code
	 * execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	 * DCRST and ICRST bits in the FLASH_CR register.
	 */
	__HAL_FLASH_DATA_CACHE_DISABLE();
	__HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

	__HAL_FLASH_DATA_CACHE_RESET();
	__HAL_FLASH_INSTRUCTION_CACHE_RESET();

	__HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
	__HAL_FLASH_DATA_CACHE_ENABLE();

	HAL_FLASH_Lock();

	return (error);
ERR_SECTOR_ERASE:
	HAL_FLASH_Lock();
ERR_FLASH_LOCK:

	return (error);
}



nerror nv_data_write_block_raw(const void * data, uint32_t size, uint32_t address)
{
	nerror						error = NERROR_NONE;
	const uint8_t * 			buffer_u8 = data;
	uint32_t					idx;
	HAL_StatusTypeDef 			status;

	if (((address + size) > NV_DATA_SIZE) || (address == NV_DATA_SIZE) ||
		 ((size % 4u) != 0) || ((address % 4u) != 0)) {
		error = NERROR_ARG_INVALID;
		goto ERR_ARG_INVALID;
	}
	status = HAL_FLASH_Unlock();

	if (status != HAL_OK) {
		error = NERROR_DEVICE_BUSY;
		goto ERR_FLASH_LOCK;
	}

	for (idx = 0; idx < size; idx += 4u) {
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)&_snvdata[0] + idx + address, *(uint32_t *)&buffer_u8[idx]) != HAL_OK) {
			error = NERROR_DEVICE_FAIL;
			goto ERR_PROGRAM;
		}
	}
	HAL_FLASH_Lock();

	return (error);
ERR_PROGRAM:
	HAL_FLASH_Lock();
ERR_FLASH_LOCK:
ERR_ARG_INVALID:

	return (error);
}



nerror nv_data_write_block(const void * data, uint32_t size, uint32_t address)
{
	nerror						error = NERROR_NONE;
	uint8_t * 					buffer_u8;
	uint32_t					SectorError;
	uint32_t					idx;

	HAL_StatusTypeDef 			status;
	FLASH_EraseInitTypeDef		EraseInitStruct;

	if (((address + size) > NV_DATA_SIZE) || (address == NV_DATA_SIZE)) {
		error = NERROR_ARG_INVALID;
		goto ERR_ARG_INVALID;
	}
	status = HAL_FLASH_Unlock();

	if (status != HAL_OK) {
		error = NERROR_DEVICE_BUSY;
		goto ERR_FLASH_LOCK;
	}
	buffer_u8 = nheap_alloc(&g_generic_heap, NV_DATA_SIZE);

	if (!buffer_u8) {
		error = NERROR_NO_MEMORY;
		goto ERR_ALLOC_BUFFER;
	}

	/*
	 * First, copy all data to temporary buffer.
	 */
	memcpy(buffer_u8, &_snvdata[0], NV_DATA_SIZE);

	/*
	 * Modify the requested bytes
	 */
	memcpy(&buffer_u8[address], data, size);

	/*
	 * Erase the flash
	 */
	EraseInitStruct.TypeErase    = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector       = CONFIG_FIRST_SECTOR;
	EraseInitStruct.NbSectors    = CONFIG_NB_OF_SECTORS;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
		/*
		 *  Try a second time
		 */
		HAL_Delay(100);

		if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
			error = NERROR_DEVICE_FAIL;
			goto ERR_SECTOR_ERASE;
		}
	}
	/*
	 * NOTE:
	 * If an erase operation in Flash memory also concerns data in the data or instruction cache,
	 * you have to make sure that these data are rewritten before they are accessed during code
	 * execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	 * DCRST and ICRST bits in the FLASH_CR register.
	 */
	__HAL_FLASH_DATA_CACHE_DISABLE();
	__HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

	__HAL_FLASH_DATA_CACHE_RESET();
	__HAL_FLASH_INSTRUCTION_CACHE_RESET();

	__HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
	__HAL_FLASH_DATA_CACHE_ENABLE();

	/*
	 * Write the temporary buffer back to flash
	 */

	for (idx = 0; idx < NV_DATA_SIZE; idx += 4u) {
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)&_snvdata[0] + idx, *(uint32_t *)&buffer_u8[idx]) != HAL_OK) {
			error = NERROR_DEVICE_FAIL;
			goto ERR_PROGRAM;
		}
	}
	nheap_free(&g_generic_heap, buffer_u8);
	HAL_FLASH_Lock();

	return (error);
ERR_PROGRAM:
ERR_SECTOR_ERASE:
	nheap_free(&g_generic_heap, buffer_u8);
ERR_ALLOC_BUFFER:
	HAL_FLASH_Lock();
ERR_FLASH_LOCK:
ERR_ARG_INVALID:

	return (error);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of nv_data.c
 ******************************************************************************/
