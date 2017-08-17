/* 
 *  rtcomm-fw - 2017
 *
 *  psm.c
 *
 *  Created on: Jun 5, 2017
 * ----------------------------------------------------------------------------
 *  This file is part of rtcomm-fw.
 *
 *  rtcomm-fw is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  rtcomm-fw is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with rtcomm-fw.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------- *//**
 * @file
 * @author      Nenad Radulovic
 * @brief       PSM (Peripheral Support Module)
 * @details     This module contains initialization and de-initialization for
 *              MCU peripherals.
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include <string.h>

#include "psm.h"
#include "status.h"
#include "rtcomm.h"
#include "cdi/io.h"
#include "epa_ctrl.h"
#include "prim_spi.h"
#include "config/hwcon.h"
#include "stm32f4xx_hal.h"

#if defined(HWCON_TEST_TIMER0_ENABLE)
#include "test_timer0.h"
#endif

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/

struct gpio_setup
{
    void *                      port;
    uint16_t                    pin;
    uint32_t                    mode;
    uint32_t                    pull;
    uint32_t                    func;
    uint32_t                    default_state;
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/

static const struct gpio_setup  g_gpio_pins[] =
{
/* +---------------------------+-----------------------+-----------------------+---------------+-----------------------+---------------+ */
/* | GPIO Port                 | GPIO Pin              | Mode                  | Pull up/down  | Alternate function    | Default state | */
/* +---------------------------+-----------------------+-----------------------+---------------+-----------------------+---------------+ */

    /* LED indicators */
    {HWCON_HEARTBEAT_PORT,      HWCON_HEARTBEAT_PIN,    GPIO_MODE_OUTPUT_PP,    GPIO_NOPULL,    0,                      GPIO_PIN_RESET},
    {HWCON_PROTOCOL_LED_PORT,   HWCON_PROTOCOL_LED_PIN, GPIO_MODE_OUTPUT_PP,    GPIO_NOPULL,    0,                      GPIO_PIN_SET},

    /* Trigger pins */
    {HWCON_TRIGGER_OUT_PORT,    HWCON_TRIGGER_OUT_PIN,  GPIO_MODE_OUTPUT_PP,    GPIO_NOPULL,    0,                      GPIO_PIN_RESET},
    {HWCON_TRIGGER_IN_PORT,     HWCON_TRIGGER_IN_PIN,   GPIO_MODE_INPUT,        GPIO_NOPULL,    0,                      0},

    /* CDI RTCOMM bus pins */
    {HWCON_RTCOMM_MOSI_PORT,    HWCON_RTCOMM_MOSI_PIN,  GPIO_MODE_AF_PP,        GPIO_PULLUP,    HWCON_RTCOMM_MOSI_AF,   0},
    {HWCON_RTCOMM_MISO_PORT,    HWCON_RTCOMM_MISO_PIN,  GPIO_MODE_AF_PP,        GPIO_PULLUP,    HWCON_RTCOMM_MISO_AF,   0},
    {HWCON_RTCOMM_SCK_PORT,     HWCON_RTCOMM_SCK_PIN,   GPIO_MODE_AF_PP,        GPIO_PULLUP,    HWCON_RTCOMM_SCK_AF,    0},
    {HWCON_RTCOMM_NSS_PORT,     HWCON_RTCOMM_NSS_PIN,   GPIO_MODE_AF_PP,        GPIO_NOPULL,    HWCON_RTCOMM_NSS_AF,    0},
    {HWCON_RTCOMM_RRQ_PORT,     HWCON_RTCOMM_RRQ_PIN,   GPIO_MODE_OUTPUT_PP,    GPIO_NOPULL,    0,                      GPIO_PIN_RESET},

    /* Probe common pins */
    {HWCON_PROBE_SYNC_PORT,     HWCON_PROBE_SYNC_PIN,   GPIO_MODE_OUTPUT_PP,    GPIO_NOPULL,    0,                      GPIO_PIN_RESET},

    /* Probe X axis pins */
    {HWCON_PROBE_X_MOSI_PORT,   HWCON_PROBE_X_MOSI_PIN, GPIO_MODE_AF_PP,        GPIO_PULLDOWN,  HWCON_PROBE_X_MOSI_AF,  0},
    {HWCON_PROBE_X_MISO_PORT,   HWCON_PROBE_X_MISO_PIN, GPIO_MODE_AF_PP,        GPIO_PULLDOWN,  HWCON_PROBE_X_MISO_AF,  0},
    {HWCON_PROBE_X_SCK_PORT,    HWCON_PROBE_X_SCK_PIN,  GPIO_MODE_AF_PP,        GPIO_PULLDOWN,  HWCON_PROBE_X_SCK_AF,   0},
    {HWCON_PROBE_X_NSS_PORT,    HWCON_PROBE_X_NSS_PIN,  GPIO_MODE_OUTPUT_PP,    GPIO_NOPULL,    0,                      GPIO_PIN_SET},
    {HWCON_PROBE_X_DRDY_PORT,   HWCON_PROBE_X_DRDY_PIN, GPIO_MODE_IT_FALLING,   GPIO_NOPULL,    0,                      0},

    /* Probe Y axis pins */
    {HWCON_PROBE_Y_MOSI_PORT,   HWCON_PROBE_Y_MOSI_PIN, GPIO_MODE_AF_PP,        GPIO_PULLDOWN,  HWCON_PROBE_Y_MOSI_AF,  0},
    {HWCON_PROBE_Y_MISO_PORT,   HWCON_PROBE_Y_MISO_PIN, GPIO_MODE_AF_PP,        GPIO_PULLDOWN,  HWCON_PROBE_Y_MISO_AF,  0},
    {HWCON_PROBE_Y_SCK_PORT,    HWCON_PROBE_Y_SCK_PIN,  GPIO_MODE_AF_PP,        GPIO_PULLDOWN,  HWCON_PROBE_Y_SCK_AF,   0},
    {HWCON_PROBE_Y_NSS_PORT,    HWCON_PROBE_Y_NSS_PIN,  GPIO_MODE_OUTPUT_PP,    GPIO_NOPULL,    0,                      GPIO_PIN_SET},
    {HWCON_PROBE_Y_DRDY_PORT,   HWCON_PROBE_Y_DRDY_PIN, GPIO_MODE_IT_FALLING,   GPIO_NOPULL,    0,                      0},

    /* Probe Z axis pins */
    {HWCON_PROBE_Z_MOSI_PORT,   HWCON_PROBE_Z_MOSI_PIN, GPIO_MODE_AF_PP,        GPIO_PULLDOWN,  HWCON_PROBE_Z_MOSI_AF,  0},
    {HWCON_PROBE_Z_MISO_PORT,   HWCON_PROBE_Z_MISO_PIN, GPIO_MODE_AF_PP,        GPIO_PULLDOWN,  HWCON_PROBE_Z_MISO_AF,  0},
    {HWCON_PROBE_Z_SCK_PORT,    HWCON_PROBE_Z_SCK_PIN,  GPIO_MODE_AF_PP,        GPIO_PULLDOWN,  HWCON_PROBE_Z_SCK_AF,   0},
    {HWCON_PROBE_Z_NSS_PORT,    HWCON_PROBE_Z_NSS_PIN,  GPIO_MODE_OUTPUT_PP,    GPIO_NOPULL,    0,                      GPIO_PIN_SET},
    {HWCON_PROBE_Z_DRDY_PORT,   HWCON_PROBE_Z_DRDY_PIN, GPIO_MODE_IT_FALLING,   GPIO_NOPULL,    0,                      0},

    /* AUX pins */
    {HWCON_AUX_MOSI_PORT,       HWCON_AUX_MOSI_PIN,     GPIO_MODE_AF_PP,        GPIO_PULLDOWN,  HWCON_AUX_MOSI_AF,      0},
    {HWCON_AUX_MISO_PORT,       HWCON_AUX_MISO_PIN,     GPIO_MODE_AF_PP,        GPIO_PULLDOWN,  HWCON_AUX_MISO_AF,      0},
    {HWCON_AUX_SCK_PORT,        HWCON_AUX_SCK_PIN,      GPIO_MODE_AF_PP,        GPIO_PULLDOWN,  HWCON_AUX_SCK_AF,       0},
    {HWCON_AUX_NSS_PORT,        HWCON_AUX_NSS_PIN,      GPIO_MODE_OUTPUT_PP,    GPIO_NOPULL,    0,                      GPIO_PIN_SET},
    {HWCON_AUX_DRDY_PORT,       HWCON_AUX_DRDY_PIN,     GPIO_MODE_IT_FALLING,   GPIO_NOPULL,    0,                      0},

    /* CDI CTRL bus pins */
    {HWCON_CTRL_SCL_PORT,       HWCON_CTRL_SCL_PIN,     GPIO_MODE_AF_OD,        GPIO_PULLUP,    HWCON_CTRL_SCL_AF,      0},
    {HWCON_CTRL_SDA_PORT,       HWCON_CTRL_SDA_PIN,     GPIO_MODE_AF_OD,        GPIO_PULLUP,    HWCON_CTRL_SDA_AF,      0},

    /* -- Last setup entry needs to be zeroed -- */
    {NULL,                      0,                      0,                      0,              0,                      0}
};

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static void init_spi_rtcomm(void)
{
    /* NOTE:
     * 1. Clocks and resets are set in psm_init_clock()
     * 2. Multiplexer configuration is set in psm_init_gpios()
     * 3. SPI and DMA peripherals are setup here
     */

    /* -- SPI -- */
    g_rtcomm.spi.Instance               = HWCON_RTCOMM_SPI;
    g_rtcomm.spi.Init.BaudRatePrescaler = HWCON_RTCOMM_SPI_BAUD_CLOCK;
    g_rtcomm.spi.Init.Direction         = SPI_DIRECTION_2LINES;
    g_rtcomm.spi.Init.CLKPhase          = HWCON_RTCOMM_SPI_CLK_PHASE;
    g_rtcomm.spi.Init.CLKPolarity       = HWCON_RTCOMM_SPI_CLK_POL;
    g_rtcomm.spi.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLED;
    g_rtcomm.spi.Init.CRCPolynomial     = 7;
    g_rtcomm.spi.Init.DataSize          = SPI_DATASIZE_8BIT;
    g_rtcomm.spi.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    g_rtcomm.spi.Init.NSS               = SPI_NSS_HARD_INPUT;
    g_rtcomm.spi.Init.TIMode            = SPI_TIMODE_DISABLED;
    g_rtcomm.spi.Init.Mode              = SPI_MODE_SLAVE;

    /* -- DMA -- */
    g_rtcomm.dma_tx.Instance                 = HWCON_RTCOMM_SPI_DMA_TX_STREAM;
    g_rtcomm.dma_tx.Init.Channel             = HWCON_RTCOMM_SPI_DMA_TX_CHANNEL;
    g_rtcomm.dma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    g_rtcomm.dma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    g_rtcomm.dma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    g_rtcomm.dma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_rtcomm.dma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    g_rtcomm.dma_tx.Init.Mode                = DMA_NORMAL;
    g_rtcomm.dma_tx.Init.Priority            = DMA_PRIORITY_LOW;
    g_rtcomm.dma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    g_rtcomm.dma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    g_rtcomm.dma_tx.Init.MemBurst            = DMA_MBURST_INC4;
    g_rtcomm.dma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
    HAL_DMA_Init(&g_rtcomm.dma_tx);
    __HAL_LINKDMA(&g_rtcomm.spi, hdmatx, g_rtcomm.dma_tx);

    /* -- DMA IRQ -- */
    HAL_NVIC_SetPriority(HWCON_RTCOMM_SPI_DMA_TX_IRQn,
            HWCON_IRQ_PRIO_RTCOMM_SPI_DMA_TX, 0);
    HAL_NVIC_EnableIRQ(HWCON_RTCOMM_SPI_DMA_TX_IRQn);

    HAL_SPI_Init(&g_rtcomm.spi);
}

