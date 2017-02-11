
#include <string.h>

#include "stm32f4xx_hal.h"
#include "hw_config.h"
#include "app_stat.h"

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

	/* The voltage scaling allows optimizing the power consumption when the device is
	 clocked below the maximum system frequency, to update the voltage scaling value
	 regarding system frequency refer to product datasheet.  */
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
		status_panic(10);
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK |
			RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
		status_panic(10);
	}
}

static
void reset_config(GPIO_InitTypeDef * config)
{
	memset(&config, 0, sizeof(*config));
}

static
void setup_gpios(void)
{
	GPIO_InitTypeDef 			pin_config;

	/*
	 * Notify pin
	 */
	NOTIFY_GPIO_CLK_ENABLE();
	reset_config(&pin_config);
	pin_config.Mode  = GPIO_MODE_OUTPUT_PP;
	pin_config.Pin   = NOTIFY_PIN;
	pin_config.Pull  = GPIO_NOPULL;
	pin_config.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(NOTIFY_PORT, &pin_config);
	HAL_GPIO_WritePin(NOTIFY_PORT, NOTIFY_PIN, GPIO_PIN_RESET);

	/*
	 * spi_ms: MOSI pin
	 */
	SPI_MS_MOSI_GPIO_CLK_ENABLE();
	reset_config(&pin_config);
    pin_config.Pin			= SPI_MS_MOSI_PIN;
    pin_config.Mode			= GPIO_MODE_AF_PP;
    pin_config.Pull			= GPIO_PULLUP;
    pin_config.Speed		= GPIO_SPEED_FAST;
    pin_config.Alternate    = SPI_MS_MOSI_AF;
    HAL_GPIO_Init(SPI_MS_MOSI_GPIO_PORT, &pin_config);


	/*
	 * spi_ms: MISO pin
	 */
	SPI_MS_MISO_GPIO_CLK_ENABLE();
	reset_config(&pin_config);
	pin_config.Pin			= SPI_MS_MISO_PIN;
	pin_config.Mode			= GPIO_MODE_AF_PP;
	pin_config.Pull			= GPIO_PULLUP;
	pin_config.Speed		= GPIO_SPEED_FAST;
	pin_config.Alternate    = SPI_MS_MISO_AF;
	HAL_GPIO_Init(SPI_MS_MISO_GPIO_PORT, &pin_config);

	/*
	 * spi_ms: SCK pin
	 */
	SPI_MS_SCK_GPIO_CLK_ENABLE();
	reset_config(&pin_config);
	pin_config.Pin			= SPI_MS_SCK_PIN;
	pin_config.Mode			= GPIO_MODE_AF_PP;
	pin_config.Pull			= GPIO_PULLUP;
	pin_config.Speed		= GPIO_SPEED_FAST;
	pin_config.Alternate    = SPI_MS_SCK_AF;
	HAL_GPIO_Init(SPI_MS_SCK_GPIO_PORT, &pin_config);

	/*
	 * spi_ms: nss pin
	 */
	SPI_MS_NSS_GPIO_CLK_ENABLE();
	reset_config(&pin_config);
	pin_config.Mode  		= GPIO_MODE_AF_PP;
	pin_config.Pin   		= SPI_MS_NSS_PIN;
	pin_config.Pull  		= GPIO_PULLUP;
	pin_config.Speed 		= GPIO_SPEED_HIGH;
	pin_config.Alternate	= SPI_MS_NSS_AF;
	HAL_GPIO_Init(SPI_MS_NSS_GPIO_PORT, &pin_config);
}

void HAL_MspInit(void)
{
	setup_clock();
	setup_gpios();

}



void HAL_MspDeInit(void)
{
    /* NOTE:
     * DeInit is not used for HAL
     */
}



void HAL_SPI_MspInit(SPI_HandleTypeDef * hspi)
{
	extern SPI_HandleTypeDef g_ms_spi_handle;

	if (hspi == &g_ms_spi_handle) {
		extern DMA_HandleTypeDef g_ms_spi_dma_tx_handle;

		SPI_MS_CLK_ENABLE();
		SPI_MS_DMA_CLK_ENABLE();

		/* DMA */
		g_ms_spi_dma_tx_handle.Instance					= SPI_MS_TX_DMA_STREAM;
		g_ms_spi_dma_tx_handle.Init.Channel				= SPI_MS_TX_DMA_CHANNEL;
		g_ms_spi_dma_tx_handle.Init.Direction			= DMA_MEMORY_TO_PERIPH;
		g_ms_spi_dma_tx_handle.Init.PeriphInc   		= DMA_PINC_DISABLE;
		g_ms_spi_dma_tx_handle.Init.MemInc				= DMA_MINC_ENABLE;
		g_ms_spi_dma_tx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		g_ms_spi_dma_tx_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
		g_ms_spi_dma_tx_handle.Init.Mode                = DMA_NORMAL;
		g_ms_spi_dma_tx_handle.Init.Priority            = DMA_PRIORITY_LOW;
		g_ms_spi_dma_tx_handle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
		g_ms_spi_dma_tx_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
		g_ms_spi_dma_tx_handle.Init.MemBurst            = DMA_MBURST_INC4;
		g_ms_spi_dma_tx_handle.Init.PeriphBurst         = DMA_PBURST_INC4;
		HAL_DMA_Init(&g_ms_spi_dma_tx_handle);
		__HAL_LINKDMA(&g_ms_spi_handle, hdmatx, g_ms_spi_dma_tx_handle);

		/* DMA IRQ */
		HAL_NVIC_SetPriority(SPI_MS_DMA_TX_IRQn, IRQ_PRIO_SPI_MS_DMA, 0);
		HAL_NVIC_EnableIRQ(SPI_MS_DMA_TX_IRQn);
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
    (void)htim;
}



void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef * htim)
{
    (void)htim;
}
