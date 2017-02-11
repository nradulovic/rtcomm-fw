
#include "stm32f4xx_hal.h"
#include "test_timer.h"

TIM_HandleTypeDef        g_test_timer;

static void (* g_test_timer_callback)(void);

HAL_StatusTypeDef test_timer_init(uint32_t TickPriority, void (* callback)(void))
{
	RCC_ClkInitTypeDef    clkconfig;
	uint32_t              uwTimclock, uwAPB1Prescaler = 0U;
	uint32_t              uwPrescalerValue = 0U;
	uint32_t              pFLatency;

	g_test_timer_callback = callback;

	/*Configure the TIM6 IRQ priority */
	HAL_NVIC_SetPriority(TIM2_IRQn, TickPriority ,0U);

	/* Enable the TIM6 global Interrupt */
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	/* Enable TIM6 clock */
	__HAL_RCC_TIM2_CLK_ENABLE();

	/* Get clock configuration */
	HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

	/* Get APB1 prescaler */
	uwAPB1Prescaler = clkconfig.APB1CLKDivider;

	/* Compute TIM6 clock */
	if (uwAPB1Prescaler == RCC_HCLK_DIV1) {
		uwTimclock = HAL_RCC_GetPCLK1Freq();
	} else {
		uwTimclock = 2*HAL_RCC_GetPCLK1Freq();
	}

	/* Compute the prescaler value to have TIM6 counter clock equal to 1MHz */
	uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000U) - 1U);

	/* Initialize TIM6 */
	g_test_timer.Instance = TIM2;

	/* Initialize TIMx peripheral as follow:
	+ Period = [(TIM6CLK/1000) - 1]. to have a (1/1000) s time base.
	+ Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
	+ ClockDivision = 0
	+ Counter direction = Up
	*/
	g_test_timer.Init.Period = (1000000U / 1000U) - 1U;
	g_test_timer.Init.Prescaler = uwPrescalerValue;
	g_test_timer.Init.ClockDivision = 0;
	g_test_timer.Init.CounterMode = TIM_COUNTERMODE_UP;

	if (HAL_TIM_Base_Init(&g_test_timer) == HAL_OK) {
		/* Start the TIM time Base generation in interrupt mode */
		return HAL_TIM_Base_Start_IT(&g_test_timer);
	}

	/* Return function status */
	return HAL_ERROR;
}



void test_timer_disable(void)
{
  /* Disable TIM6 update Interrupt */
  __HAL_TIM_DISABLE_IT(&g_test_timer, TIM_IT_UPDATE);
}



void test_timer_enable(void)
{
  /* Enable TIM6 Update interrupt */
  __HAL_TIM_ENABLE_IT(&g_test_timer, TIM_IT_UPDATE);
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    g_test_timer_callback();
}



