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

#include "acquisition.h"
#include "prim_gpio.h"
#include "protocol.h"
#include "rtcomm.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static void axis_x_drdy_isr_enable(void);
static void axis_x_drdy_isr_disable(void);
static void axis_x_nss_activate(void);
static void axis_x_nss_deactivate(void);
static void axis_x_reader(void *);

static void axis_y_drdy_isr_enable(void);
static void axis_y_drdy_isr_disable(void);
static void axis_y_nss_activate(void);
static void axis_y_nss_deactivate(void);
static void axis_y_reader(void *);

static void axis_z_drdy_isr_enable(void);
static void axis_z_drdy_isr_disable(void);
static void axis_z_nss_activate(void);
static void axis_z_nss_deactivate(void);
static void axis_z_reader(void *);

static void power_activate(void);
static void power_deactivate(void);

/*=======================================================  LOCAL VARIABLES  ==*/

static const struct ads1256_chip_vt g_probe_chip_vt[IO_PROBE_CHANNELS] =
{
    [IO_CHANNEL_X] = {
        .drdy_isr_enable 	= axis_x_drdy_isr_enable,
		.drdy_isr_disable 	= axis_x_drdy_isr_disable,
        .nss_activate 		= axis_x_nss_activate,
        .nss_deactivate 	= axis_x_nss_deactivate,
		.reader             = axis_x_reader
    },
    [IO_CHANNEL_Y] = {
		.drdy_isr_enable 	= axis_y_drdy_isr_enable,
		.drdy_isr_disable 	= axis_y_drdy_isr_disable,
        .nss_activate		= axis_y_nss_activate,
        .nss_deactivate		= axis_y_nss_deactivate,
		.reader             = axis_y_reader
    },
	[IO_CHANNEL_Z] = {
		.drdy_isr_enable 	= axis_z_drdy_isr_enable,
		.drdy_isr_disable 	= axis_z_drdy_isr_disable,
		.nss_activate		= axis_z_nss_activate,
		.nss_deactivate		= axis_z_nss_deactivate,
		.reader             = axis_z_reader
    }
};

static struct spi_bus * const 	g_probe_chip_spi_bus[IO_PROBE_CHANNELS] =
{
	[IO_CHANNEL_X] = &HWCON_PROBE_X_SPI,
	[IO_CHANNEL_Y] = &HWCON_PROBE_Y_SPI,
	[IO_CHANNEL_Z] = &HWCON_PROBE_Z_SPI,
};

static const struct ads1256_group_vt g_probe_group_vt =
{
	.power_activate = power_activate,
	.power_deactivate = power_deactivate,
};

/*======================================================  GLOBAL VARIABLES  ==*/

struct probe					g_probe;
struct aux						g_aux;
struct autorange				g_autorange;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

/*
 * Axis X methods
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
	NVIC_ClearPendingIRQ(HWCON_PROBE_X_DRDY_EXTI);
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

static void axis_x_reader(void * arg)
{
	(void)arg;
	/*
	 * TODO: Write something here
	 */
}

/*
 * Axis Y methods
 */

static void axis_y_drdy_isr_enable(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(HWCON_PROBE_Y_DRDY_PIN);
	NVIC_ClearPendingIRQ(HWCON_PROBE_Y_DRDY_EXTI);
    NVIC_EnableIRQ(HWCON_PROBE_Y_DRDY_EXTI);
}

