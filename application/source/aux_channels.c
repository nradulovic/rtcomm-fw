/*
 * aux_channels.c
 *
 *  Created on: May 26, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Auxiliary channel code
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include <prim_gpio.h>
#include <prim_spi.h>
#include "aux_channels.h"
#include "ads1256.h"
#include "hw_config.h"
#include "cdi/io.h"
#include "app_stat.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/

struct aux_channel
{
    struct spi_client           client;
    struct ads1256_chip         chip;
    void                     (* isr_handler)(void);
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static void aux_init(void);
static void aux_drdy_isr_enable(void);
static void aux_drdy_isr_disable(void);
static bool aux_drdy_is_active(void);
static void aux_cs_enable(void);
static void aux_cs_disable(void);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct aux_channel       g_aux_channel;
static const struct ads1256_chip_vt g_aux_chip_vt =
{
    aux_drdy_is_active,
    aux_drdy_isr_enable,
    aux_drdy_isr_disable
};

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/


static void aux_init(void)
{
    /*
     * Channel AUX
     */
    GPIO_InitTypeDef            pin_config;

    /* SPI */
    ACQ_AUX_SPI_CLK_ENABLE();

    /* DRDY */
    ACQ_AUX_DRDY_CLK_ENABLE();
    pin_config.Mode         = GPIO_MODE_IT_FALLING;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Pin          = ACQ_AUX_DRDY_PIN;
    HAL_GPIO_Init(ACQ_AUX_DRDY_PORT, &pin_config);

    /* NSS */
    ACQ_AUX_SPI_NSS_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_OUTPUT_PP;
    pin_config.Pin          = ACQ_AUX_SPI_NSS_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(ACQ_AUX_SPI_NSS_GPIO_PORT, &pin_config);

    /* MISO */
    ACQ_AUX_SPI_MISO_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = ACQ_AUX_SPI_MISO_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = ACQ_AUX_SPI_MISO_AF;
    HAL_GPIO_Init(ACQ_AUX_SPI_MISO_GPIO_PORT, &pin_config);

    /* MOSI */
    ACQ_AUX_SPI_MOSI_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = ACQ_AUX_SPI_MOSI_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = ACQ_AUX_SPI_MOSI_AF;
    HAL_GPIO_Init(ACQ_AUX_SPI_MOSI_GPIO_PORT, &pin_config);

    /* SCK */
    ACQ_AUX_SPI_SCK_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = ACQ_AUX_SPI_SCK_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = ACQ_AUX_SPI_SCK_AF;
    HAL_GPIO_Init(ACQ_AUX_SPI_SCK_GPIO_PORT, &pin_config);

    HAL_NVIC_SetPriority(ACQ_AUX_DRDY_EXTI, IRQ_PRIO_ACQ_AUX_EXTI, 0);
    HAL_NVIC_SetPriority(ACQ_AUX_SPI_IRQ, IRQ_PRIO_ACQ_AUX_SPI, 0);
    HAL_NVIC_ClearPendingIRQ(ACQ_AUX_SPI_IRQ);
    HAL_NVIC_EnableIRQ(ACQ_AUX_SPI_IRQ);
}



static void aux_drdy_isr_enable(void)
{
    NVIC_EnableIRQ(ACQ_AUX_DRDY_EXTI);
}



static void aux_drdy_isr_disable(void)
{
    NVIC_DisableIRQ(ACQ_AUX_DRDY_EXTI);
}



/*------------------------------------------------------------------------*//**
 * @name     aux_drdy_is_active()
 * @{ *//*-------------------------------------------------------------------*/

/**@brief    return status of aux_drdy data line
 * @param    no input parameters
 * @retval   true: if active; false: if not
 *
 */
static
bool aux_drdy_is_active(void)
{
    if (gpio_read(ACQ_AUX_DRDY_PORT, ACQ_AUX_DRDY_PIN)) {
        return (true);
    } else {
        return (false);
    }
}


static
void aux_cs_enable(void)
{
    gpio_clr(ACQ_AUX_SPI_NSS_GPIO_PORT, ACQ_AUX_SPI_NSS_PIN);
}



static
void aux_cs_disable(void)
{
    gpio_set(ACQ_AUX_SPI_NSS_GPIO_PORT, ACQ_AUX_SPI_NSS_PIN);
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/


void aux_channel_init(void)
{
    struct spi_config spi_config;

    spi_config.flags    = SPI_TRANSFER_TX   | SPI_TRANSFER_RX   |
                          SPI_CLK_POL_LOW   |
                          SPI_CLK_PHA_2EDGE |
                          SPI_MODE_MS       |
                          SPI_SS_SOFTWARE   |
                          SPI_DATA_8BIT;
    spi_config.prescaler = ACQ_AUX_SPI_BAUD_CLOCK;
    spi_host_init(&ACQ_AUX_SPI, &spi_config, aux_init);
    spi_client_init(
            &g_aux_channel.client,
            &ACQ_AUX_SPI,
            aux_cs_enable,
            aux_cs_disable);
    ads1256_init(&g_aux_channel.chip, &g_aux_channel.client, &g_aux_chip_vt, 0);
}



void aux_set_signal(void (* signal)(void))
{
    g_aux_channel.isr_handler = signal;
}



void aux_signal_enable(void)
{
    NVIC_ClearPendingIRQ(ACQ_AUX_DRDY_EXTI);
    NVIC_EnableIRQ(ACQ_AUX_DRDY_EXTI);
}



void aux_signal_disable(void)
{
    NVIC_DisableIRQ(ACQ_AUX_DRDY_EXTI);
}



void aux_reset(void)
{
    ads1256_reset_sync(&g_aux_channel.chip);
}



void aux_write_reg(
    uint32_t                    reg,
    uint8_t                     data)
{
    ads1256_write_reg_sync(&g_aux_channel.chip, reg, data);
}



uint8_t aux_read_reg(
    uint32_t                    reg)
{
    return (ads1256_read_reg_sync(&g_aux_channel.chip, reg));
}



uint8_t aux_write_then_read_reg(uint32_t reg, uint8_t data)
{
	return (ads1256_write_then_read_reg_sync(&g_aux_channel.chip, reg, data));
}


void aux_write_cmd(
    uint8_t                     cmd)
{
    ads1256_set_cmd_sync_no_wait(&g_aux_channel.chip, cmd);
}



uint32_t aux_read_data(void)
{
	struct spi_transfer *	transfer = &g_aux_channel.chip.transfer;
	uint32_t 				retval;

	ads1256_read_sync(&g_aux_channel.chip);
    /* NOTE:
	 * Take the transfer buffer of 8 chars and convert it to a single 4 byte
	 * integer, then reverse the bytes in it to match CPU endian.
	 * This code will generate warning on certain optimization levels.
	 */
	retval = __REV(*(uint32_t *)&transfer->buff[0]) >> 8u;

    return (retval);
}



bool aux_is_drdy_active(void)
{
    return (aux_drdy_is_active());
}



void acq_aux_drdy_isr(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(ACQ_AUX_DRDY_PIN) != RESET) {
        g_aux_channel.isr_handler();
        __HAL_GPIO_EXTI_CLEAR_IT(ACQ_AUX_DRDY_PIN);
    }
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of aux_channels.c
 ******************************************************************************/
