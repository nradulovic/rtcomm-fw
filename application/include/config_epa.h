/*
 * events.h
 *
 *  Created on: Mar 3, 2016
 *      Author: nenad
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       EPA configuration and definitions
 *********************************************************************//** @{ */


#ifndef APPLICATION_INCLUDE_CONFIG_EPA_H_
#define APPLICATION_INCLUDE_CONFIG_EPA_H_

/*=========================================================  INCLUDE FILES  ==*/
/*===============================================================  MACRO's  ==*/

/*
 * Event Base ID
 */
#define EPA_MAIN_EVENT_BASE_ID			1000
#define EPA_ADT7410_EVENT_BASE_ID     	2000
#define EPA_ACQ_EVENT_BASE_ID          	3000
#define EPA_AUX_EVENT_BASE_ID			4000
#define EPA_HEARTBEAT_BASE_ID			5000
#define EPA_EEPROM_EVENT_BASE_ID   		6000
#define EPA_PROTOCOL_EVENT_BASE_ID		7000
#define EPA_I2C_EVENT_BASE_ID			8000
#define EPA_TFT_EVENT_BASE_ID			9000
#define EPA_ACQ_INTERFACE_EVENT_BASE_ID 10000
#define EPA_CALIBRATION_EVENT_BASE_ID 	11000

/*
 * Priority
 */
#define EPA_MAIN_PRIO					31
#define EPA_ACQ_PRIO					30
#define EPA_AUX_PRIO					29
#define EPA_I2C_PRIO					5
#define EPA_PROTOCOL_PRIO				4
#define EPA_EEPROM_PRIO					3
#define EPA_FRAM_PRIO					3
#define EPA_ADT7410_PRIO				2
#define EPA_TFT_PRIO					2
#define EPA_CALIBRATION_PRIO			2
#define EPA_ACQ_INT_PRIO                2
#define EPA_HEARTBEAT_PRIO				1

/*
 * Event queue size
 */
#define EPA_PROTOCOL_QUEUE_SIZE			16
#define EPA_I2C_QUEUE_SIZE				32
#define EPA_EEPROM_QUEUE_SIZE			8
#define EPA_FRAM_QUEUE_SIZE				8
#define EPA_ACQ_QUEUE_SIZE				8
#define EPA_AUX_QUEUE_SIZE				8
#define EPA_ADT7410_QUEUE_SIZE			8
#define EPA_HEARTBEAT_QUEUE_SIZE		8
#define EPA_TFT_QUEUE_SIZE              10
#define EPA_MAIN_QUEUE_SIZE				8
#define EPA_CALIBRATION_QUEUE_SIZE		8
#define EPA_ACQINTC_QUEUE_SIZE          8


/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/
/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of events.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_CONFIG_EPA_H_ */
