
#include <string.h>

#include "stm32f4xx_hal.h"
#include "hwcon.h"
#include "status.h"
#include "rtcomm.h"
#include "prim_spi.h"

#if defined(HWCON_TEST_TIMER0_ENABLE)
#include "test_timer0.h"
#endif

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
static
void setup_clock(void)
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
	RCC_OscInitStruct.HSEState 	     = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState       = RCC_LSE_OFF;
	RCC_OscInitStruct.HSIState	     = RCC_HSI_OFF;
	RCC_OscInitStruct.LSIState       = RCC_LSI_OFF;
	RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM       = 8;
	RCC_OscInitStruct.PLL.PLLN       = 192;
	RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ       = 4;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		status_panic(STATUS_HW_INIT_FAILED);
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
		status_panic(STATUS_HW_INIT_FAILED);
	}

    /* --  Notification GPIO  ----------------------------------------------- */
    HWCON_NOTIFY_CLK_ENABLE();

    /* --  Heartbeat LED GPIO  ---------------------------------------------- */
    HWCON_HEARTBEAT_CLK_ENABLE();

    /* --  SYNC GPIO  ------------------------------------------------------- */
    HWCON_SYNC_CLK_ENABLE();

    /* --  Protocol status LED GPIO  ---------------------------------------- */
    HWCON_PROTOCOL_LED_CLK_ENABLE();

    /* --  Trigger GPIO OUT ------------------------------------------------- */
    HWCON_TRIGGER_OUT_CLK_ENABLE();

    /* --  Trigger GPIO IN  ------------------------------------------------- */
    HWCON_TRIGGER_IN_CLK_ENABLE();

    /* --  SYNC pin  -------------------------------------------------------- */
    HWCON_SYNC_CLK_ENABLE();

    /* --  RTCOMM: MOSI pin  ------------------------------------------------ */
    HWCON_RTCOMM_MOSI_CLK_ENABLE();

    /* --  RTCOMM: MISO pin  ------------------------------------------------ */
    HWCON_RTCOMM_MISO_CLK_ENABLE();

    /* --  RTCOMM: SCK pin  ------------------------------------------------- */
    HWCON_RTCOMM_SCK_CLK_ENABLE();

    /* --  RTCOMM: NSS pin  ------------------------------------------------- */
    HWCON_RTCOMM_NSS_CLK_ENABLE();

    /* --  RTCOMM SPI & DMA  ------------------------------------------------ */
    HWCON_RTCOMM_SPI_CLK_ENABLE();
    HWCON_RTCOMM_SPI_DMA_CLK_ENABLE();

    /* --  ACQ0: SPI  ------------------------------------------------------- */
    HWCON_ACQ_0_SPI_CLK_ENABLE();

    /* --  ACQ0: NSS pin  --------------------------------------------------- */
    HWCON_ACQ_0_SPI_NSS_CLK_ENABLE();

    /* --  ACQ0: MISO pin  -------------------------------------------------- */
    HWCON_ACQ_0_SPI_MISO_CLK_ENABLE();

    /* --  ACQ0: MOSI pin  -------------------------------------------------- */
    HWCON_ACQ_0_SPI_MOSI_CLK_ENABLE();

    /* --  ACQ0: SCK pin  --------------------------------------------------- */
    HWCON_ACQ_0_SPI_SCK_CLK_ENABLE();

    /* --  ACQ0: DRDY pin  -------------------------------------------------- */
    HWCON_ACQ_0_DRDY_CLK_ENABLE();

    /* --  ACQ1: SPI  ------------------------------------------------------- */
    HWCON_ACQ_1_SPI_CLK_ENABLE();

    /* --  ACQ1: NSS pin  --------------------------------------------------- */
    HWCON_ACQ_1_SPI_NSS_CLK_ENABLE();

    /* --  ACQ1: MISO pin  -------------------------------------------------- */
    HWCON_ACQ_1_SPI_MISO_CLK_ENABLE();

    /* --  ACQ1: MOSI pin  -------------------------------------------------- */
    HWCON_ACQ_1_SPI_MOSI_CLK_ENABLE();

    /* --  ACQ0: SCK pin  --------------------------------------------------- */
    HWCON_ACQ_1_SPI_SCK_CLK_ENABLE();

    /* --  ACQ1: DRDY pin  -------------------------------------------------- */
    HWCON_ACQ_1_DRDY_CLK_ENABLE();

    /* --  ACQ2: SPI  ------------------------------------------------------- */
    HWCON_ACQ_2_SPI_CLK_ENABLE();

    /* --  ACQ1: NSS pin  --------------------------------------------------- */
    HWCON_ACQ_2_SPI_NSS_CLK_ENABLE();

    /* --  ACQ2: MISO pin  -------------------------------------------------- */
    HWCON_ACQ_2_SPI_MISO_CLK_ENABLE();

    /* --  ACQ1: MOSI pin  -------------------------------------------------- */
    HWCON_ACQ_2_SPI_MOSI_CLK_ENABLE();

    /* --  ACQ2: SCK pin  --------------------------------------------------- */
    HWCON_ACQ_2_SPI_SCK_CLK_ENABLE();

    /* --  ACQ2: DRDY pin  -------------------------------------------------- */
    HWCON_ACQ_2_DRDY_CLK_ENABLE();
