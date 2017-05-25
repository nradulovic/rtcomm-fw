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
#include "cdi/io.h"

#if defined(HWCON_TEST_TIMER0_ENABLE)
#include "test_timer0.h"
#endif

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/

struct rtcs_wspace
{
    uint32_t                    frame;
    struct acq_buffer           buffer[2];
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static void deferred(void * arg);

static naction state_init(struct nsm *, const nevent *);
static naction state_run(struct nsm *, const nevent *);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct rtcs_wspace       g_rtcs_wspace;
static struct nevent *          g_rtcs_event_queue[EPA_RTCS_SERVER_QUEUE_SIZE];
static struct acq_buffer        g_storage[2];
static struct nsched_deferred   g_deferred;

/*======================================================  GLOBAL VARIABLES  ==*/

struct nepa_define              g_rtcs_epa_define = NEPA_DEF_INIT(
        &g_rtcs_wspace, state_init, NSM_TYPE_FSM, g_rtcs_event_queue,
        sizeof(g_rtcs_event_queue), "rt-c server", EPA_RTCOMM_PRIO);

struct nepa                     g_rtcs_epa;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static void deferred(void * arg)
{
    (void)arg;

    static const struct nevent rtcomm_push = NEVENT_INITIALIZER(RTCOMM_PUSH,
            NULL, 0);
    nepa_send_event_i(&g_rtcs_epa, &rtcomm_push);
}

#if defined(HWCON_TEST_TIMER0_ENABLE)
void test_timer0_callback(void)
{
    static uint32_t sample_idx;
    struct acq_buffer * buffer;

    buffer = rtcomm_request_new(&g_rtcomm);

    buffer->sample[0][ACQ_CHANNEL_X] = 1;
    buffer->sample[0][ACQ_CHANNEL_Y] = 2;
    buffer->sample[0][ACQ_CHANNEL_Z] = 3;
    sample_idx +=30;

    if (sample_idx == NARRAY_DIMENSION(g_storage[0].sample)) {
        sample_idx = 0;
        rtcomm_release_new(&g_rtcomm);
        nsched_deferred_do(&g_deferred);
    }
}
#endif


static naction state_init(struct nsm * sm, const nevent * event)
{
    struct rtcs_wspace *      ws = nsm_wspace(sm);

    switch (nevent_id(event)) {
        case NSM_INIT: {
            static const struct rtcomm_channel_data channels[] =
            {
                {
                    .elements = 3000,
                    .addr_offset = offsetof(struct acq_buffer, sample[0][0]),
                    .addr_mul = 12,
                    .bit_offset = 0,
                    .bit_mul = 0,
                    .mask = 24,
                    .content = 1,
                    .name = "x_data"
                }, {
                    .elements = 3000,
                    .addr_offset = offsetof(struct acq_buffer, sample[0][1]),
                    .addr_mul = 12,
                    .bit_offset = 0,
                    .bit_mul = 0,
                    .mask = 24,
                    .content = 1,
                    .name = "y_data"
                }, {
                    .elements = 3000,
                    .addr_offset = offsetof(struct acq_buffer, sample[0][2]),
                    .addr_mul = 12,
                    .bit_offset = 0,
                    .bit_mul = 0,
                    .mask = 24,
                    .content = 1,
                    .name = "z_data"
                }, {
                    .elements = 3000,
                    .addr_offset = offsetof(struct acq_buffer, sample[0][0]),
                    .addr_mul = 12,
                    .bit_offset = 24,
                    .bit_mul = 0,
                    .mask = 8,
                    .content = 1,
                    .name = "x_gain"
                }, {
                    .elements = 3000,
                    .addr_offset = offsetof(struct acq_buffer, sample[0][1]),
                    .addr_mul = 12,
                    .bit_offset = 24,
                    .bit_mul = 0,
                    .mask = 8,
                    .content = 1,
                    .name = "y_gain"
                }, {
                    .elements = 3000,
                    .addr_offset = offsetof(struct acq_buffer, sample[0][2]),
                    .addr_mul = 12,
                    .bit_offset = 24,
                    .bit_mul = 0,
                    .mask = 8,
                    .content = 1,
                    .name = "z_gain"
                }, {
                    .name = '\0'
                },
            };
            rtcomm_init(&g_rtcomm, &ws->buffer[0], &ws->buffer[1],
                    sizeof(ws->buffer[0]));
            nsched_deferred_init(&g_deferred, deferred, NULL);

            rtcomm_header_define_channel(&ws->buffer[0].header, channels);
            rtcomm_header_define_channel(&ws->buffer[1].header, channels);
#if defined(HWCON_TEST_TIMER0_ENABLE)
            test_timer0_enable();
#endif

            return (naction_transit_to(sm, state_run));
        }
        default: {
            return (naction_handled());
        }
    }
}

static naction state_run(struct nsm * sm, const nevent * event)
{
    struct rtcs_wspace *      ws = nsm_wspace(sm);

    switch (nevent_id(event)) {
        case RTCOMM_PUSH: {
            struct acq_buffer * buffer = rtcomm_peek(&g_rtcomm);

            if (buffer) {
                rtcomm_header_pack(&buffer->header, sizeof(struct acq_buffer),
                        1, ws->frame++, &g_rtcomm.counters);
            }
            rtcomm_emit(&g_rtcomm);

            return (naction_handled());
        }
        default:
            return (naction_handled());
    }
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of epa_rtcomm.c
 ******************************************************************************/
