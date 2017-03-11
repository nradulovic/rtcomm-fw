/*
 * acq_channels.c
 *
 *  Created on: May 25, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Acquisition channel code
 *********************************************************************//** @{ */



/*=========================================================  INCLUDE FILES  ==*/

#include <stddef.h>

#include "acq_channels.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define gpio_set(port, pin)             (port)->BSRR = (pin);

#define gpio_clr(port, pin)             (port)->BSRR = (uint32_t)(pin) << 16u;

#define gpio_read(port, pin)            ((port)->IDR & pin)

/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static void acq_0_drdy_isr_enable(void);
static void acq_0_drdy_isr_disable(void);
static bool acq_0_drdy_is_active(void);
static void acq_0_cs_enable(void);
static void acq_0_cs_disable(void);

static void acq_1_drdy_isr_enable_high(void);
static void acq_1_drdy_isr_disable(void);
static bool acq_1_drdy_is_active(void);
static void acq_1_cs_enable(void);
static void acq_1_cs_disable(void);

static void acq_2_drdy_isr_enable_high(void);
static void acq_2_drdy_isr_disable(void);
static bool acq_2_drdy_is_active(void);
static void acq_2_cs_enable(void);
static void acq_2_cs_disable(void);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct ads1256_chip g_acq_chip[ACQUNITY_ACQ_CHANNELS];

static const struct ads1256_chip_vt g_acq_chip_vt[ACQUNITY_ACQ_CHANNELS] =
{
    {
        acq_0_drdy_is_active,
        acq_0_drdy_isr_enable,
        acq_0_drdy_isr_disable,
        acq_0_cs_enable,
        acq_0_cs_disable
    },
    {
        acq_1_drdy_is_active,
        acq_1_drdy_isr_enable_high,
        acq_1_drdy_isr_disable,
        acq_1_cs_enable,
        acq_1_cs_disable
    },
    {
        acq_2_drdy_is_active,
        acq_2_drdy_isr_enable_high,
        acq_2_drdy_isr_disable,
        acq_2_cs_enable,
        acq_2_cs_disable
    }
};

static struct spi_bus * const g_acq_chip_spi_bus[ACQUNITY_ACQ_CHANNELS] =
{
    &HWCON_ACQ_0_SPI,
    &HWCON_ACQ_1_SPI,
    &HWCON_ACQ_2_SPI,
};

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

/* -------------------------------------------------------------------------- *
 * Channel 0 methods
 * -------------------------------------------------------------------------- */


static void acq_0_drdy_isr_enable(void)
{
	/* Clear EXTI controller interrupt bit, because this is holding interrupt.
	 */
	__HAL_GPIO_EXTI_CLEAR_IT(HWCON_ACQ_0_DRDY_PIN);
    NVIC_ClearPendingIRQ(HWCON_ACQ_0_DRDY_EXTI);
    NVIC_EnableIRQ(HWCON_ACQ_0_DRDY_EXTI);
}



static void acq_0_drdy_isr_disable(void)
{
    NVIC_DisableIRQ(HWCON_ACQ_0_DRDY_EXTI);
}



static bool acq_0_drdy_is_active(void)
{
    if (gpio_read(HWCON_ACQ_0_DRDY_PORT, HWCON_ACQ_0_DRDY_PIN)) {
        return (true);
    } else {
        return (false);
    }
}



static void acq_0_cs_enable(void)
{
    gpio_clr(HWCON_ACQ_0_SPI_NSS_PORT, HWCON_ACQ_0_SPI_NSS_PIN);
}



static void acq_0_cs_disable(void)
{
    gpio_set(HWCON_ACQ_0_SPI_NSS_PORT, HWCON_ACQ_0_SPI_NSS_PIN);
}

/* -------------------------------------------------------------------------- *
 * Channel 1 methods
 * -------------------------------------------------------------------------- */


static void acq_1_drdy_isr_enable_high(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(HWCON_ACQ_1_DRDY_PIN);
	NVIC_ClearPendingIRQ(HWCON_ACQ_1_DRDY_EXTI);
    NVIC_EnableIRQ(HWCON_ACQ_1_DRDY_EXTI);
}



static void acq_1_drdy_isr_disable(void)
{
    NVIC_DisableIRQ(HWCON_ACQ_1_DRDY_EXTI);
}



static bool acq_1_drdy_is_active(void)
{
    if (gpio_read(HWCON_ACQ_1_DRDY_PORT, HWCON_ACQ_1_DRDY_PIN)) {
        return (true);
    } else {
        return (false);
    }
}



static void acq_1_cs_enable(void)
{
    gpio_clr(HWCON_ACQ_1_SPI_NSS_PORT, HWCON_ACQ_1_SPI_NSS_PIN);
}



static void acq_1_cs_disable(void)
{
    gpio_set(HWCON_ACQ_1_SPI_NSS_PORT, HWCON_ACQ_1_SPI_NSS_PIN);
}

/* -------------------------------------------------------------------------- *
 * Channel 2 methods
 * -------------------------------------------------------------------------- */


static void acq_2_drdy_isr_enable_high(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(HWCON_ACQ_2_DRDY_PIN);
	NVIC_ClearPendingIRQ(HWCON_ACQ_2_DRDY_EXTI);
    NVIC_EnableIRQ(HWCON_ACQ_2_DRDY_EXTI);
}



static void acq_2_drdy_isr_disable(void)
{
    NVIC_DisableIRQ(HWCON_ACQ_2_DRDY_EXTI);
}



static bool acq_2_drdy_is_active(void)
{
    if (gpio_read(HWCON_ACQ_2_DRDY_PORT, HWCON_ACQ_2_DRDY_PIN)) {
        return (true);
    } else {
        return (false);
    }
}



static void acq_2_cs_enable(void)
{
    gpio_clr(HWCON_ACQ_2_SPI_NSS_PORT, HWCON_ACQ_2_SPI_NSS_PIN);
}



static void acq_2_cs_disable(void)
{
    gpio_set(HWCON_ACQ_2_SPI_NSS_PORT, HWCON_ACQ_2_SPI_NSS_PIN);
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

void acq_x_init(void)
{
    uint32_t                    index;

    for (index = 0; index < ACQUNITY_ACQ_CHANNELS; index++) {
        ads1256_init(&g_acq_chip[index], g_acq_chip_spi_bus[index],
                &g_acq_chip_vt[index]);
    }
    acq_x_reset();
}

void acq_x_reset(void)
{
    uint32_t                    index;

    for (index = 0; index < ACQUNITY_ACQ_CHANNELS; index++) {
        ads1256_wait_ready(&g_acq_chip[index]);
        ads1256_reset_sync(&g_acq_chip[index]);
    }
}

nerror acq_x_set_config(const struct acq_x_config * config)
{
    static const uint8_t sps_table[] =
    {
        [ACQ_X_SPS_10] = ADS_DRATE_10SPS,
        [ACQ_X_SPS_1000] = ADS_DRATE_100SPS
    };
    uint32_t                    index;

    acq_x_reset();

    for (index = 0; index < ACQUNITY_ACQ_CHANNELS; index++) {
        ads1256_wait_ready(&g_acq_chip[index]);
        ads1256_write_reg_sync(&g_acq_chip[index], ADS_REG_DRATE,
                sps_table[config->sps]);
    }

    return (NERROR_NONE);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of acq_channels.c
 ******************************************************************************/