#if defined(HWCON_TEST_TIMER0_ENABLE)

    /* --  TEST TIMER0  ----------------------------------------------------- */
    HWCON_TEST_TIMER0_CLK_ENABLE();
#endif
}

static
void reset_config(GPIO_InitTypeDef * config)
{
	memset(config, 0, sizeof(*config));
}

static
void setup_gpio(void)
{
	GPIO_InitTypeDef 			pin_config;

    /* --  Notification GPIO  ----------------------------------------------- */
	reset_config(&pin_config);
	pin_config.Mode  = GPIO_MODE_OUTPUT_PP;
	pin_config.Pin   = HWCON_NOTIFY_PIN;
	pin_config.Pull  = GPIO_NOPULL;
	pin_config.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(HWCON_NOTIFY_PORT, &pin_config);
	HAL_GPIO_WritePin(HWCON_NOTIFY_PORT, HWCON_NOTIFY_PIN, GPIO_PIN_RESET);

    /* --  Heartbeat LED GPIO  ---------------------------------------------- */
	reset_config(&pin_config);
	pin_config.Mode  = GPIO_MODE_OUTPUT_PP;
	pin_config.Pin   = HWCON_HEARTBEAT_PIN;
	pin_config.Pull  = GPIO_NOPULL;
	pin_config.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(HWCON_HEARTBEAT_PORT, &pin_config);
	HAL_GPIO_WritePin(HWCON_HEARTBEAT_PORT, HWCON_HEARTBEAT_PIN,
			GPIO_PIN_RESET);

    /* --  SYNC GPIO  ------------------------------------------------------- */
	reset_config(&pin_config);
	pin_config.Mode  = GPIO_MODE_OUTPUT_PP;
	pin_config.Pin   = HWCON_SYNC_PIN;
	pin_config.Pull  = GPIO_NOPULL;
	pin_config.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(HWCON_SYNC_PORT, &pin_config);
	HAL_GPIO_WritePin(HWCON_SYNC_PORT, HWCON_SYNC_PIN, GPIO_PIN_RESET);

    /* --  Protocol status LED GPIO  ---------------------------------------- */
	reset_config(&pin_config);
	pin_config.Mode  = GPIO_MODE_OUTPUT_PP;
	pin_config.Pin   = HWCON_PROTOCOL_LED_PIN;
	pin_config.Pull  = GPIO_NOPULL;
	pin_config.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(HWCON_PROTOCOL_LED_PORT, &pin_config);
	HAL_GPIO_WritePin(HWCON_PROTOCOL_LED_PORT, HWCON_PROTOCOL_LED_PIN,
			GPIO_PIN_RESET);

    /* --  Trigger GPIO OUT ------------------------------------------------- */
	reset_config(&pin_config);
	pin_config.Mode  = GPIO_MODE_OUTPUT_PP;
	pin_config.Pin   = HWCON_TRIGGER_OUT_PIN;
	pin_config.Pull  = GPIO_NOPULL;
	pin_config.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(HWCON_TRIGGER_OUT_PORT, &pin_config);
	HAL_GPIO_WritePin(HWCON_TRIGGER_OUT_PORT, HWCON_TRIGGER_OUT_PIN,
			GPIO_PIN_RESET);

	/* --  Trigger GPIO IN  ------------------------------------------------- */
	reset_config(&pin_config);
	pin_config.Mode  = GPIO_MODE_INPUT;
	pin_config.Pin   = HWCON_TRIGGER_IN_PIN;
	pin_config.Pull  = GPIO_NOPULL;
	pin_config.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(HWCON_TRIGGER_IN_PORT, &pin_config);

	/* --  SYNC pin  -------------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Mode  = GPIO_MODE_OUTPUT_PP;
    pin_config.Pin   = HWCON_SYNC_PIN;
    pin_config.Pull  = GPIO_NOPULL;
    pin_config.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(HWCON_SYNC_PORT, &pin_config);
    HAL_GPIO_WritePin(HWCON_SYNC_PORT, HWCON_SYNC_PIN, GPIO_PIN_RESET);
    /*
     * TODO: move this stuff to some other place
     */
    HAL_Delay(10);
    HAL_GPIO_WritePin(HWCON_SYNC_PORT, HWCON_SYNC_PIN, GPIO_PIN_SET);

    /* --  RTCOMM: MOSI pin  ------------------------------------------------ */
    reset_config(&pin_config);
    pin_config.Pin          = HWCON_RTCOMM_MOSI_PIN;
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pull         = GPIO_PULLUP;
    pin_config.Speed        = GPIO_SPEED_FAST;
    pin_config.Alternate    = HWCON_RTCOMM_MOSI_AF;
    HAL_GPIO_Init(HWCON_RTCOMM_MOSI_PORT, &pin_config);

    /* --  RTCOMM: MISO pin  ------------------------------------------------ */
    reset_config(&pin_config);
    pin_config.Pin          = HWCON_RTCOMM_MISO_PIN;
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pull         = GPIO_PULLUP;
    pin_config.Speed        = GPIO_SPEED_FAST;
    pin_config.Alternate    = HWCON_RTCOMM_MISO_AF;
    HAL_GPIO_Init(HWCON_RTCOMM_MISO_PORT, &pin_config);

    /* --  RTCOMM: SCK pin  ------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Pin          = HWCON_RTCOMM_SCK_PIN;
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pull         = GPIO_PULLUP;
    pin_config.Speed        = GPIO_SPEED_FAST;
    pin_config.Alternate    = HWCON_RTCOMM_SCK_AF;
    HAL_GPIO_Init(HWCON_RTCOMM_SCK_PORT, &pin_config);

    /* --  RTCOMM: NSS pin  ------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = HWCON_RTCOMM_NSS_PIN;
    pin_config.Pull         = GPIO_PULLUP;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = HWCON_RTCOMM_NSS_AF;
    HAL_GPIO_Init(HWCON_RTCOMM_NSS_PORT, &pin_config);

	/* --  ACQ0: NSS pin  --------------------------------------------------- */
	reset_config(&pin_config);
	pin_config.Mode  = GPIO_MODE_OUTPUT_PP;
	pin_config.Pin   = HWCON_ACQ_0_SPI_NSS_PIN;
	pin_config.Pull  = GPIO_NOPULL;
	pin_config.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(HWCON_ACQ_0_SPI_NSS_PORT, &pin_config);

	/* --  ACQ0: MISO pin  -------------------------------------------------- */
	reset_config(&pin_config);
	pin_config.Mode  = GPIO_MODE_AF_PP;
	pin_config.Pin   = HWCON_ACQ_0_SPI_MISO_PIN;
	pin_config.Pull  = GPIO_PULLDOWN;
	pin_config.Speed = GPIO_SPEED_HIGH;
	pin_config.Alternate = HWCON_ACQ_0_SPI_MISO_AF;
	HAL_GPIO_Init(HWCON_ACQ_0_SPI_MISO_PORT, &pin_config);

	/* --  ACQ0: MOSI pin  -------------------------------------------------- */
	reset_config(&pin_config);
	pin_config.Mode  = GPIO_MODE_AF_PP;
	pin_config.Pin   = HWCON_ACQ_0_SPI_MOSI_PIN;
	pin_config.Pull  = GPIO_PULLDOWN;
	pin_config.Speed = GPIO_SPEED_HIGH;
	pin_config.Alternate = HWCON_ACQ_0_SPI_MOSI_AF;
	HAL_GPIO_Init(HWCON_ACQ_0_SPI_MOSI_PORT, &pin_config);

	/* --  ACQ0: SCK pin  --------------------------------------------------- */
	reset_config(&pin_config);
	pin_config.Mode  = GPIO_MODE_AF_PP;
	pin_config.Pin   = HWCON_ACQ_0_SPI_SCK_PIN;
	pin_config.Pull  = GPIO_PULLDOWN;
	pin_config.Speed = GPIO_SPEED_HIGH;
	pin_config.Alternate = HWCON_ACQ_0_SPI_SCK_AF;
	HAL_GPIO_Init(HWCON_ACQ_0_SPI_SCK_PORT, &pin_config);

	/* --  ACQ0: DRDY pin  -------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Mode         = GPIO_MODE_IT_FALLING;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Pin          = HWCON_ACQ_0_DRDY_PIN;
    HAL_GPIO_Init(HWCON_ACQ_0_DRDY_PORT, &pin_config);

    /* --  ACQ1: NSS pin  --------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Mode         = GPIO_MODE_OUTPUT_PP;
    pin_config.Pin          = HWCON_ACQ_1_SPI_NSS_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(HWCON_ACQ_1_SPI_NSS_PORT, &pin_config);

    /* --  ACQ1: MISO pin  -------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = HWCON_ACQ_1_SPI_MISO_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = HWCON_ACQ_1_SPI_MISO_AF;
    HAL_GPIO_Init(HWCON_ACQ_1_SPI_MISO_PORT, &pin_config);

    /* --  ACQ1: MOSI pin  -------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = HWCON_ACQ_1_SPI_MOSI_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = HWCON_ACQ_1_SPI_MOSI_AF;
    HAL_GPIO_Init(HWCON_ACQ_1_SPI_MOSI_PORT, &pin_config);

    /* --  ACQ1: SCK pin  --------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = HWCON_ACQ_1_SPI_SCK_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = HWCON_ACQ_1_SPI_SCK_AF;
    HAL_GPIO_Init(HWCON_ACQ_1_SPI_SCK_PORT, &pin_config);

    /* --  ACQ1: DRDY pin  -------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Mode         = GPIO_MODE_IT_FALLING;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Pin          = HWCON_ACQ_1_DRDY_PIN;
    HAL_GPIO_Init(HWCON_ACQ_1_DRDY_PORT, &pin_config);

    /* --  ACQ2: NSS pin  --------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Mode         = GPIO_MODE_OUTPUT_PP;
    pin_config.Pin          = HWCON_ACQ_2_SPI_NSS_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(HWCON_ACQ_2_SPI_NSS_PORT, &pin_config);

    /* --  ACQ2: MISO pin  -------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = HWCON_ACQ_2_SPI_MISO_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = HWCON_ACQ_2_SPI_MISO_AF;
    HAL_GPIO_Init(HWCON_ACQ_2_SPI_MISO_PORT, &pin_config);

    /* --  ACQ2: MOSI pin  -------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = HWCON_ACQ_2_SPI_MOSI_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = HWCON_ACQ_2_SPI_MOSI_AF;
    HAL_GPIO_Init(HWCON_ACQ_2_SPI_MOSI_PORT, &pin_config);

    /* --  ACQ2: SCK pin  --------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Mode         = GPIO_MODE_AF_PP;
    pin_config.Pin          = HWCON_ACQ_2_SPI_SCK_PIN;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Speed        = GPIO_SPEED_HIGH;
    pin_config.Alternate    = HWCON_ACQ_2_SPI_SCK_AF;
    HAL_GPIO_Init(HWCON_ACQ_2_SPI_SCK_PORT, &pin_config);

    /* --  ACQ2: DRDY pin  -------------------------------------------------- */
    reset_config(&pin_config);
    pin_config.Mode         = GPIO_MODE_IT_FALLING;
    pin_config.Pull         = GPIO_NOPULL;
    pin_config.Pin          = HWCON_ACQ_2_DRDY_PIN;
    HAL_GPIO_Init(HWCON_ACQ_2_DRDY_PORT, &pin_config);
}

