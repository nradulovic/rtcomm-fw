/*
 * device_mem_map.c
 *
 *  Created on: May 27, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Device memory map implementation
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include <string.h>

#include "device_mem_map.h"
#include "port/compiler.h"
#include "port/core.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

struct PORT_C_ALIGN(NCPU_DATA_ALIGNMENT) mem_map g_dev_mem_map;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of device_mem_map.c
 ******************************************************************************/


