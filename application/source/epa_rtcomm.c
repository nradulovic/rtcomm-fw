/*
 * This file is part of teslameter_3mhx-fw.
 *
 * Copyright (C) 2010 - 2017 nenad
 *
 * teslameter_3mhx-fw is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * teslameter_3mhx-fw is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with teslameter_3mhx-fw.  If not, see <http://www.gnu.org/licenses/>.
 *
 * web site:    
 * e-mail  :    
 *//***********************************************************************//**
 * @file
 * @author      nenad
 * @brief       Brief description
 *********************************************************************//** @{ */
/**@defgroup    def_impl Implementation
 * @brief       Default Implementation
 * @{ *//*--------------------------------------------------------------------*/

/*=========================================================  INCLUDE FILES  ==*/

#include "neon_eds.h"
#include "epa_rtcomm.h"
#include "rtcomm.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/

struct rtcomm_wspace
{
    uint32_t                    dummy;
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static naction state_init(struct nsm *, const nevent *);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct rtcomm_wspace     g_rtcomm_wspace;
static struct nevent *          g_rtcomm_event_queue[EPA_RTCOMM_QUEUE_SIZE];

/*======================================================  GLOBAL VARIABLES  ==*/

struct nepa_define              g_rtcomm_epa_define = NEPA_DEF_INIT(
        &g_rtcomm_wspace, state_init, NSM_TYPE_FSM, g_rtcomm_event_queue,
        sizeof(g_rtcomm_event_queue), "rtcomm", EPA_RTCOMM_PRIO);

struct nepa                     g_rtcomm_epa;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static naction state_init(struct nsm * sm, const nevent * event)
{
    switch (nevent_id(event)) {
        case RTCOMM_PUSH:
            rtcomm_push(&g_rtcomm);
            return (naction_handled());
        default:
            return (naction_handled());
    }
}
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of epa_rtcomm.c
 ******************************************************************************/