static
void setup_exti(void)
{
    /* --  ACQ0: DRDY pin  -------------------------------------------------- */
    HAL_NVIC_SetPriority(HWCON_ACQ_0_DRDY_EXTI, HWCON_IRQ_PRIO_ACQ_x_EXTI, 0);

    /* --  ACQ1: DRDY pin  -------------------------------------------------- */
    HAL_NVIC_SetPriority(HWCON_ACQ_1_DRDY_EXTI, HWCON_IRQ_PRIO_ACQ_x_EXTI, 0);

    /* --  ACQ2: DRDY pin  -------------------------------------------------- */
    HAL_NVIC_SetPriority(HWCON_ACQ_2_DRDY_EXTI, HWCON_IRQ_PRIO_ACQ_x_EXTI, 0);
}

static
void setup_spi(void)
{
    struct spi_config config;

	/* NOTE:
	 * 1. Multiplexer configuration is set in setup_gpios()
	 * 2. DMA is set in HAL_SPI_MspInit()
	 * 3. Clocks and resets are set in setup_clock()
	 */
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
    HAL_SPI_Init(&g_rtcomm.spi);

    /* --  ACQ0: SPI  ------------------------------------------------------- */
    config.flags = SPI_TRANSFER_TX   | SPI_TRANSFER_RX   | SPI_CLK_POL_LOW   |
                   SPI_CLK_PHA_2EDGE | SPI_MODE_MS       | SPI_SS_SOFTWARE   |
                   SPI_DATA_8BIT;
    config.prescaler = HWCON_ACQ_0_SPI_BAUD_CLOCK;
    spi_bus_init(&HWCON_ACQ_0_SPI, &config);
    HAL_NVIC_SetPriority(HWCON_ACQ_0_SPI_IRQ, HWCON_IRQ_PRIO_ACQ_x_SPI, 0);
    HAL_NVIC_ClearPendingIRQ(HWCON_ACQ_0_SPI_IRQ);
    HAL_NVIC_EnableIRQ(HWCON_ACQ_0_SPI_IRQ);

    /* --  ACQ1: SPI  ------------------------------------------------------- */
    config.prescaler = HWCON_ACQ_1_SPI_BAUD_CLOCK;
    spi_bus_init(&HWCON_ACQ_1_SPI, &config);
    HAL_NVIC_SetPriority(HWCON_ACQ_1_SPI_IRQ, HWCON_IRQ_PRIO_ACQ_x_SPI, 0);
    HAL_NVIC_ClearPendingIRQ(HWCON_ACQ_1_SPI_IRQ);
    HAL_NVIC_EnableIRQ(HWCON_ACQ_1_SPI_IRQ);

    /* --  ACQ2: SPI  ------------------------------------------------------- */
    config.prescaler = HWCON_ACQ_2_SPI_BAUD_CLOCK;
    spi_bus_init(&HWCON_ACQ_2_SPI, &config);
    HAL_NVIC_SetPriority(HWCON_ACQ_2_SPI_IRQ, HWCON_IRQ_PRIO_ACQ_x_SPI, 0);
    HAL_NVIC_ClearPendingIRQ(HWCON_ACQ_2_SPI_IRQ);
    HAL_NVIC_EnableIRQ(HWCON_ACQ_2_SPI_IRQ);
}

