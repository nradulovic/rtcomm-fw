/*
 *  teslameter_3mhx-fw - 2017
 *
 *  epa_ctrl.c
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
 * @brief       Control bus implementation
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include <string.h>

#include "psm.h"
#include "cdi/io.h"
#include "status.h"
#include "epa_ctrl.h"
#include "neon_eds.h"
#include "epa_controller.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/

struct ctrl_wspace
{
    struct netimer              timeout;
    union ctrl_data_buff {
        struct io_ctrl_config       config;
        struct io_ctrl_param        param;
    }                           buff;
};

enum ctrl_local_events
{
    SIG_ERR_BUSY = NEVENT_LOCAL_ID,
    SIG_RETRY,
    SIG_XFER_COMPLETE
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static naction state_init(struct nsm *, const nevent *);
static naction state_wait_config(struct nsm *, const nevent *);
static naction state_wait_param(struct nsm *, const nevent *);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct ctrl_wspace       g_ctrl_wspace;
static struct nevent *          g_ctrl_event_queue[EPA_CTRL_QUEUE_SIZE];

/*======================================================  GLOBAL VARIABLES  ==*/

struct nepa_define              g_ctrl_epa_define = NEPA_DEF_INIT(
        &g_ctrl_wspace, state_init, NSM_TYPE_FSM, g_ctrl_event_queue,
        sizeof(g_ctrl_event_queue), "ctrl", EPA_CTRL_PRIO);

struct nepa                     g_ctrl_epa;
struct ctrl                     g_ctrl;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

/* -- Helper function ------------------------------------------------------- */

static void ctrl_rx_complete(struct ctrl * ctrl, void * buffer, uint16_t size)
{
    static const struct nevent sig_xfer_complete = NEVENT_INITIALIZER(
            SIG_XFER_COMPLETE, NULL, 0);
    nerror                      error;

    (void)ctrl;
    (void)buffer;
    (void)size;

    error = nepa_send_event_i(&g_ctrl_epa, &sig_xfer_complete);

    if (error) {
        status_warn(STATUS_NO_RESOURCE_ERR);
    }
}

static void ctrl_tx_complete(struct ctrl * ctrl, void * buffer, uint16_t size)
{
    (void)ctrl;
    (void)buffer;
    (void)size;
}

static void ctrl_error(struct ctrl * ctrl, uint32_t error)
{
    static const struct nevent sig_err_busy = NEVENT_INITIALIZER(SIG_ERR_BUSY,
            NULL, 0);
    nerror                  neon_error;
    const struct nevent *   event;

    (void)ctrl;

    if (error == 0) {
        event = &sig_err_busy;
    } else {
        event = &sig_err_busy;
    }
    /*
     * Usually this functions is called from ISR. In that case we call _i
     * methods. The only exception is when this functions is called with
     * error argument equal to zero. This means that it is called from loop
     * code and we must call non _i methods.
     */
    if (ncore_is_lock_valid()) {
        neon_error = nepa_send_event_i(&g_ctrl_epa, event);
    } else {
        neon_error = nepa_send_event(&g_ctrl_epa, event);
    }

    if (neon_error) {
        status_warn(STATUS_NO_RESOURCE_ERR);
    }
    status_warn(STATUS_CTRL_COMM_ERR);
}

static void ctrl_start_rx(struct ctrl * ctrl, void * buffer, uint16_t size)
{
    HAL_StatusTypeDef           status = HAL_BUSY;

    if (HAL_I2C_GetState(&ctrl->i2c) == HAL_I2C_STATE_READY) {
        ctrl->buffer = buffer;
        ctrl->size = size;
        status = HAL_I2C_Slave_Receive_IT(&ctrl->i2c, (uint8_t *)buffer, size);
    }

    if (status != HAL_OK) {
        ctrl_error(ctrl, 0u);
    }
}

static void ctrl_reinit(struct ctrl * ctrl)
{
    /* NOTE:
     * This is one dirty solution. If I2C is not ready, just turn-off all IRQ
     * and set state variable to ready, the function HAL_I2C_Slave_Receive_IT()
     * will handle the rest of stuff.
     */
    if (HAL_I2C_GetState(&ctrl->i2c) != HAL_I2C_STATE_READY) {
        psm_reinit_i2c_ctrl();
    }
}

/* - State machine states --------------------------------------------------- */

static naction state_init(struct nsm * sm, const nevent * event)
{
    struct ctrl_wspace *        ws = nsm_wspace(sm);

    switch (nevent_id(event)) {
        case NSM_INIT: {
            return (naction_handled());
        }
        case CONTROLLER_SIG_INIT: {
            netimer_init(&ws->timeout);
            nepa_send_signal(&g_controller_epa, CONTROLLER_SIG_INIT_DONE);

            {
                nepa_send_signal(&g_ctrl_epa, SIG_XFER_COMPLETE);

                ctrl_header_pack(&ws->buff.config.header, sizeof(ws->buff.config));
                ws->buff.config.en_x = 1;
                ws->buff.config.en_y = 1;
                ws->buff.config.en_z = 1;
                ws->buff.config.en_probe_buffer = 0;
                ws->buff.config.probe_mux_hi = 3;
                ws->buff.config.probe_mux_lo = 2;
                ws->buff.config.aux1_mux_hi = 2;
                ws->buff.config.aux1_mux_lo = 3;
                ws->buff.config.aux2_mux_hi = 4;
                ws->buff.config.aux2_mux_lo = 5;
                ws->buff.config.en_aux1 = 1;
                ws->buff.config.en_aux2 = 1;
                ws->buff.config.en_aux_buffer = 0;
                ws->buff.config.probe_pga = 1;
            }
            return (naction_transit_to(sm, state_wait_config));
        }
        default: {
            return (naction_handled());
        }
    }
}

