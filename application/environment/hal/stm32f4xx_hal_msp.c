
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_msp.h"
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
void HAL_MspInit(void)
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

    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        status_panic(10);
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
    RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        status_panic(10);
    }
}



void HAL_MspDeInit(void)
{
    /* NOTE:
     * DeInit is not used for HAL
     */
}



/**
  * @brief  Initializes the PPP MSP.
  * @note   This functiona is called from HAL_PPP_Init() function to perform
  *         peripheral(PPP) system level initialization (GPIOs, clock, DMA, interrupt)
  * @retval None
  */
void HAL_PPP_MspInit(void)
{

}

/**
  * @brief  DeInitializes the PPP MSP.
  * @note   This functiona is called from HAL_PPP_DeInit() function to perform
  *         peripheral(PPP) system level de-initialization (GPIOs, clock, DMA, interrupt)
  * @retval None
  */
void HAL_PPP_MspDeInit(void)
{

}



void HAL_SPI_MspInit(SPI_HandleTypeDef * hspi)
{
    /* NOTE:
     * Since this is the only SPI we ignore the handle
     */
	  GPIO_InitTypeDef			pin_config;
    (void)hspi;
    

    SPI_MS_MOSI_GPIO_ENABLE();
    SPI_MS_MISO_GPIO_ENABLE();
    SPI_MS_SCK_GPIO_ENABLE();
    SPI_MS_NSS_GPIO_ENABLE();
    SPI_MS_CLK_ENABLE();
    SPI_MS_DMA_CLK_ENABLE();

    /* MOSI */
    pin_config.Pin			= SPI_MS_MOSI_PIN;
    pin_config.Mode			= GPIO_MODE_AF_PP;
    pin_config.Pull			= GPIO_PULLUP;
    pin_config.Speed		= GPIO_SPEED_FAST;
    pin_config.Alternate    = SPI_MS_MOSI_AF;
    HAL_GPIO_Init(SPI_MS_MOSI_GPIO_PORT, &pin_config);

    /* MISO */
    pin_config.Pin			= SPI_MS_MISO_PIN;
    pin_config.Mode			= GPIO_MODE_AF_PP;
    pin_config.Pull			= GPIO_PULLUP;
    pin_config.Speed		= GPIO_SPEED_FAST;
    pin_config.Alternate    = SPI_MS_MISO_AF;
    HAL_GPIO_Init(SPI_MS_MISO_GPIO_PORT, &pin_config);

    /* SCK */
    pin_config.Pin			= SPI_MS_SCK_PIN;
    pin_config.Mode			= GPIO_MODE_AF_PP;
    pin_config.Pull			= GPIO_PULLUP;
    pin_config.Speed		= GPIO_SPEED_FAST;
    pin_config.Alternate    = SPI_MS_SCK_AF;
    HAL_GPIO_Init(SPI_MS_SCK_GPIO_PORT, &pin_config);

    /* NSS */
    pin_config.Mode  		= GPIO_MODE_AF_PP;
    pin_config.Pin   		= SPI_MS_NSS_PIN;
    pin_config.Pull  		= GPIO_PULLUP;
    pin_config.Speed 		= GPIO_SPEED_HIGH;
    pin_config.Alternate	= SPI_MS_NSS_AF;
    HAL_GPIO_Init(SPI_MS_NSS_GPIO_PORT, &pin_config);

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



void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
    /* NOTE:
     * DeInit is not used for SPI
     */
	(void)hspi;
}



void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
	GPIO_InitTypeDef			pin_config;

	(void)hi2c;

	/* SCL */
	I2C_CTRL_SCL_GPIO_CLK_ENABLE();
	pin_config.Mode 		= GPIO_MODE_AF_OD;
	pin_config.Pull 		= GPIO_PULLUP;
	pin_config.Speed 		= GPIO_SPEED_FAST;
	pin_config.Pin  		= I2C_CTRL_SCL_PIN;
	pin_config.Alternate	= I2C_CTRL_SCL_AF;
	HAL_GPIO_Init(I2C_CTRL_SCL_GPIO_PORT, &pin_config);

	/* SDA */
	I2C_CTRL_SDA_GPIO_CLK_ENABLE();
	pin_config.Mode  		= GPIO_MODE_AF_OD;
	pin_config.Pull  		= GPIO_PULLUP;
	pin_config.Speed 		= GPIO_SPEED_FAST;
	pin_config.Pin  		= I2C_CTRL_SDA_PIN;
	pin_config.Alternate	= I2C_CTRL_SDA_AF;
	HAL_GPIO_Init(I2C_CTRL_SDA_GPIO_PORT, &pin_config);

	I2C_CTRL_CLK_ENABLE();
	I2C_CTRL_FORCE_RESET();
	I2C_CTRL_RELEASE_RESET();

	HAL_NVIC_SetPriority(I2C_CTRL_ER_IRQn, IRQ_PRIO_I2C_CTRL, 0);
	HAL_NVIC_EnableIRQ(I2C_CTRL_ER_IRQn);
	HAL_NVIC_SetPriority(I2C_CTRL_EV_IRQn, IRQ_PRIO_I2C_CTRL, 0);
	HAL_NVIC_EnableIRQ(I2C_CTRL_EV_IRQn);
}



void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
	(void)hi2c;

    HAL_NVIC_DisableIRQ(I2C_CTRL_ER_IRQn);
    HAL_NVIC_DisableIRQ(I2C_CTRL_EV_IRQn);
}



void HAL_TIM_Base_MspInit(TIM_HandleTypeDef * htim)
{
    (void)htim;
}



void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef * htim)
{
    (void)htim;
}
