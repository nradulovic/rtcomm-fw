/*
 * probe_channels.c
 *
 *  Created on: May 25, 2016
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Probe channels code
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "probe_channels.h"
#include "ads1256.h"
#include "prim_gpio.h"
#include "cdi/io.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

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

static void power_activate(void);
static void power_deactivate(void);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct ads1256_chip g_probe_chip[IO_PROBE_CHANNELS];

static const struct ads1256_chip_vt g_probe_chip_vt[IO_PROBE_CHANNELS] =
{
    [IO_CHANNEL_X] = {
        .drdy_isr_enable 	= axis_x_drdy_isr_enable,
		.drdy_isr_disable 	= axis_x_drdy_isr_disable,
        .nss_activate 		= axis_x_nss_activate,
        .nss_deactivate 	= axis_x_nss_deactivate,
		.reader             = probe_axis_x_reader
    },
    [IO_CHANNEL_Y] = {
		.drdy_isr_enable 	= axis_y_drdy_isr_enable,
		.drdy_isr_disable 	= axis_y_drdy_isr_disable,
        .nss_activate		= axis_y_nss_activate,
        .nss_deactivate		= axis_y_nss_deactivate,
		.reader             = probe_axis_y_reader
    },
	[IO_CHANNEL_Z] = {
		.drdy_isr_enable 	= axis_z_drdy_isr_enable,
		.drdy_isr_disable 	= axis_z_drdy_isr_disable,
		.nss_activate		= axis_z_nss_activate,
		.nss_deactivate		= axis_z_nss_deactivate,
		.reader             = probe_axis_z_reader
    }
};

static struct spi_bus * const g_probe_chip_spi_bus[IO_PROBE_CHANNELS] =
{
	[IO_CHANNEL_X] = &HWCON_PROBE_X_SPI,
	[IO_CHANNEL_Y] = &HWCON_PROBE_Y_SPI,
	[IO_CHANNEL_Z] = &HWCON_PROBE_Z_SPI,
};

static struct ads1256_group_vt g_probe_group_vt =
{
	.power_activate = power_activate,
	.power_deactivate = power_deactivate,
};

/*======================================================  GLOBAL VARIABLES  ==*/

struct ads1256_group g_probe_group;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

/* -------------------------------------------------------------------------- *
 * Axis X methods
 * -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- *
 * Axis Y methods
 * -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- *
 * Axis Z methods
 * -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- *
 * Common probe methods
 * -------------------------------------------------------------------------- */

static void power_activate(void)
{
	gpio_set(HWCON_PROBE_SYNC_PORT, HWCON_PROBE_SYNC_PIN);
}

static void power_deactivate(void)
{
	gpio_clr(HWCON_PROBE_SYNC_PORT, HWCON_PROBE_SYNC_PIN);
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void probe_init(void)
{
    uint32_t                    index;

    ads1256_group_init(&g_probe_group, &g_probe_group_vt);

    for (index = 0; index < IO_PROBE_CHANNELS; index++) {
        ads1256_init_chip(&g_probe_chip[index], g_probe_chip_spi_bus[index],
                &g_probe_chip_vt[index]);
        ads1256_group_add_chip(&g_probe_group, &g_probe_chip[index]);
    }
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of probe_channels.c
 ******************************************************************************/