static naction state_wait_config(struct nsm * sm, const nevent * event)
{
    struct ctrl_wspace *        ws = nsm_wspace(sm);

    switch (nevent_id(event)) {
        case NSM_ENTRY: {
            ctrl_reinit(&g_ctrl);
            ctrl_start_rx(&g_ctrl, &ws->buff.config, sizeof(ws->buff.config));

            return (naction_handled());
        }
        case NSM_EXIT: {
            netimer_cancel(&ws->timeout);

            return (naction_handled());
        }
        case SIG_XFER_COMPLETE: {
            struct ctrl_evt_config * config;
            uint32_t            dummy;
            bool                is_valid;
            nerror              neon_error = !NERROR_NONE;

            is_valid = ctrl_header_unpack(&ws->buff.config.header, &dummy);

            if (is_valid == false) {
                status_warn(STATUS_CTRL_DATA_ERR);

                return (naction_transit_to(sm, state_wait_config));
            }
            config = NEVENT_CREATE(struct ctrl_evt_config, CTRL_EVT_CONFIG);

            if (config) {
                memcpy(&config->config, &ws->buff.config,
                        sizeof(config->config));
                neon_error = nepa_send_event(&g_controller_epa, &config->super);
            }

            if (neon_error) {
                status_warn(STATUS_NO_RESOURCE_ERR);

                return (naction_transit_to(sm, state_wait_config));
            } else {
                {
                    nepa_send_signal(&g_ctrl_epa, SIG_XFER_COMPLETE);

                    ctrl_header_pack(&ws->buff.config.header, sizeof(ws->buff.param));
                    ws->buff.param.data.en_autorange = 0;
                    ws->buff.param.data.probe_gain = 0;
                    ws->buff.param.data.vspeed = 1;
                    ws->buff.param.data.workmode = IO_WORKMODE_NORMAL;
                }
                return (naction_transit_to(sm, state_wait_param));
            }
        }
        case SIG_ERR_BUSY: {
            netimer_after(&ws->timeout, NTIMER_MS(10), SIG_RETRY);

            return (naction_handled());
        }
        case SIG_RETRY: {
            return (naction_transit_to(sm, state_wait_config));
        }
        default: {
            return (naction_handled());
        }
    }
}

static naction state_wait_param(struct nsm * sm, const nevent * event)
{
    struct ctrl_wspace *        ws = nsm_wspace(sm);

    switch (nevent_id(event)) {
        case NSM_ENTRY: {
            ctrl_reinit(&g_ctrl);
            ctrl_start_rx(&g_ctrl, &ws->buff.param, sizeof(ws->buff.param));

            return (naction_handled());
        }
        case NSM_EXIT: {
            netimer_cancel(&ws->timeout);

            return (naction_handled());
        }
        case SIG_XFER_COMPLETE: {
            struct ctrl_evt_param * param;
            bool                    is_valid;
            uint32_t                dummy;
            nerror                  neon_error = !NERROR_NONE;

            is_valid = ctrl_header_unpack(&ws->buff.config.header, &dummy);

            if (is_valid == false) {
                status_warn(STATUS_CTRL_DATA_ERR);

                return (naction_transit_to(sm, state_wait_param));
            }
            param = NEVENT_CREATE(struct ctrl_evt_param, CTRL_EVT_PARAM);

            if (param) {
                memcpy(&param->param, &ws->buff.param, sizeof(param->param));
                neon_error = nepa_send_event(&g_controller_epa, &param->super);
            }

            if (neon_error) {
                status_warn(STATUS_NO_RESOURCE_ERR);
            }

            return (naction_transit_to(sm, state_wait_param));
        }
        case SIG_ERR_BUSY: {
            netimer_after(&ws->timeout, NTIMER_MS(10), SIG_RETRY);

            return (naction_handled());
        }
        case SIG_RETRY: {
            return (naction_transit_to(sm, state_wait_param));
        }
        default: {
            return (naction_handled());
        }
    }
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void ctrl_isr_rx_complete(struct ctrl * ctrl)
{
    ctrl_rx_complete(ctrl, ctrl->buffer, ctrl->size);
}

void ctrl_isr_tx_complete(struct ctrl * ctrl)
{
    ctrl_tx_complete(ctrl, ctrl->buffer, ctrl->size);
}

void ctrl_isr_error(struct ctrl * ctrl)
{
    ctrl_error(ctrl, HAL_I2C_GetError(&ctrl->i2c));
}
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of epa_ctrl.c
 ******************************************************************************/
