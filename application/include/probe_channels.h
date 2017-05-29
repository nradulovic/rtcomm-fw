/*
 * acq_channels.h
 *
 *  Created on: May 26, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_PROBE_CHANNELS_H_
#define APPLICATION_INCLUDE_PROBE_CHANNELS_H_

/*=========================================================  INCLUDE FILES  ==*/
/*===============================================================  MACRO's  ==*/
/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

void probe_init(void);

extern void probe_axis_x_reader(void *);
extern void probe_axis_y_reader(void *);
extern void probe_axis_z_reader(void *);

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of acq_channels.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_PROBE_CHANNELS_H_ */
