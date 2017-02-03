/*
 * trigger.c
 *
 *  Created on: Jun 15, 2015
 *      Author: nenad
 */

#include <stdbool.h>

#include "hw_config.h"
#include "trigger.h"
#include "prim_gpio.h"
#include "cdi/io.h"

#define trigger_out_set()               gpio_set(TRIGGER_OUT_PORT, TRIGGER_OUT_PIN)
#define trigger_out_clr()               gpio_clr(TRIGGER_OUT_PORT, TRIGGER_OUT_PIN)

struct trigger_handle
{
    TIM_HandleTypeDef           in_pt;
    bool                     (* in_handler)(void);
    bool                        in_should_stop;
    TIM_HandleTypeDef           out_pt;
    void                     (* out_start)(void);
    void                     (* out_stop)(void);
};


static struct trigger_handle    g_trigger;


static void simple_start(void)
{
    trigger_out_set();
}



static void simple_stop(void)
{
    trigger_out_clr();
}



static void timed_start(void)
{
    trigger_out_set();
    TIM_Base_SetConfig(g_trigger.out_pt.Instance, &g_trigger.out_pt.Init);
    __HAL_TIM_CLEAR_IT(&g_trigger.out_pt, TIM_IT_UPDATE);
    __HAL_TIM_ENABLE_IT(&g_trigger.out_pt, TIM_IT_UPDATE);
    __HAL_TIM_ENABLE(&g_trigger.out_pt);
}



static void timed_stop(void)
{
    return;
}



static void empty_out_handler(void)
{
	return;
}


static bool empty_in_handler(void)
{
	return (false);
}



void trigger_out_init(void)
{
    GPIO_InitTypeDef            pin_config;

    TRIGGER_OUT_TIM_CLK_ENABLE();
    TRIGGER_OUT_CLK_ENABLE();
    pin_config.Mode  = GPIO_MODE_OUTPUT_PP;
    pin_config.Pin   = TRIGGER_OUT_PIN;
    pin_config.Pull  = GPIO_NOPULL;
    pin_config.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(TRIGGER_OUT_PORT, &pin_config);
    trigger_out_clr();
    g_trigger.out_start  = empty_out_handler;
    g_trigger.out_stop   = empty_out_handler;
    g_trigger.in_handler = empty_in_handler;
}



void trigger_out_set_speed(uint32_t speed)
{
    uint32_t                    period;

    switch (speed) {
		case ADS_DRATE_2_5SPS:
			period = 200000u;
			break;
		case ADS_DRATE_5SPS:
			period = 100000u;
			break;
		case ADS_DRATE_10SPS:
			period = 50000u;
			break;
		case ADS_DRATE_15SPS:
			period = 33333u;
			break;
		case ADS_DRATE_25SPS:
			period = 20000u;
			break;
		case ADS_DRATE_30SPS:
			period = 16667u;
			break;
		case ADS_DRATE_50SPS:
			period = 10000u;
			break;
		case ADS_DRATE_60SPS:
			period = 8333u;
			break;
		case ADS_DRATE_100SPS:
			period = 5000u;
			break;
		case ADS_DRATE_500SPS:
			period = 1000u;
			break;
		case ADS_DRATE_1000SPS:
			period = 500u;
			break;
		case ADS_DRATE_2000SPS:
			period = 250u;
			break;
		case ADS_DRATE_3750SPS:
			period = 133u;
			break;
		case ADS_DRATE_7500SPS:
			period = 66u;
			break;
		case ADS_DRATE_15KSPS:
			period = 33u;
			break;
		case ADS_DRATE_30KSPS:
			period = 0u;
			break;
		default:
			period = 0u;
	}

    if (period) {
        uint32_t                prescaler;

        g_trigger.out_start = timed_start;
        g_trigger.out_stop  = timed_stop;

        /* NOTE:
         * Timer regiters are 16-bit wide. In order to prevent overflow when
         * using large delay compute the prescaler to a less accurate value.
         */
        if (period > UINT16_MAX) {
            /* NOTE:
             * Compute the prescaler value to have counter clock equal to 100kHz
             */
            prescaler = (SystemCoreClock / 1u) / 100000u;
            period   /= 10ul;
        } else {
            /* NOTE:
             * Compute the prescaler value to have counter clock equal to 1MHz
             */
            prescaler = (SystemCoreClock / 1u) / 1000000u;
        }

        /* Initialize timer peripheral as follows:
         *  Period            = p - 1
         *  Prescaler         = ((SystemCoreClock / 2) / 1000000) - 1
         *  ClockDivision     = 0
         *  Counter direction = Up
         */
        g_trigger.out_pt.Instance           = TRIGGER_OUT_TIM_INSTANCE;
        g_trigger.out_pt.Init.Period        = period    - 1u;
        g_trigger.out_pt.Init.Prescaler     = prescaler - 1u;
        g_trigger.out_pt.Init.ClockDivision = 0;
        g_trigger.out_pt.Init.CounterMode   = TIM_COUNTERMODE_UP;
        HAL_NVIC_SetPriority(TRIGGER_OUT_TIM_IRQn, IRQ_PRIO_TRIGGER_OUT_TIM, 0);
        NVIC_EnableIRQ(TRIGGER_OUT_TIM_IRQn);
    } else {
        g_trigger.out_start = simple_start;
        g_trigger.out_stop  = simple_stop;
    }
}



void trigger_out_enable(void)
{
    g_trigger.out_start();
}



