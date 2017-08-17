/*
 *  rtcomm-fw - 2017
 *
 *  psm.h
 *
 *  Created on: Jun 5, 2017
 * ----------------------------------------------------------------------------
 *  This file is part of rtcomm-fw.
 *
 *  rtcomm-fw is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  rtcomm-fw is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with rtcomm-fw.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------- *//**
 * @file
 * @author      Nenad Radulovic
 * @brief       PSM (Peripheral Support Module)
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_PSM_H_
#define APPLICATION_INCLUDE_PSM_H_

/*=========================================================  INCLUDE FILES  ==*/
/*===============================================================  MACRO's  ==*/
/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

/* -- Initialization functions -- */

void psm_init_clock(void);
void psm_init_gpio(void);
void psm_init_exti(void);
void psm_init_spi(void);
void psm_init_i2c(void);
void psm_init_timer(void);

/* -- Re-initialization functions -- */

void psm_reinit_i2c_ctrl(void);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of psm.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_PSM_H_ */