static void init_spi_probe(void)
{
    /* NOTE:
     * 1. Clocks and resets are set in psm_init_clock()
     * 2. Multiplexer configuration is set in psm_init_gpios()
     * 3. SPI peripherals are setup here
     */

    struct spi_config config;

    /* -- Probe X axis SPI -------------------------------------------------- */
    config.flags = SPI_TRANSFER_TX   | SPI_TRANSFER_RX   | SPI_CLK_POL_LOW   |
                   SPI_CLK_PHA_2EDGE | SPI_MODE_MS       | SPI_SS_SOFTWARE   |
                   SPI_DATA_8BIT;
    config.prescaler = HWCON_PROBE_X_SPI_BAUD_CLOCK;
    HAL_NVIC_SetPriority(HWCON_PROBE_X_SPI_IRQ, HWCON_IRQ_PRIO_PROBE_SPI, 0);
    HAL_NVIC_ClearPendingIRQ(HWCON_PROBE_X_SPI_IRQ);
    HAL_NVIC_EnableIRQ(HWCON_PROBE_X_SPI_IRQ);
    spi_bus_init(&HWCON_PROBE_X_SPI, &config);

    /* -- Probe Y axis SPI -------------------------------------------------- */
    config.prescaler = HWCON_PROBE_Y_SPI_BAUD_CLOCK;
    HAL_NVIC_SetPriority(HWCON_PROBE_Y_SPI_IRQ, HWCON_IRQ_PRIO_PROBE_SPI, 0);
    HAL_NVIC_ClearPendingIRQ(HWCON_PROBE_Y_SPI_IRQ);
    HAL_NVIC_EnableIRQ(HWCON_PROBE_Y_SPI_IRQ);
    spi_bus_init(&HWCON_PROBE_Y_SPI, &config);

    /* -- Probe Z axis SPI -------------------------------------------------- */
    config.prescaler = HWCON_PROBE_Z_SPI_BAUD_CLOCK;
    HAL_NVIC_SetPriority(HWCON_PROBE_Z_SPI_IRQ, HWCON_IRQ_PRIO_PROBE_SPI, 0);
    HAL_NVIC_ClearPendingIRQ(HWCON_PROBE_Z_SPI_IRQ);
    HAL_NVIC_EnableIRQ(HWCON_PROBE_Z_SPI_IRQ);
    spi_bus_init(&HWCON_PROBE_Z_SPI, &config);
}

