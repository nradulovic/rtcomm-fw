/*
 *  teslameter_3mhx-fw - 2017
 *
 *  acquisition.c
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
 * @brief       Acquisition
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include <string.h>

#include "acquisition.h"
#include "prim_gpio.h"
#include "protocol.h"
#include "prim_spi.h"
#include "status.h"
#include "rtcomm.h"
#include "cdi/io.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/

struct acquisition
{
	struct io_ctrl_config		config;
	struct io_ctrl_param 		param;
	uint32_t					buffer_size;
	uint32_t					current_sample;
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

/* -- Probe callbacks for ADS1256 Module -- */
static void axis_x_drdy_isr_enable(void);
static void axis_x_drdy_isr_disable(void);
static void axis_x_nss_activate(void);
static void axis_x_nss_deactivate(void);

static void axis_y_drdy_isr_enable(void);
static void axis_y_drdy_isr_disable(void);
static void axis_y_nss_activate(void);
static void axis_y_nss_deactivate(void);

static void axis_z_drdy_isr_enable(void);
static void axis_z_drdy_isr_disable(void);
static void axis_z_nss_activate(void);
static void axis_z_nss_deactivate(void);

static void probe_power_activate(void);
static void probe_power_deactivate(void);
static void probe_sample_finished(const struct ads1256_group * group);

/* -- AUX ADC callbacks for ADS1256 Module -- */
static void aux_drdy_isr_enable(void);
static void aux_drdy_isr_disable(void);
static void aux_nss_activate(void);
static void aux_nss_deactivate(void);

static void aux_sample_finished(const struct ads1256_group * group);

/*=======================================================  LOCAL VARIABLES  ==*/

/* -- Probe pointers to callbacks and info structs -- */
static const struct ads1256_chip_vt
								g_probe_chip_vt[IO_PROBE_CHANNELS] =
{
    [IO_CHANNEL_X] = {
        .drdy_isr_enable 	= axis_x_drdy_isr_enable,
		.drdy_isr_disable 	= axis_x_drdy_isr_disable,
        .nss_activate 		= axis_x_nss_activate,
        .nss_deactivate 	= axis_x_nss_deactivate,
    },
    [IO_CHANNEL_Y] = {
		.drdy_isr_enable 	= axis_y_drdy_isr_enable,
		.drdy_isr_disable 	= axis_y_drdy_isr_disable,
        .nss_activate		= axis_y_nss_activate,
        .nss_deactivate		= axis_y_nss_deactivate,
    },
	[IO_CHANNEL_Z] = {
		.drdy_isr_enable 	= axis_z_drdy_isr_enable,
		.drdy_isr_disable 	= axis_z_drdy_isr_disable,
		.nss_activate		= axis_z_nss_activate,
		.nss_deactivate		= axis_z_nss_deactivate,
    }
};

static struct spi_bus * const 	g_probe_chip_spi_bus[IO_PROBE_CHANNELS] =
{
	[IO_CHANNEL_X] = &HWCON_PROBE_X_SPI,
	[IO_CHANNEL_Y] = &HWCON_PROBE_Y_SPI,
	[IO_CHANNEL_Z] = &HWCON_PROBE_Z_SPI,
};

static const struct ads1256_group_vt
								g_probe_group_vt =
{
	.power_activate 	= probe_power_activate,
	.power_deactivate 	= probe_power_deactivate,
	.sample_finished 	= probe_sample_finished
};

/* -- AUX pointers to callbacks and info structs -- */

static const struct ads1256_chip_vt
								g_aux_chip_vt =
{
	.drdy_isr_enable 	= aux_drdy_isr_enable,
	.drdy_isr_disable	= aux_drdy_isr_disable,
	.nss_activate		= aux_nss_activate,
	.nss_deactivate		= aux_nss_deactivate
};

static struct spi_bus * const 	g_aux_chip_spi_bus = &HWCON_AUX_SPI;

