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
#include "acq_sync.h"
#include "app_stat.h"
#include "prim_gpio.h"
#include "prim_spi.h"

#include "acq_channels.h"
#include "hw_config.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static void acq_0_init(void);
static void acq_0_drdy_isr_enable(void);
static void acq_0_drdy_isr_disable(void);
static bool acq_0_drdy_is_active(void);
static void acq_0_cs_enable(void);
static void acq_0_cs_disable(void);

static void acq_1_init(void);
static void acq_1_drdy_isr_enable(void);
static void acq_1_drdy_isr_disable(void);
static bool acq_1_drdy_is_active(void);
static void acq_1_cs_enable(void);
static void acq_1_cs_disable(void);

static void acq_2_init(void);
static void acq_2_drdy_isr_enable(void);
static void acq_2_drdy_isr_disable(void);
static bool acq_2_drdy_is_active(void);
static void acq_2_cs_enable(void);
static void acq_2_cs_disable(void);

static void trigger_out_finished_2_wrap(struct spi_transfer * transfer);

/*=======================================================  LOCAL VARIABLES  ==*/

static const struct ads1256_chip_vt g_acq_chip_vt[ACQUNITY_ACQ_CHANNELS] =
{
    {
        acq_0_drdy_is_active,
        acq_0_drdy_isr_enable,
        acq_0_drdy_isr_disable,
    },
    {
        acq_1_drdy_is_active,
        acq_1_drdy_isr_enable,
        acq_1_drdy_isr_disable
    },
    {
        acq_2_drdy_is_active,
        acq_2_drdy_isr_enable,
        acq_2_drdy_isr_disable
    }
};

/*======================================================  GLOBAL VARIABLES  ==*/

struct acq_channels		g_acq;

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

/* -------------------------------------------------------------------------- *
 * Channel 0 methods
 * -------------------------------------------------------------------------- */

static void acq_0_init(void)
{
    /*
     * Channel X
     */
    GPIO_InitTypeDef            pin_config;

    /* SPI */
    ACQ_0_SPI_CLK_ENABLE();

    /* DRDY */
    ACQ_0_DRDY_CLK_ENABLE();
    pin_config.Mode         = GPIO_MODE_IT_FALLING;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Pin          = ACQ_0_DRDY_PIN;
    HAL_GPIO_Init(ACQ_0_DRDY_PORT, &pin_config);

    /* CS */
    ACQ_0_SPI_NSS_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_OUTPUT_PP;
    pin_config.Pin          = ACQ_0_SPI_NSS_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(ACQ_0_SPI_NSS_GPIO_PORT, &pin_config);

    /* MISO */
    ACQ_0_SPI_MISO_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = ACQ_0_SPI_MISO_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = ACQ_0_SPI_MISO_AF;
    HAL_GPIO_Init(ACQ_0_SPI_MISO_GPIO_PORT, &pin_config);

    /* MOSI */
    ACQ_0_SPI_MOSI_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = ACQ_0_SPI_MOSI_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = ACQ_0_SPI_MOSI_AF;
    HAL_GPIO_Init(ACQ_0_SPI_MOSI_GPIO_PORT, &pin_config);

    /* SCK */
    ACQ_0_SPI_SCK_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = ACQ_0_SPI_SCK_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = ACQ_0_SPI_SCK_AF;
    HAL_GPIO_Init(ACQ_0_SPI_SCK_GPIO_PORT, &pin_config);

    HAL_NVIC_SetPriority(ACQ_0_DRDY_EXTI, IRQ_PRIO_ACQ_x_EXTI, 0);
    HAL_NVIC_SetPriority(ACQ_0_SPI_IRQ, IRQ_PRIO_ACQ_x_SPI, 0);
    HAL_NVIC_ClearPendingIRQ(ACQ_0_SPI_IRQ);
    HAL_NVIC_EnableIRQ(ACQ_0_SPI_IRQ);
}



static void acq_0_drdy_isr_enable(void)
{
	/* Clear EXTI controller interrupt bit, because this is holding interrupt.
	 */
	__HAL_GPIO_EXTI_CLEAR_IT(ACQ_0_DRDY_PIN);
    NVIC_ClearPendingIRQ(ACQ_0_DRDY_EXTI);
    NVIC_EnableIRQ(ACQ_0_DRDY_EXTI);
}