static void init_spi_aux(void)
{
    /* NOTE:
     * 1. Clocks and resets are set in psm_init_clock()
     * 2. Multiplexer configuration is set in psm_init_gpios()
     * 3. SPI peripherals are setup here
     */

    struct spi_config config;

    config.flags = SPI_TRANSFER_TX   | SPI_TRANSFER_RX   | SPI_CLK_POL_LOW   |
                   SPI_CLK_PHA_2EDGE | SPI_MODE_MS       | SPI_SS_SOFTWARE   |
                   SPI_DATA_8BIT;
    config.prescaler = HWCON_AUX_SPI_BAUD_CLOCK;
    HAL_NVIC_SetPriority(HWCON_AUX_SPI_IRQ, HWCON_IRQ_PRIO_AUX_SPI, 0);
    HAL_NVIC_ClearPendingIRQ(HWCON_AUX_SPI_IRQ);
    HAL_NVIC_EnableIRQ(HWCON_AUX_SPI_IRQ);
    spi_bus_init(&HWCON_AUX_SPI, &config);
}

static void init_i2c_ctrl(void)
{
    /* NOTE:
     * 1. Clocks and resets are set in psm_init_clock()
     * 2. Multiplexer configuration is set in psm_init_gpios()
     * 3. I2C peripheral is setup here
     */

    g_ctrl.i2c.Instance              = HWCON_CTRL_I2C;
    g_ctrl.i2c.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    g_ctrl.i2c.Init.ClockSpeed       = 100000;
    g_ctrl.i2c.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
    g_ctrl.i2c.Init.DutyCycle        = I2C_DUTYCYCLE_2;
    g_ctrl.i2c.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
    g_ctrl.i2c.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
    g_ctrl.i2c.Init.OwnAddress1      = IO_ID;
    g_ctrl.i2c.Init.OwnAddress2      = 0xFE;

    HWCON_CTRL_I2C_FORCE_RESET();
    HAL_NVIC_SetPriority(HWCON_CTRL_I2C_ER_IRQn, HWCON_IRQ_PRIO_CTRL, 0);
    HAL_NVIC_ClearPendingIRQ(HWCON_CTRL_I2C_EV_IRQn);
    HAL_NVIC_EnableIRQ(HWCON_CTRL_I2C_ER_IRQn);
    HAL_NVIC_SetPriority(HWCON_CTRL_I2C_EV_IRQn, HWCON_IRQ_PRIO_CTRL, 0);
    HAL_NVIC_ClearPendingIRQ(HWCON_CTRL_I2C_ER_IRQn);
    HAL_NVIC_EnableIRQ(HWCON_CTRL_I2C_EV_IRQn);
    HWCON_CTRL_I2C_RELASE_RESET();

    HAL_I2C_Init(&g_ctrl.i2c);
}

