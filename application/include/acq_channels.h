/*
 * acq_channels.h
 *
 *  Created on: May 26, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_ACQ_CHANNELS_H_
#define APPLICATION_INCLUDE_ACQ_CHANNELS_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "ads1256.h"
#include "cdi/io.h"
#include "neon_eds.h"

/*===============================================================  MACRO's  ==*/

enum acq_x_sps
{
    ACQ_X_SPS_10,
    ACQ_X_SPS_1000
};

enum acq_x_mode
{
    ACQ_X_MODE_CONTINUOUS
};

/*============================================================  DATA TYPES  ==*/

struct acq_x_config
{
    enum acq_x_sps              sps;
    enum acq_x_mode             mode;
};

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

void acq_x_init(void);

void acq_x_reset(void);

nerror acq_x_set_config(const struct acq_x_config * config);

extern void acq_x_callback(const uint32_t * data[ACQUNITY_ACQ_CHANNELS]);

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of acq_channels.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_ACQ_CHANNELS_H_ */