static void acq_0_drdy_isr_disable(void)
{
    NVIC_DisableIRQ(ACQ_0_DRDY_EXTI);
}



static bool acq_0_drdy_is_active(void)
{
    if (gpio_read(ACQ_0_DRDY_PORT, ACQ_0_DRDY_PIN)) {
        return (true);
    } else {
        return (false);
    }
}



static void acq_0_cs_enable(void)
{
    gpio_clr(ACQ_0_SPI_NSS_GPIO_PORT, ACQ_0_SPI_NSS_PIN);
}



static void acq_0_cs_disable(void)
{
    gpio_set(ACQ_0_SPI_NSS_GPIO_PORT, ACQ_0_SPI_NSS_PIN);
}

/* -------------------------------------------------------------------------- *
 * Channel 1 methods
 * -------------------------------------------------------------------------- */

static void acq_1_init(void)
{
    /*
     * Channel Y
     */
    GPIO_InitTypeDef            pin_config;

    /* SPI */
    ACQ_1_SPI_CLK_ENABLE();

    /* DRDY */
    ACQ_1_DRDY_CLK_ENABLE();
    pin_config.Mode         = GPIO_MODE_IT_FALLING;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Pin          = ACQ_1_DRDY_PIN;
    HAL_GPIO_Init(ACQ_1_DRDY_PORT, &pin_config);

    /* CS */
    ACQ_1_SPI_NSS_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_OUTPUT_PP;
    pin_config.Pin          = ACQ_1_SPI_NSS_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(ACQ_1_SPI_NSS_GPIO_PORT, &pin_config);

    /* MISO */
    ACQ_1_SPI_MISO_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = ACQ_1_SPI_MISO_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = ACQ_1_SPI_MISO_AF;
    HAL_GPIO_Init(ACQ_1_SPI_MISO_GPIO_PORT, &pin_config);

    /* MOSI */
    ACQ_1_SPI_MOSI_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = ACQ_1_SPI_MOSI_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = ACQ_1_SPI_MOSI_AF;
    HAL_GPIO_Init(ACQ_1_SPI_MOSI_GPIO_PORT, &pin_config);

    /* SCK */
    ACQ_1_SPI_SCK_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = ACQ_1_SPI_SCK_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = ACQ_1_SPI_SCK_AF;
    HAL_GPIO_Init(ACQ_1_SPI_SCK_GPIO_PORT, &pin_config);

    HAL_NVIC_SetPriority(ACQ_1_DRDY_EXTI, IRQ_PRIO_ACQ_x_EXTI, 0);
    HAL_NVIC_SetPriority(ACQ_1_SPI_IRQ, IRQ_PRIO_ACQ_x_SPI, 0);
    HAL_NVIC_ClearPendingIRQ(ACQ_1_SPI_IRQ);
    HAL_NVIC_EnableIRQ(ACQ_1_SPI_IRQ);
}



static void acq_1_drdy_isr_enable(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(ACQ_1_DRDY_PIN);
	NVIC_ClearPendingIRQ(ACQ_1_DRDY_EXTI);
    NVIC_EnableIRQ(ACQ_1_DRDY_EXTI);
}



static void acq_1_drdy_isr_disable(void)
{
    NVIC_DisableIRQ(ACQ_1_DRDY_EXTI);
}



static bool acq_1_drdy_is_active(void)
{
    if (gpio_read(ACQ_1_DRDY_PORT, ACQ_1_DRDY_PIN)) {
        return (true);
    } else {
        return (false);
    }
}



static void acq_1_cs_enable(void)
{
    gpio_clr(ACQ_1_SPI_NSS_GPIO_PORT, ACQ_1_SPI_NSS_PIN);
}



static void acq_1_cs_disable(void)
{
    gpio_set(ACQ_1_SPI_NSS_GPIO_PORT, ACQ_1_SPI_NSS_PIN);
}

/* -------------------------------------------------------------------------- *
 * Channel 2 methods
 * -------------------------------------------------------------------------- */