static void deinit_i2c_ctrl(void)
{
    HAL_NVIC_DisableIRQ(HWCON_CTRL_I2C_EV_IRQn);
    HAL_NVIC_DisableIRQ(HWCON_CTRL_I2C_ER_IRQn);

    HAL_I2C_DeInit(&g_ctrl.i2c);
}

static void init__test_timer0(void)
{
#if defined(HWCON_TEST_TIMER0_ENABLE)
    /*
     * Setup TEST_TIMER0
     */
    {
        RCC_ClkInitTypeDef    clkconfig;
        uint32_t              clk;
        uint32_t              clk_divider = 0U;
        uint32_t              prescaler = 0U;
        uint32_t              dummy;

        /* Get clock configuration */
        HAL_RCC_GetClockConfig(&clkconfig, &dummy);

        /* Get APB1 prescaler */
        switch (HWCON_TEST_TIMER0_APB) {
            case 1:
                clk_divider = clkconfig.APB1CLKDivider;
                clk = HAL_RCC_GetPCLK1Freq();
                break;
            case 2:
                clk_divider = clkconfig.APB2CLKDivider;
                clk = HAL_RCC_GetPCLK2Freq();
                break;
            default:
                break;
        }

        /* Compute clock */
        if (clk_divider == RCC_HCLK_DIV2) {
            clk *= 2;
        }

        /* Compute the prescaler value to have TIM6 counter clock equal to 1MHz */
        prescaler = (uint32_t) ((clk / 1000000U) - 1U);

        /* Initialize timer instance */
        g_test_timer0.Instance = HWCON_TEST_TIMER0;

        /* Initialize TIMx peripheral as follow:
        + Period = [(TIMCLK/1000) - 1]. to have a (1/1000) s time base.
        + Prescaler = (clk / 1000000 - 1) to have a 1MHz counter clock.
        + ClockDivision = 0
        + Counter direction = Up
        */
        g_test_timer0.Init.Period = (1000000U / 1000U) - 1U;
        g_test_timer0.Init.Prescaler = prescaler;
        g_test_timer0.Init.ClockDivision = 0;
        g_test_timer0.Init.CounterMode = TIM_COUNTERMODE_UP;

        /*Configure the IRQ priority */
        HAL_NVIC_SetPriority(HWCON_TEST_TIMER0_IRQn, HWCON_TEST_TIMER0_IRQ_PRIO,
                0U);

        /* Enable the global Interrupt */
        HAL_NVIC_EnableIRQ(HWCON_TEST_TIMER0_IRQn);

        HAL_TIM_Base_Init(&g_test_timer0);
    }
#endif
}