static const struct ads1256_group_vt
								g_aux_group_vt =
{
	.power_activate 	= NULL, /* NOTE: The group doesn't have power control */
	.power_deactivate   = NULL, /* NOTE: The group doesn't have power control */
	.sample_finished    = aux_sample_finished
};

/*======================================================  GLOBAL VARIABLES  ==*/

struct probe					g_probe;
struct aux						g_aux;
struct autorange				g_autorange;
struct acquisition 				g_acquisition;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

/*
 * Probe axis X methods
 */

static void axis_x_drdy_isr_enable(void)
{
	/* Clear EXTI controller interrupt bit, because this is holding interrupt.
	 */
	__HAL_GPIO_EXTI_CLEAR_IT(HWCON_PROBE_X_DRDY_PIN);
    NVIC_ClearPendingIRQ(HWCON_PROBE_X_DRDY_EXTI);
    NVIC_EnableIRQ(HWCON_PROBE_X_DRDY_EXTI);
}

static void axis_x_drdy_isr_disable(void)
{
    NVIC_DisableIRQ(HWCON_PROBE_X_DRDY_EXTI);
}

static void axis_x_nss_activate(void)
{
    gpio_clr(HWCON_PROBE_X_NSS_PORT, HWCON_PROBE_X_NSS_PIN);
}

static void axis_x_nss_deactivate(void)
{
    gpio_set(HWCON_PROBE_X_NSS_PORT, HWCON_PROBE_X_NSS_PIN);
}

/*
 * Probe axis Y methods
 */

static void axis_y_drdy_isr_enable(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(HWCON_PROBE_Y_DRDY_PIN);
	NVIC_ClearPendingIRQ(HWCON_PROBE_Y_DRDY_EXTI);
    NVIC_EnableIRQ(HWCON_PROBE_Y_DRDY_EXTI);
}

static void axis_y_drdy_isr_disable(void)
{
    NVIC_DisableIRQ(HWCON_PROBE_Y_DRDY_EXTI);
}

static void axis_y_nss_activate(void)
{
    gpio_clr(HWCON_PROBE_Y_NSS_PORT, HWCON_PROBE_Y_NSS_PIN);
}

static void axis_y_nss_deactivate(void)
{
    gpio_set(HWCON_PROBE_Y_NSS_PORT, HWCON_PROBE_Y_NSS_PIN);
}

/*
 * Probe axis Z methods
 */

static void axis_z_drdy_isr_enable(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(HWCON_PROBE_Z_DRDY_PIN);
	NVIC_ClearPendingIRQ(HWCON_PROBE_Z_DRDY_EXTI);
    NVIC_EnableIRQ(HWCON_PROBE_Z_DRDY_EXTI);
}

static void axis_z_drdy_isr_disable(void)
{
    NVIC_DisableIRQ(HWCON_PROBE_Z_DRDY_EXTI);
}

static void axis_z_nss_activate(void)
{
    gpio_clr(HWCON_PROBE_Z_NSS_PORT, HWCON_PROBE_Z_NSS_PIN);
}

static void axis_z_nss_deactivate(void)
{
    gpio_set(HWCON_PROBE_Z_NSS_PORT, HWCON_PROBE_Z_NSS_PIN);
}

/*
 * Probe common methods
 */

static void probe_power_activate(void)
{
	gpio_set(HWCON_PROBE_SYNC_PORT, HWCON_PROBE_SYNC_PIN);
}

static void probe_power_deactivate(void)
{
	gpio_clr(HWCON_PROBE_SYNC_PORT, HWCON_PROBE_SYNC_PIN);
}

static void probe_sample_finished(const struct ads1256_group * group)
{
	struct ads1256_chip *		chip;

	struct io_buffer * buffer = rtcomm_request_new(&g_rtcomm);

	for (chip = group->chips; chip != NULL; chip = chip->next) {
		buffer->sample[g_acquisition.current_sample][chip->id] =
				ads1256_get_value(chip);
	}
	g_acquisition.current_sample++;

	if (g_acquisition.current_sample == g_acquisition.buffer_size) {
		g_acquisition.current_sample = 0u;
		rtcomm_release_new(&g_rtcomm);
	}
}

