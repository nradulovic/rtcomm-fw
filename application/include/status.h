/*
 *  rtcomm-fw - 2017
 *
 *  status.h (former app_stat.h)
 *
 *  Created on: May 26, 2015
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
 * @brief       Status interface
 *********************************************************************//** @{ */

#ifndef APPLICATION_INCLUDE_STATUS_H_
#define APPLICATION_INCLUDE_STATUS_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>

/*===============================================================  MACRO's  ==*/

enum status_msg
{
    STATUS_FATAL_UNHANDLED_EXCP,
    STATUS_FATAL_HW_INIT_FAILED,
    STATUS_RUNTIME_CHECK_FAILED,
    STATUS_CTRL_COMM_ERR,
    STATUS_CTRL_DATA_ERR,
    STATUS_ADS_ERR,
    STATUS_NO_RESOURCE_ERR,
    STATUS_RTCOMM_SKIPPED_ERR,
    STATUS_RTCOMM_TRANSFER_ERR,
    STATUS_RTCOMM_COMPLETE_ERR,
    _STATUS_LAST_ID
};

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

void status_warn(enum status_msg warn);

void status_error(enum status_msg error);

static inline
uint32_t status_get(enum status_msg stat)
{
    extern uint32_t             g_status_counters[];

    return (g_status_counters[stat]);
}

static inline
uint32_t status_get_total(void)
{
    uint32_t                    stat_id;
    uint32_t                    retval;

    for (stat_id = 0, retval =0; stat_id < _STATUS_LAST_ID; stat_id++) {
        retval += status_get(stat_id);
    }

    return (retval);
}
/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of app_stat.h
 ******************************************************************************/

#endif /* APPLICATION_INCLUDE_STATUS_H_ */