/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

/* The system Clock is configured as follow :
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 96000000
 *            HCLK(Hz)                       = 96000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 2
 *            APB2 Prescaler                 = 1
 *            HSI Frequency(Hz)              = 8000000
 *            PLL_M                          = 8
 *            PLL_N                          = 192
 *            PLL_P                          = 2
 *            PLL_Q                          = 4
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 3
 */
void psm_init_clock(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* NOTE:
     * The voltage scaling allows optimising the power consumption when the
     * device is clocked below the maximum system frequency, to update the
     * voltage scaling value regarding system frequency refer to product
     * data-sheet.
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Enable HSI Oscillator and activate PLL with HSI as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.LSEState       = RCC_LSE_OFF;
    RCC_OscInitStruct.HSIState       = RCC_HSI_OFF;
    RCC_OscInitStruct.LSIState       = RCC_LSI_OFF;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 8;
    RCC_OscInitStruct.PLL.PLLN       = 200;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = 4;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        status_error(STATUS_FATAL_HW_INIT_FAILED);
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     * clocks dividers
     */
    RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK |
            RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
        status_error(STATUS_FATAL_HW_INIT_FAILED);
    }

    /* --  Heartbeat LED GPIO  ---------------------------------------------- */
    HWCON_HEARTBEAT_CLK_ENABLE();

    /* --  Protocol status LED GPIO  ---------------------------------------- */
    HWCON_PROTOCOL_LED_CLK_ENABLE();

    /* --  Trigger  --------------------------------------------------------- */
    HWCON_TRIGGER_OUT_CLK_ENABLE();
    HWCON_TRIGGER_IN_CLK_ENABLE();

    /* --  RTCOMM SPI & DMA  ------------------------------------------------ */
    HWCON_RTCOMM_SPI_CLK_ENABLE();
    HWCON_RTCOMM_MOSI_CLK_ENABLE();
    HWCON_RTCOMM_MISO_CLK_ENABLE();
    HWCON_RTCOMM_SCK_CLK_ENABLE();
    HWCON_RTCOMM_NSS_CLK_ENABLE();

    HWCON_RTCOMM_SPI_DMA_CLK_ENABLE();

    HWCON_RTCOMM_RRQ_CLK_ENABLE();

    /* --  Probe common ----------------------------------------------------- */
    HWCON_PROBE_SYNC_CLK_ENABLE();

    /* --  Probe X axis ----------------------------------------------------- */
    HWCON_PROBE_X_SPI_CLK_ENABLE();
    HWCON_PROBE_X_MOSI_CLK_ENABLE();
    HWCON_PROBE_X_MISO_CLK_ENABLE();
    HWCON_PROBE_X_SCK_CLK_ENABLE();
    HWCON_PROBE_X_NSS_CLK_ENABLE();
    HWCON_PROBE_X_DRDY_CLK_ENABLE();

    /* --  Probe Y axis ----------------------------------------------------- */
    HWCON_PROBE_Y_SPI_CLK_ENABLE();
    HWCON_PROBE_Y_MOSI_CLK_ENABLE();
    HWCON_PROBE_Y_MISO_CLK_ENABLE();
    HWCON_PROBE_Y_SCK_CLK_ENABLE();
    HWCON_PROBE_Y_NSS_CLK_ENABLE();
    HWCON_PROBE_Y_DRDY_CLK_ENABLE();

    /* --  Probe Z axis ----------------------------------------------------- */
    HWCON_PROBE_Z_SPI_CLK_ENABLE();
    HWCON_PROBE_Z_MOSI_CLK_ENABLE();
    HWCON_PROBE_Z_MISO_CLK_ENABLE();
    HWCON_PROBE_Z_SCK_CLK_ENABLE();
    HWCON_PROBE_Z_NSS_CLK_ENABLE();
    HWCON_PROBE_Z_DRDY_CLK_ENABLE();

    /* --  CDI CTRL bus ----------------------------------------------------- */
    HWCON_CTRL_I2C_CLK_ENABLE();
    HWCON_CTRL_SCL_CLK_ENABLE();
    HWCON_CTRL_SDA_CLK_ENABLE();

    /* -- AUX --------------------------------------------------------------- */
    HWCON_AUX_SPI_CLK_ENABLE();
    HWCON_AUX_MOSI_CLK_ENABLE();
    HWCON_AUX_MISO_CLK_ENABLE();
    HWCON_AUX_SCK_CLK_ENABLE();
    HWCON_AUX_NSS_CLK_ENABLE();
    HWCON_AUX_DRDY_CLK_ENABLE();
