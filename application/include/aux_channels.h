/*
 * aux_channels.h
 *
 *  Created on: May 26, 2015
 *      Author: Nenad Radulovic
 */

#ifndef APPLICATION_INCLUDE_AUX_CHANNELS_H_
#define APPLICATION_INCLUDE_AUX_CHANNELS_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdbool.h>
#include <stdint.h>

#include "hw_config.h"

/*===============================================================  MACRO's  ==*/
/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

void aux_channel_init(void);



void aux_set_signal(
    void                     (* signal)(void));



void aux_signal_enable(void);



void aux_signal_disable(void);



void aux_reset(void);



void aux_write_reg(
    uint32_t                    reg,
    uint8_t                     data);



uint8_t aux_read_reg(
    uint32_t                    reg);



uint8_t aux_write_then_read_reg(uint32_t reg, uint8_t data);



void aux_write_cmd(
    uint8_t                     cmd);



uint32_t aux_read_data(void);



bool aux_is_drdy_active(void);



void acq_aux_drdy_isr(void);


/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of aux_channels.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_AUX_CHANNELS_H_ */