static void acq_2_init(void)
{
    /*
     * Channel Z
     */
    GPIO_InitTypeDef            pin_config;

    /* SPI */
    ACQ_2_SPI_CLK_ENABLE();

    /* DRDY */
    ACQ_2_DRDY_CLK_ENABLE();
    pin_config.Mode         = GPIO_MODE_IT_FALLING;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Pin          = ACQ_2_DRDY_PIN;
    HAL_GPIO_Init(ACQ_2_DRDY_PORT, &pin_config);

    /* CS */
    ACQ_2_SPI_NSS_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_OUTPUT_PP;
    pin_config.Pin          = ACQ_2_SPI_NSS_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(ACQ_2_SPI_NSS_GPIO_PORT, &pin_config);

    /* MISO */
    ACQ_2_SPI_MISO_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = ACQ_2_SPI_MISO_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = ACQ_2_SPI_MISO_AF;
    HAL_GPIO_Init(ACQ_2_SPI_MISO_GPIO_PORT, &pin_config);

    /* MOSI */
    ACQ_2_SPI_MOSI_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = ACQ_2_SPI_MOSI_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = ACQ_2_SPI_MOSI_AF;
    HAL_GPIO_Init(ACQ_2_SPI_MOSI_GPIO_PORT, &pin_config);

    /* SCK */
    ACQ_2_SPI_SCK_GPIO_ENABLE();
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = ACQ_2_SPI_SCK_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = ACQ_2_SPI_SCK_AF;
    HAL_GPIO_Init(ACQ_2_SPI_SCK_GPIO_PORT, &pin_config);

    HAL_NVIC_SetPriority(ACQ_2_DRDY_EXTI, IRQ_PRIO_ACQ_x_EXTI, 0);
    HAL_NVIC_SetPriority(ACQ_2_SPI_IRQ, IRQ_PRIO_ACQ_x_SPI, 0);
    HAL_NVIC_ClearPendingIRQ(ACQ_2_SPI_IRQ);
    HAL_NVIC_EnableIRQ(ACQ_2_SPI_IRQ);
}



static void acq_2_drdy_isr_enable(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(ACQ_2_DRDY_PIN);
	NVIC_ClearPendingIRQ(ACQ_2_DRDY_EXTI);
    NVIC_EnableIRQ(ACQ_2_DRDY_EXTI);
}



static void acq_2_drdy_isr_disable(void)
{
    NVIC_DisableIRQ(ACQ_2_DRDY_EXTI);
}



static bool acq_2_drdy_is_active(void)
{
    if (gpio_read(ACQ_2_DRDY_PORT, ACQ_2_DRDY_PIN)) {
        return (true);
    } else {
        return (false);
    }
}



static void acq_2_cs_enable(void)
{
    gpio_clr(ACQ_2_SPI_NSS_GPIO_PORT, ACQ_2_SPI_NSS_PIN);
}



static void acq_2_cs_disable(void)
{
    gpio_set(ACQ_2_SPI_NSS_GPIO_PORT, ACQ_2_SPI_NSS_PIN);
}

/* -------------------------------------------------------------------------- *
 * IRQ handler helper functions
 * -------------------------------------------------------------------------- */

/* NOTE:
 * Pre pozivanja pravih ISR handlera za zavrsetak citanja gotovih podataka
 * poziva se ova funkcija kako bi se generisala zadnja ivica TRIG output signala
 */
static
void trigger_out_finished_2_wrap(struct spi_transfer * transfer)
{
	trigger_out_conditional_disable();
	acq_transfer_finished_2(transfer);
}



/* NOTE:
 * Citanje podatka koji se odbacuje
 */
