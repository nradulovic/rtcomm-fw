/*
 *  teslameter_3mhx-fw - 2017
 *
 *  epa_controller.c
 *
 *  Created on: May 29, 2017
 * ----------------------------------------------------------------------------
 *  This file is part of teslameter_3mhx-fw.
 *
 *  teslameter_3mhx-fw is free software: you can redistribute it and/or modify
 *  it under the terms of the Lesser GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  teslameter_3mhx-fw is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with teslameter_3mhx-fw.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------- *//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Controller EPA
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "status.h"
#include "neon_eds.h"
#include "epa_ctrl.h"
#include "acquisition.h"
#include "epa_controller.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/

struct controller_wspace
{
	struct netimer				timeout;
};

enum controller_local_events
{
	SIG_ERR_BUSY = NEVENT_LOCAL_ID,
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static naction state_init(struct nsm *, const nevent *);
static naction state_init_ctrl(struct nsm *, const nevent *);
static naction state_uninitialized(struct nsm *, const nevent *);
static naction state_initialized(struct nsm *, const nevent *);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct controller_wspace g_controller_wspace;
static struct nevent *          g_controller_event_queue[
									 EPA_CONTROLLER_QUEUE_SIZE];

/*======================================================  GLOBAL VARIABLES  ==*/

struct nepa_define              g_controller_epa_define = NEPA_DEF_INIT(
        &g_controller_wspace, state_init, NSM_TYPE_FSM,
		g_controller_event_queue, sizeof(g_controller_event_queue),
		"controller", EPA_CONTROLLER_PRIO);

struct nepa                     g_controller_epa;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

static naction state_init(struct nsm * sm, const nevent * event)
{
    struct controller_wspace *  ws = nsm_wspace(sm);

    switch (nevent_id(event)) {
        case NSM_INIT: {
        	netimer_init(&ws->timeout);
        	acquisition_init();

        	return (naction_transit_to(sm, state_init_ctrl));
        }
        default: {
            return (naction_handled());
        }
    }
}

static naction state_init_ctrl(struct nsm * sm, const nevent * event)
{
	(void)sm;

	switch (nevent_id(event)) {
		case NSM_ENTRY: {
			nepa_send_signal(&g_ctrl_epa, CONTROLLER_SIG_INIT);

			return (naction_handled());
		}
		case CONTROLLER_SIG_INIT_DONE: {
			return (naction_transit_to(sm, state_uninitialized));
		}
		default: {
			return (naction_handled());
		}
	}
}

static naction state_uninitialized(struct nsm * sm, const nevent * event)
{
	(void)sm;

	switch (nevent_id(event)) {
		case NSM_ENTRY: {

			return (naction_handled());
		}
		case CTRL_EVT_CONFIG: {
			int					retval;
			const struct ctrl_evt_config *
								evt_config = nevent_data(event);

			retval = acquisition_set_config(&evt_config->config);

			if (retval) {
				status_warn(STATUS_RUNTIME_CHECK_FAILED);

				return (naction_handled());
			}

			return (naction_transit_to(sm, state_initialized));
		}
		default: {
			return (naction_handled());
		}
	}
}

static naction state_initialized(struct nsm * sm, const nevent * event)
{
	(void)sm;

	switch (nevent_id(event)) {
		case NSM_ENTRY: {
			return (naction_handled());
		}
		case CTRL_EVT_PARAM: {
			int					retval;
			const struct ctrl_evt_param *
								evt_param = nevent_data(event);

			retval = acquisition_probe_set_param(&evt_param->param);

			if (retval) {
				status_warn(STATUS_RUNTIME_CHECK_FAILED);

				return (naction_transit_to(sm, state_initialized));
			}
			retval = acquisition_aux_set_param(&evt_param->param);

			if (retval) {
				status_warn(STATUS_RUNTIME_CHECK_FAILED);

				return (naction_transit_to(sm, state_initialized));
			}
			retval = acquisition_autorange_set_param(&evt_param->param);

			if (retval) {
				status_warn(STATUS_RUNTIME_CHECK_FAILED);
				return (naction_transit_to(sm, state_initialized));
			}
			acquisition_start_sampling();

			return (naction_transit_to(sm, state_initialized));
		}
		default: {
			return (naction_handled());
		}
	}
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of epa_controller.c
 ******************************************************************************/
