
#include <test_timer0.h>
#include "stm32f4xx_hal.h"
#include "hwcon.h"

TIM_HandleTypeDef        g_test_timer0;


void test_timer0_disable(void)
{
	HAL_TIM_Base_Stop_IT(&g_test_timer0);
}



void test_timer0_enable(void)
{
	HAL_TIM_Base_Start_IT(&g_test_timer0);
}