void trigger_out_conditional_disable(void)
{
    g_trigger.out_stop();
}



void trigger_out_disable(void)
{
	trigger_out_clr();
	__HAL_TIM_CLEAR_IT(&g_trigger.out_pt, TIM_IT_UPDATE);
	__HAL_TIM_DISABLE_IT(&g_trigger.out_pt, TIM_IT_UPDATE);
	__HAL_TIM_DISABLE(&g_trigger.out_pt);

    g_trigger.out_start  = empty_out_handler;
    g_trigger.out_stop   = empty_out_handler;
    g_trigger.in_handler = empty_in_handler;
}



void trigger_in_init(void)
{
    uint32_t                    period;
    uint32_t                    prescaler;
    GPIO_InitTypeDef            pin_config;

    TRIGGER_IN_TIM_CLK_ENABLE();
    TRIGGER_IN_CLK_ENABLE();

    period = 100u;

    /* NOTE:
     * Compute the prescaler value to have counter clock equal to 1MHz
     */
    prescaler = (SystemCoreClock / 1u) / 1000000u;

    /* NOTE:
     * Initialize timer peripheral as follows:
     *  Period            = p - 1
     *  Prescaler         = ((SystemCoreClock / 2) / 1000000) - 1
     *  ClockDivision     = 0
     *  Counter direction = Up
     */
    g_trigger.in_pt.Instance           = TRIGGER_IN_TIM_INSTANCE;
    g_trigger.in_pt.Init.Period        = period    - 1u;
    g_trigger.in_pt.Init.Prescaler     = prescaler - 1u;
    g_trigger.in_pt.Init.ClockDivision = 0;
    g_trigger.in_pt.Init.CounterMode   = TIM_COUNTERMODE_UP;

	  pin_config.Mode = GPIO_MODE_IT_RISING;
    pin_config.Pull = GPIO_NOPULL;
    pin_config.Pin  = TRIGGER_IN_PIN;
    pin_config.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(TRIGGER_IN_PORT, &pin_config);

    g_trigger.out_start  = empty_out_handler;
    g_trigger.out_stop   = empty_out_handler;
    g_trigger.in_handler = empty_in_handler;
}



void trigger_in_enable(bool (* fn)(void))
{
    g_trigger.in_handler     = fn;
    g_trigger.in_should_stop = false;

	  /* podesavanja za interrupt tajmera TRIGGER_IN_TIM */
    HAL_NVIC_SetPriority(TRIGGER_IN_TIM_IRQn, IRQ_PRIO_TRIGGER_IN, 0);
    NVIC_ClearPendingIRQ(TRIGGER_IN_TIM_IRQn);
    NVIC_EnableIRQ(TRIGGER_IN_TIM_IRQn);
    
	  /* podesavanja za enskternog interapta TRIGGER_IN_EXTI_IRQn */
    HAL_NVIC_SetPriority(TRIGGER_IN_EXTI_IRQn, IRQ_PRIO_TRIGGER_IN, 0);
    NVIC_ClearPendingIRQ(TRIGGER_IN_EXTI_IRQn);
    NVIC_EnableIRQ(TRIGGER_IN_EXTI_IRQn);
}



void trigger_in_disable(void)
{
    g_trigger.in_should_stop = true;
    NVIC_DisableIRQ(TRIGGER_IN_EXTI_IRQn);
    NVIC_DisableIRQ(TRIGGER_IN_TIM_IRQn);

    __HAL_TIM_DISABLE(&g_trigger.in_pt);
    __HAL_TIM_DISABLE_IT(&g_trigger.in_pt, TIM_IT_UPDATE);
    __HAL_TIM_CLEAR_IT(&g_trigger.in_pt, TIM_IT_UPDATE);

    g_trigger.out_start  = empty_out_handler;
    g_trigger.out_stop   = empty_out_handler;
    g_trigger.in_handler = empty_in_handler;
}



void TRIGGER_IN_EXTI_Handler(void)
{
	NVIC_DisableIRQ(TRIGGER_IN_EXTI_IRQn);

    if (!g_trigger.in_should_stop) {

    	if (g_trigger.in_handler()) {
    		TIM_Base_SetConfig(g_trigger.in_pt.Instance, &g_trigger.in_pt.Init);
			__HAL_TIM_CLEAR_IT(&g_trigger.in_pt, TIM_IT_UPDATE);
			__HAL_TIM_ENABLE_IT(&g_trigger.in_pt, TIM_IT_UPDATE);
			__HAL_TIM_ENABLE(&g_trigger.in_pt);
    	}
    }
    __HAL_GPIO_EXTI_CLEAR_IT(TRIGGER_IN_PIN);
}



void TRIGGER_OUT_TIM_IRQHandler(void)
{
    trigger_out_clr();
    __HAL_TIM_CLEAR_IT(&g_trigger.out_pt, TIM_IT_UPDATE);
    __HAL_TIM_DISABLE_IT(&g_trigger.out_pt, TIM_IT_UPDATE);
    __HAL_TIM_DISABLE(&g_trigger.out_pt);
}



void TRIGGER_IN_TIM_IRQHandler(void)
{
	__HAL_TIM_CLEAR_IT(&g_trigger.in_pt, TIM_IT_UPDATE);
    if (!g_trigger.in_should_stop) {
        HAL_NVIC_EnableIRQ(TRIGGER_IN_EXTI_IRQn);
    }
}