static
void isr_dummy_read(struct spi_transfer * transfer)
{
	(void)transfer;
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/


void acq_x_bus_init(void)
{
    uint32_t                    idx;
    struct spi_config           spi_config;

    for (idx = 0; idx < ACQUNITY_ACQ_CHANNELS; idx++) {
        spi_config.flags    = SPI_TRANSFER_TX   | SPI_TRANSFER_RX   |
                              SPI_CLK_POL_LOW   |
                              SPI_CLK_PHA_2EDGE |
                              SPI_MODE_MS       |
                              SPI_SS_SOFTWARE   |
                              SPI_DATA_8BIT;

        switch (idx) {
            case ACQ_CHANNEL_X: {
                spi_config.prescaler = ACQ_0_SPI_BAUD_CLOCK;
                spi_host_init(&ACQ_0_SPI, &spi_config, acq_0_init);
                spi_client_init(
                        &g_acq.chn[idx].client,
                        &ACQ_0_SPI,
                        acq_0_cs_enable,
                        acq_0_cs_disable);
                break;
            }
            case ACQ_CHANNEL_Y: {
                spi_config.prescaler = ACQ_1_SPI_BAUD_CLOCK;
                spi_host_init(&ACQ_1_SPI, &spi_config, acq_1_init);
                spi_client_init(
                        &g_acq.chn[idx].client,
                        &ACQ_1_SPI,
                        acq_1_cs_enable,
                        acq_1_cs_disable);
                break;
            }
            case ACQ_CHANNEL_Z: {
                spi_config.prescaler = ACQ_2_SPI_BAUD_CLOCK;
                spi_host_init(&ACQ_2_SPI, &spi_config, acq_2_init);
                spi_client_init(
                        &g_acq.chn[idx].client,
                        &ACQ_2_SPI,
                        acq_2_cs_enable,
                        acq_2_cs_disable);
                break;
            }
            default: {
            	status_panic(20);
            }
        }
        ads1256_init(
                &g_acq.chn[idx].chip,
                &g_acq.chn[idx].client,
                &g_acq_chip_vt[idx],
				idx);
    }
}



void acq_x_reset(uint32_t channel_no)
{
	ads1256_reset_sync(&g_acq.chn[channel_no].chip);
}



void acq_x_write_reg(uint32_t channel_no, uint32_t reg, uint8_t data)
{
    ads1256_write_reg_sync(&g_acq.chn[channel_no].chip, reg, data);
}



uint8_t acq_x_read_reg(uint32_t channel_no, uint32_t reg)
{
    return (ads1256_read_reg_sync(&g_acq.chn[channel_no].chip, reg));
}




void acq_x_channel_enable(uint32_t channel_no)
{
	g_acq.chn[channel_no].is_enabled = true;
}



void acq_x_channel_disable(uint32_t channel_no)
{
	g_acq.chn[channel_no].is_enabled = false;
}



bool acq_x_is_enabled(uint32_t channel_no)
{
    return (g_acq.chn[channel_no].is_enabled);
}



void acq_x_trigger_mode(uint32_t trig_mode)
{
	g_acq.trig_mode  		= trig_mode;
	g_acq.trig_is_allowed 	= false;
}



void acq_x_trigger_enable(void)
{
	g_acq.trig_is_allowed = true;
}



void acq_x_mode(uint32_t mode)
{
	g_acq.acq_mode = mode;
}



void acq_x_trigger_sync_min_delay(void)
{
	ads1256_sync_min_delay();
}



void acq_x_isr_enable(void (* fn)(void))
{
    uint32_t                    idx;

    g_acq.isr_begin_transfer = fn;

    /* NOTE:
     * Turn on EXTI interrupt only for the first is_enabled ADC channel. In this
     * mode all other channels are read from single EXTI interrupt.
     */
    for (idx = 0; idx < ACQUNITY_ACQ_CHANNELS; idx++) {
        if (g_acq.chn[idx].is_enabled) {
            ads1256_drdy_isr_enable(&g_acq.chn[idx].chip);

            break;
        }
    }
}


/**********************************************************************
  * @name   acq_x_start_rdc()
  * @brief  put all adc-s in continuos conversion mode
   *********************************************************************/

void acq_x_start_rdc(void)
{
	uint32_t                    idx;

	for (idx = 0; idx < ACQUNITY_ACQ_CHANNELS; idx++) {
								   /* Enable RDATAC mode for all channels */
		ads1256_rdc_setup(&g_acq.chn[idx].chip);
	}
}


/**********************************************************************
  * @name			acq_x_isr_disable()
  * @brief  	disables all interrupt service routines
  *********************************************************************/
void acq_x_isr_disable(void)
{
	uint32_t                    idx;

	for (idx = 0; idx < ACQUNITY_ACQ_CHANNELS; idx++) {
		ads1256_drdy_isr_disable(&g_acq.chn[idx].chip);
	}
}


/**********************************************************************
  * @name		acq_x_stop_rdc()
  * @brief  stop data read in continious acquisition mode (ads1256)
  *********************************************************************/
void acq_x_stop_rdc(void)
{
    uint32_t                    idx;

    for (idx = 0; idx < ACQUNITY_ACQ_CHANNELS; idx++) {
		ads1256_rdc_finish(&g_acq.chn[idx].chip);
    }
}


/* Interrupt Service Routine
 *
 * ISR             : pocetak citanja podataka
 * Trigger mode    : out
 * Acquisition mode: RDC (Read Data Continuously)
 */
void acq_isr_begin_rdc_trigger_out(void)
{
    trigger_out_enable();

	ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_X].chip,
			acq_transfer_finished);

	ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_Y].chip,
			acq_transfer_finished);

    /* NOTE:
     * trigger_out_finished_2_wrap() se poziva jer ona clear-uje TRIGGER OUT pin
     * pre poziva trigger_out_finished_2() funkcije.
     */
	ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_Z].chip,
			trigger_out_finished_2_wrap);
}