static
void setup_i2c(void)
{

}

static
void setup_timer(void)
{
#if defined(HWCON_TEST_TIMER0_ENABLE)
	/*
	 * Setup TEST_TIMER0
	 */
	{
		RCC_ClkInitTypeDef    clkconfig;
		uint32_t              clk;
		uint32_t			  clk_divider = 0U;
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

		HAL_TIM_Base_Init(&g_test_timer0);
	}
#endif
}

void HAL_MspInit(void)
{
	setup_clock();
	setup_gpio();
	setup_exti();
	setup_spi();
	setup_i2c();
	setup_timer();
}



void HAL_MspDeInit(void)
{
    /* NOTE:
     * DeInit is not used for HAL
     */
}



void HAL_SPI_MspInit(SPI_HandleTypeDef * hspi)
{
	if (hspi == &g_rtcomm.spi) {
		/* DMA */
		g_rtcomm.dma_tx.Instance = HWCON_RTCOMM_SPI_DMA_TX_STREAM;
		g_rtcomm.dma_tx.Init.Channel = HWCON_RTCOMM_SPI_DMA_TX_CHANNEL;
		g_rtcomm.dma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		g_rtcomm.dma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		g_rtcomm.dma_tx.Init.MemInc	= DMA_MINC_ENABLE;
		g_rtcomm.dma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		g_rtcomm.dma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		g_rtcomm.dma_tx.Init.Mode = DMA_NORMAL;
		g_rtcomm.dma_tx.Init.Priority = DMA_PRIORITY_LOW;
		g_rtcomm.dma_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		g_rtcomm.dma_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		g_rtcomm.dma_tx.Init.MemBurst = DMA_MBURST_INC4;
		g_rtcomm.dma_tx.Init.PeriphBurst = DMA_PBURST_INC4;
		HAL_DMA_Init(&g_rtcomm.dma_tx);
		__HAL_LINKDMA(&g_rtcomm.spi, hdmatx, g_rtcomm.dma_tx);

		/* DMA IRQ */
		HAL_NVIC_SetPriority(HWCON_RTCOMM_SPI_DMA_TX_IRQn, HWCON_RTCOMM_SPI_DMA_TX_IRQ_PRIO, 0);
		HAL_NVIC_EnableIRQ(HWCON_RTCOMM_SPI_DMA_TX_IRQn);
	}
}



void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
    /* NOTE:
     * DeInit is not used for SPI
     */
	(void)hspi;
}



void HAL_TIM_Base_MspInit(TIM_HandleTypeDef * htim)
{
#if defined(HWCON_TEST_TIMER0_ENABLE)
	if (htim == &g_test_timer0) {

		/*Configure the IRQ priority */
		HAL_NVIC_SetPriority(HWCON_TEST_TIMER0_IRQn, HWCON_TEST_TIMER0_IRQ_PRIO,
				0U);

		/* Enable the global Interrupt */
		HAL_NVIC_EnableIRQ(HWCON_TEST_TIMER0_IRQn);
	}
#endif
}



void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef * htim)
{
    (void)htim;
}