static void axis_y_drdy_isr_disable(void)
{
	NVIC_ClearPendingIRQ(HWCON_PROBE_Y_DRDY_EXTI);
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

static void axis_y_reader(void * arg)
{
	(void)arg;
	/*
	 * TODO: Write something here
	 */
}

/*
 * Axis Z methods
 */

static void axis_z_drdy_isr_enable(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(HWCON_PROBE_Z_DRDY_PIN);
	NVIC_ClearPendingIRQ(HWCON_PROBE_Z_DRDY_EXTI);
    NVIC_EnableIRQ(HWCON_PROBE_Z_DRDY_EXTI);
}

static void axis_z_drdy_isr_disable(void)
{
	NVIC_ClearPendingIRQ(HWCON_PROBE_Z_DRDY_EXTI);
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

static void axis_z_reader(void * arg)
{
	(void)arg;
	/*
	 * TODO: Write something here
	 */
}

/*
 * Common probe methods
 */

static void power_activate(void)
{
	gpio_set(HWCON_PROBE_SYNC_PORT, HWCON_PROBE_SYNC_PIN);
}

static void power_deactivate(void)
{
	gpio_clr(HWCON_PROBE_SYNC_PORT, HWCON_PROBE_SYNC_PIN);
}

/*
 * Other helper functions
 */

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void acquisition_init(void)
{
	static struct io_buffer		buffer[2];

	ads1256_group_init(&g_probe.group, &g_probe_group_vt);
	ads1256_init_chip(&g_probe.chip[IO_CHANNEL_X],
			g_probe_chip_spi_bus[IO_CHANNEL_X],
			&g_probe_chip_vt[IO_CHANNEL_X]);
	ads1256_init_chip(&g_probe.chip[IO_CHANNEL_Y],
			g_probe_chip_spi_bus[IO_CHANNEL_Y],
			&g_probe_chip_vt[IO_CHANNEL_Y]);
	ads1256_init_chip(&g_probe.chip[IO_CHANNEL_Z],
			g_probe_chip_spi_bus[IO_CHANNEL_Z],
			&g_probe_chip_vt[IO_CHANNEL_Z]);
    rtcomm_init(&g_rtcomm, &buffer[0], &buffer[1], sizeof(buffer[0]));
}

int acquisition_probe_set_config(const struct io_ctrl_config * config)
{
	bool						channel_is_enabled[IO_PROBE_CHANNELS];
	uint32_t					master_channel = UINT32_MAX;
	uint32_t					index;
	int							retval;

	channel_is_enabled[IO_CHANNEL_X] = config->en_x == 1u ? true : false;
	channel_is_enabled[IO_CHANNEL_Y] = config->en_y == 1u ? true : false;
	channel_is_enabled[IO_CHANNEL_Z] = config->en_z == 1u ? true : false;

	g_probe.group_config.sampling_mode = ADS1256_SAMPLE_MODE_CONT;
	g_probe.group_config.sampling_rate = 1000u;
	ads1256_set_group_config(&g_probe.group, &g_probe.group_config);

	for (index = 0u; index < IO_PROBE_CHANNELS; index++) {
		if (channel_is_enabled[index]) {
			ads1256_group_add_chip(&g_probe.group, &g_probe.chip[index]);

			if (master_channel == UINT32_MAX) {
				master_channel = index;
			}
			g_probe.chip_config[index].enable_buffer =
					protocol_from_en_probe_buffer(config);
			g_probe.chip_config[index].enable_ext_osc = true;
			g_probe.chip_config[index].gpio = 0u;
			g_probe.chip_config[index].is_master =
					index == master_channel ? true : false;
			g_probe.chip_config[index].mux_hi =
					protocol_from_probe_mux_hi(config);
			g_probe.chip_config[index].mux_lo =
					protocol_from_probe_mux_lo(config);
			ads1256_set_per_chip_config(&g_probe.chip[index],
					&g_probe.chip_config[index]);
		}
	}
	retval = ads1256_apply_group_config(&g_probe.group);

	return (retval);
}

int acquisition_aux_set_config(const struct io_ctrl_config * config)
{
	(void)config;

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
	retval = ads1256_apply_group_config(&g_probe.group);

	return (retval);
}

int acquisition_aux_set_param(const struct io_ctrl_param * param)
{
	(void)param;

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

	retval = ads1256_start_sampling(&g_probe.group);

	return (retval);
}

void rtcomm_pre_send(void * buffer)
{
	/*
	 * TODO: Prepare buffer here just before sending.
	 *
	 * Here we can load some shared variables, counters, status etc.
	 */
	(void)buffer;
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of acquisition.c
 ******************************************************************************/