/* Interrupt Service Routine
 *
 * ISR             : pocetak citanja podataka
 * Trigger mode    : in, single shot
 * Acquisition mode: RDC (Read Data Continuously)
 */
void acq_isr_begin_rdc_trigger_in_s(void)
{
	if (g_acq.trig_is_allowed) {
		ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_X].chip,
			acq_transfer_finished);

		ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_Y].chip,
			acq_transfer_finished);

		ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_Z].chip,
			acq_transfer_finished);
	} else {
		/* NOTE:
		 * This data is read by the controller but not stored anywhere, so use
		 * isr_dummy_read() function.
		 */
		ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_X].chip,
			isr_dummy_read);

		ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_Y].chip,
			isr_dummy_read);

		ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_Z].chip,
			isr_dummy_read);
	}
}



/* Interrupt Service Routine
 *
 * ISR             : pocetak citanja podataka
 * Trigger mode    : in, continuous
 * Acquisition mode: RDC (Read Data Continuously)
 */
void acq_isr_begin_rdc_trigger_in_c(void)
{
	if (g_acq.trig_is_allowed) {
		/* NOTE:
		 * Since this is triggered read, toggle enable bit to false.
		 */
		g_acq.trig_is_allowed = false;

		ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_X].chip,
			acq_transfer_finished);

		ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_Y].chip,
			acq_transfer_finished);

		ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_Z].chip,
			acq_transfer_finished);
	} else {
		/* NOTE:
		 * This data is read by the controller but not stored anywhere, so use
		 * isr_dummy_read() function.
		 */
		ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_X].chip,
			isr_dummy_read);

		ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_Y].chip,
			isr_dummy_read);

		ads1256_rdc_read_async(&g_acq.chn[ACQ_CHANNEL_Z].chip,
			isr_dummy_read);
	}
}



static void rc_read_begin_complete_2(struct spi_transfer * transfer)
{
	(void)transfer;

	ads1256_read_delay();
	ads1256_read_finish_sync(&g_acq.chn[ACQ_CHANNEL_X].chip,
		acq_transfer_finished);
	ads1256_read_finish_sync(&g_acq.chn[ACQ_CHANNEL_Y].chip,
		acq_transfer_finished);
	ads1256_read_finish_sync(&g_acq.chn[ACQ_CHANNEL_Z].chip,
		acq_transfer_finished);
}



void acq_isr_begin_rc_trigger_in_c(void)
{
	if (g_acq.trig_is_allowed) {
		g_acq.trig_is_allowed = false;
		ads1256_read_begin_sync(&g_acq.chn[ACQ_CHANNEL_X].chip, isr_dummy_read);
		ads1256_read_begin_sync(&g_acq.chn[ACQ_CHANNEL_Y].chip, isr_dummy_read);
		ads1256_read_begin_sync(&g_acq.chn[ACQ_CHANNEL_Z].chip, rc_read_begin_complete_2);
	}
}


/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of acq_channels.c
 ******************************************************************************/