/*
 * Aux ADC methods
 */
static void aux_drdy_isr_enable(void)
{
	/* Clear EXTI controller interrupt bit, because this is holding interrupt.
	 */
	__HAL_GPIO_EXTI_CLEAR_IT(HWCON_AUX_DRDY_PIN);
    NVIC_ClearPendingIRQ(HWCON_AUX_DRDY_EXTI);
    NVIC_EnableIRQ(HWCON_AUX_DRDY_EXTI);
}

static void aux_drdy_isr_disable(void)
{
	NVIC_DisableIRQ(HWCON_AUX_DRDY_EXTI);
}

static void aux_nss_activate(void)
{
	gpio_clr(HWCON_AUX_NSS_PORT, HWCON_AUX_NSS_PIN);
}

static void aux_nss_deactivate(void)
{
	gpio_set(HWCON_AUX_NSS_PORT, HWCON_AUX_NSS_PIN);
}

static void aux_sample_finished(const struct ads1256_group * group)
{
	(void)group;
	/*
	 * TODO: Fill this with appropriate code
	 */
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void acquisition_init(void)
{
	/* NOTE:
	 * Allocate the io_buffer. We need two of the because the FW works as
	 * ping-pong buffer.
	 */
	static struct io_buffer		buffer[2];
	uint32_t					channel_id;

	/* -- Probe chips initialization -- */
	ads1256_group_init(&g_probe.group, &g_probe_group_vt);
	ads1256_set_group_config(&g_probe.group, &g_probe.group_config);

	for (channel_id = 0u; channel_id < IO_PROBE_CHANNELS; channel_id++) {
		ads1256_init_chip(&g_probe.chip[channel_id],
				g_probe_chip_spi_bus[channel_id],
				&g_probe_chip_vt[channel_id], channel_id);
		ads1256_set_per_chip_config(&g_probe.chip[channel_id],
				&g_probe.chip_config[channel_id]);
	}

	/* -- AUX chip initialization -- */
	ads1256_group_init(&g_aux.group, &g_aux_group_vt);
	ads1256_set_group_config(&g_aux.group, &g_aux.group_config);
	ads1256_init_chip(&g_aux.chip, g_aux_chip_spi_bus, &g_aux_chip_vt, 0);
	ads1256_set_per_chip_config(&g_aux.chip, &g_aux.chip_config);

	/* -- CDI rtcomm initialization -- */
    rtcomm_init(&g_rtcomm, &buffer[0], &buffer[1], sizeof(buffer[0]));
}

int acquisition_probe_set_config(const struct io_ctrl_config * config)
{
	bool						channel_is_enabled[IO_PROBE_CHANNELS];
	uint32_t					master_channel = UINT32_MAX;
	uint32_t					channel_id;

	channel_is_enabled[IO_CHANNEL_X] = config->en_x == 1u ? true : false;
	channel_is_enabled[IO_CHANNEL_Y] = config->en_y == 1u ? true : false;
	channel_is_enabled[IO_CHANNEL_Z] = config->en_z == 1u ? true : false;

	for (channel_id = 0u; channel_id < IO_PROBE_CHANNELS; channel_id++) {
		if (channel_is_enabled[channel_id]) {
			ads1256_group_add_chip(&g_probe.group, &g_probe.chip[channel_id]);

			if (master_channel == UINT32_MAX) {
				master_channel = channel_id;
			}
			g_probe.chip_config[channel_id].enable_buffer =
					protocol_from_en_probe_buffer(config);
			g_probe.chip_config[channel_id].enable_ext_osc = true;
			g_probe.chip_config[channel_id].gpio = 0u;
			g_probe.chip_config[channel_id].is_master =
					channel_id == master_channel ? true : false;
			g_probe.chip_config[channel_id].mux_hi =
					protocol_from_probe_mux_hi(config);
			g_probe.chip_config[channel_id].mux_lo =
					protocol_from_probe_mux_lo(config);
		}
	}
	memcpy(&g_acquisition.config, config, sizeof(g_acquisition.config));

	return (0);
}

int acquisition_aux_set_config(const struct io_ctrl_config * config)
{
	g_aux.chip_config.enable_buffer = protocol_from_en_aux_bufer(config);
	g_aux.chip_config.enable_ext_osc = true;
	g_aux.chip_config.gpio = 0u;
	g_aux.chip_config.is_master = true;
	g_aux.group_config.sampling_mode = ADS1256_SAMPLE_MODE_REQ;
	g_aux.group_config.sampling_rate = ADS1256_SAMPLE_RATE_2_5;

	return (0);
}

int acquisition_autorange_set_config(const struct io_ctrl_config * config)
{
	(void)config;

	return (0);
}

int acquisition_probe_set_param(const struct io_ctrl_param * param)
{
	int						retval;

	g_probe.group_config.sampling_mode =
			protocol_from_workmode(param);
	g_probe.group_config.sampling_rate =
			protocol_from_vspeed(param);

	/* Finally apply all settings */
	retval = ads1256_apply_group_config(&g_probe.group);

	if (retval) {
		return (retval);
	}

	/* Set buffer size ten times smaller then sample rate. We want 10 buffers
	 * per second which are sent from firmware to application processor.
	 */
	g_acquisition.buffer_size = protocol_from_vspeed(param) / 10u;

	/* Clear main buffers before using them. This is good when watching SPI
	 * signals using logic analyzer.
	 */
	rtcomm_clear(&g_rtcomm);

	/* Make a copy of params because we will later use this info to create
	 * io_buffer.
	 */
	memcpy(&g_acquisition.param, param, sizeof(g_acquisition.param));

	return (retval);
}

int acquisition_aux_set_param(const struct io_ctrl_param * param)
{
	(void)param;
	/* NOTE:
	 * AUX does not have any parameters.
	 */

	return (0);
}

int acquisition_autorange_set_param(const struct io_ctrl_param * param)
{
	g_autorange.is_enabled = protocol_from_en_autorange(param);

	return (0);
}

void acquisition_stop_sampling(void)
{
	ads1256_stop_sampling(&g_probe.group);
}

int acquisition_start_sampling(void)
{
	int							retval;

	g_acquisition.current_sample = 0u;

	retval = ads1256_start_sampling(&g_probe.group);

	return (retval);
}

void rtcomm_pre_send(void * buffer)
{
	struct io_buffer *		io_buffer = buffer;
	static uint32_t 		frame_count;

	/* Update header and footer */
	rtcomm_header_pack(&io_buffer->header, sizeof(struct io_buffer),
			frame_count++);
	rtcomm_footer_pack(&io_buffer->footer, &io_buffer->header);

	/* Get status */
	io_buffer->stats.runtime_check_failed =
			status_get(STATUS_RUNTIME_CHECK_FAILED);
	io_buffer->stats.total_errors =
			status_get_total();
	io_buffer->stats.ctrl_comm_err =
			status_get(STATUS_CTRL_COMM_ERR);
	io_buffer->stats.ctrl_data_err =
			status_get(STATUS_CTRL_DATA_ERR);
	io_buffer->stats.ads_err =
			status_get(STATUS_ADS_ERR);
	io_buffer->stats.no_resource_err =
			status_get(STATUS_NO_RESOURCE_ERR);
	io_buffer->stats.rtcomm_skipped_err =
			status_get(STATUS_RTCOMM_SKIPPED_ERR);
	io_buffer->stats.rtcomm_transfer_err =
			status_get(STATUS_RTCOMM_TRANSFER_ERR);
	io_buffer->stats.rtcomm_complete_err =
			status_get(STATUS_RTCOMM_COMPLETE_ERR);

	/* Return parameters that were used for sampling */
	memcpy(&io_buffer->param, &g_acquisition.param.data,
			sizeof(io_buffer->param));
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of acquisition.c
 ******************************************************************************/