#if defined(HWCON_TEST_TIMER0_ENABLE)
    /* --  TEST TIMER0  ----------------------------------------------------- */
    HWCON_TEST_TIMER0_CLK_ENABLE();
#endif
}

void psm_init_gpio(void)
{
    const struct gpio_setup *   current_setup;

    for (current_setup = g_gpio_pins; current_setup->port; current_setup++) {
        GPIO_InitTypeDef            pin_config;

        memset(&pin_config, 0, sizeof(pin_config));
        pin_config.Mode         = current_setup->mode;
        pin_config.Pin          = current_setup->pin;
        pin_config.Pull         = current_setup->pull;
        pin_config.Speed        = GPIO_SPEED_HIGH;
        pin_config.Alternate    = current_setup->func;
        HAL_GPIO_Init(current_setup->port, &pin_config);

        if ((current_setup->mode == GPIO_MODE_OUTPUT_OD) ||
            (current_setup->mode == GPIO_MODE_OUTPUT_PP)) {
            HAL_GPIO_WritePin(current_setup->port, current_setup->pin,
                    current_setup->default_state);
        }
    }
}

void psm_init_exti(void)
{
    /* --  Probe X axis DRDY pin  ------------------------------------------- */
    HAL_NVIC_SetPriority(HWCON_PROBE_X_DRDY_EXTI, HWCON_IRQ_PRIO_PROBE_EXTI, 0);

    /* --  Probe Y axis DRDY pin  ------------------------------------------- */
    HAL_NVIC_SetPriority(HWCON_PROBE_Y_DRDY_EXTI, HWCON_IRQ_PRIO_PROBE_EXTI, 0);

    /* --  Probe Z axis DRDY pin  ------------------------------------------- */
    HAL_NVIC_SetPriority(HWCON_PROBE_Z_DRDY_EXTI, HWCON_IRQ_PRIO_PROBE_EXTI, 0);

    /* --  AUX DRDY pin  ---------------------------------------------------- */
    HAL_NVIC_SetPriority(HWCON_AUX_DRDY_EXTI, HWCON_IRQ_PRIO_AUX_EXTI, 0);
}

void psm_init_spi(void)
{
    init_spi_rtcomm();
    init_spi_probe();
    init_spi_aux();
}

void psm_init_i2c(void)
{
    init_i2c_ctrl();
}

void psm_init_timer(void)
{
    init__test_timer0();
}

void psm_reinit_i2c_ctrl(void)
{
    deinit_i2c_ctrl();
    init_i2c_ctrl();
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of psp_.c
 ******************************************************************************/
