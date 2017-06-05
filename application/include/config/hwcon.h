/*
 *  teslameter_3mhx-fw - 2017
 *
 *  hwcon.h (former hw_config.h)
 *
 *  Created on: Mar 12, 2015
 * ----------------------------------------------------------------------------
 *  This file is part of teslameter_3mhx-fw.
 *
 *  teslameter_3mhx-fw is free software: you can redistribute it and/or modify
 *  it under the terms of the Lesser GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  teslameter_3mhx-fw is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with teslameter_3mhx-fw.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------- *//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Hardware configuration header
 *********************************************************************//** @{ */

#ifndef HWCON_H_
#define HWCON_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "stm32f4xx_hal.h"

/*===============================================================  MACRO's  ==*/

/*
 * --  IRQ Priorities  --------------------------------------------------------
 *
 * NOTE:
 * Zero level priority is the highest IRQ priority.
 */

#define IRQ_PRIO_TRIGGER_IN             1
#define HWCON_IRQ_PRIO_PROBE_SPI            1
#define HWCON_IRQ_PRIO_PROBE_EXTI           2
#define IRQ_PRIO_TRIGGER_OUT_TIM        2
#define IRQ_PRIO_SYSTICK                3
#define HWCON_IRQ_PRIO_NEON					4
#define HWCON_IRQ_PRIO_RTCOMM_SPI_DMA_TX    1
#define HWCON_IRQ_PRIO_AUX_SPI            	4
#define HWCON_IRQ_PRIO_AUX_EXTI           	4
#define HWCON_IRQ_PRIO_CTRL               	8

#define HWCON_TEST_TIMER0_IRQ_PRIO		1

/*
 * --  Heartbeat LED GPIO  ----------------------------------------------------
 */
#define HWCON_HEARTBEAT_PIN                	GPIO_PIN_14
#define HWCON_HEARTBEAT_PORT               	GPIOD
#define HWCON_HEARTBEAT_CLK_ENABLE()       	__GPIOD_CLK_ENABLE()

/*
 * --  Protocol status LED ----------------------------------------------------
 */
#define HWCON_PROTOCOL_LED_CLK_ENABLE()		__GPIOD_CLK_ENABLE()
#define HWCON_PROTOCOL_LED_PORT				GPIOD
#define HWCON_PROTOCOL_LED_PIN				GPIO_PIN_6

/*
 * --  Trigger GPIO OUT -------------------------------------------------------
 */
#define HWCON_TRIGGER_OUT_PIN               GPIO_PIN_7
#define HWCON_TRIGGER_OUT_PORT              GPIOC
#define HWCON_TRIGGER_OUT_CLK_ENABLE()      __GPIOC_CLK_ENABLE()

/*
 * --  Trigger GPIO IN --------------------------------------------------------
 */
#define HWCON_TRIGGER_IN_PIN                GPIO_PIN_5
#define HWCON_TRIGGER_IN_PORT               GPIOD
#define HWCON_TRIGGER_IN_CLK_ENABLE()       __GPIOD_CLK_ENABLE()
#define TRIGGER_IN_EXTI_IRQn            	EXTI9_5_IRQn
#define TRIGGER_IN_EXTI_Handler         	EXTI9_5_IRQHandler

/*
 * --  Probe channel common  --------------------------------------------------
 */
#define HWCON_PROBE_SPI_CLK_PHASE			SPI_PHASE_2EDGE
#define HWCON_PROBE_SPI_CLK_POL				SPI_POLARITY_LOW

/*
 * --  SYNC GPIO  -------------------------------------------------------------
 */
#define HWCON_PROBE_SYNC_PIN				GPIO_PIN_7
#define HWCON_PROBE_SYNC_PORT				GPIOD
#define HWCON_PROBE_SYNC_CLK_ENABLE()		__GPIOD_CLK_ENABLE()

/*
 * --  Channel X --------------------------------------------------------------
 */
#define HWCON_PROBE_X_DRDY_PIN            	GPIO_PIN_2
#define HWCON_PROBE_X_DRDY_PORT           	GPIOD
#define HWCON_PROBE_X_DRDY_CLK_ENABLE()   	__GPIOD_CLK_ENABLE()
#define HWCON_PROBE_X_DRDY_EXTI           	EXTI2_IRQn
#define HWCON_PROBE_X_DRDY_EXTI_Handler   	EXTI2_IRQHandler

#define HWCON_PROBE_X_SPI					g_spi4
#define HWCON_PROBE_X_SPI_CLK_ENABLE()		__SPI4_CLK_ENABLE()
#define HWCON_PROBE_X_SPI_IRQ				SPI4_IRQn
#define HWCON_PROBE_X_SPI_IRQ_Handler		SPI4_IRQHandler
#define HWCON_PROBE_X_SPI_BAUD_CLOCK      	SPI_BAUDRATEPRESCALER_64

#define HWCON_PROBE_X_NSS_PIN				GPIO_PIN_11
#define HWCON_PROBE_X_NSS_PORT				GPIOE
#define HWCON_PROBE_X_NSS_CLK_ENABLE()		__GPIOE_CLK_ENABLE()
#define HWCON_PROBE_X_NSS_AF				GPIO_AF5_SPI4

#define HWCON_PROBE_X_MISO_PIN				GPIO_PIN_13
#define HWCON_PROBE_X_MISO_PORT				GPIOE
#define HWCON_PROBE_X_MISO_CLK_ENABLE()		__GPIOE_CLK_ENABLE()
#define HWCON_PROBE_X_MISO_AF				GPIO_AF5_SPI4

#define HWCON_PROBE_X_MOSI_PIN				GPIO_PIN_14
#define HWCON_PROBE_X_MOSI_PORT				GPIOE
#define HWCON_PROBE_X_MOSI_CLK_ENABLE()		__GPIOE_CLK_ENABLE()
#define HWCON_PROBE_X_MOSI_AF				GPIO_AF5_SPI4

#define HWCON_PROBE_X_SCK_PIN				GPIO_PIN_12
#define HWCON_PROBE_X_SCK_PORT				GPIOE
#define HWCON_PROBE_X_SCK_CLK_ENABLE()		__GPIOE_CLK_ENABLE()
#define HWCON_PROBE_X_SCK_AF				GPIO_AF5_SPI4

/*
 * --  Channel Y --------------------------------------------------------------
 */
#define HWCON_PROBE_Y_DRDY_PIN		    	GPIO_PIN_1
#define HWCON_PROBE_Y_DRDY_PORT           	GPIOD
#define HWCON_PROBE_Y_DRDY_CLK_ENABLE()   	__GPIOD_CLK_ENABLE()
#define HWCON_PROBE_Y_DRDY_EXTI				EXTI1_IRQn
#define HWCON_PROBE_Y_DRDY_EXTI_Handler		EXTI1_IRQHandler

#define HWCON_PROBE_Y_SPI					g_spi5
#define HWCON_PROBE_Y_SPI_CLK_ENABLE()		__SPI5_CLK_ENABLE()
#define HWCON_PROBE_Y_SPI_IRQ				SPI5_IRQn
#define HWCON_PROBE_Y_SPI_IRQ_Handler		SPI5_IRQHandler
#define HWCON_PROBE_Y_SPI_BAUD_CLOCK      	SPI_BAUDRATEPRESCALER_64

#define HWCON_PROBE_Y_NSS_PIN				GPIO_PIN_4
#define HWCON_PROBE_Y_NSS_PORT				GPIOE
#define HWCON_PROBE_Y_NSS_CLK_ENABLE()		__GPIOE_CLK_ENABLE()

#define HWCON_PROBE_Y_MISO_PIN				GPIO_PIN_5
#define HWCON_PROBE_Y_MISO_PORT				GPIOE
#define HWCON_PROBE_Y_MISO_CLK_ENABLE()		__GPIOE_CLK_ENABLE()
#define HWCON_PROBE_Y_MISO_AF				GPIO_AF6_SPI5

#define HWCON_PROBE_Y_MOSI_PIN				GPIO_PIN_6
#define HWCON_PROBE_Y_MOSI_PORT				GPIOE
#define HWCON_PROBE_Y_MOSI_CLK_ENABLE()		__GPIOE_CLK_ENABLE()
#define HWCON_PROBE_Y_MOSI_AF				GPIO_AF6_SPI5

#define HWCON_PROBE_Y_SCK_PIN				GPIO_PIN_2
#define HWCON_PROBE_Y_SCK_PORT				GPIOE
#define HWCON_PROBE_Y_SCK_CLK_ENABLE()		__GPIOE_CLK_ENABLE()
#define HWCON_PROBE_Y_SCK_AF				GPIO_AF6_SPI5

/*
 * --  Channel Z --------------------------------------------------------------
 */
#define HWCON_PROBE_Z_DRDY_PIN				GPIO_PIN_0
#define HWCON_PROBE_Z_DRDY_PORT           	GPIOD
#define HWCON_PROBE_Z_DRDY_CLK_ENABLE()   	__GPIOD_CLK_ENABLE()
#define HWCON_PROBE_Z_DRDY_EXTI				EXTI0_IRQn
#define HWCON_PROBE_Z_DRDY_EXTI_Handler		EXTI0_IRQHandler

#define HWCON_PROBE_Z_SPI					g_spi2
#define HWCON_PROBE_Z_SPI_CLK_ENABLE()		__SPI2_CLK_ENABLE()
#define HWCON_PROBE_Z_SPI_IRQ				SPI2_IRQn
#define HWCON_PROBE_Z_IRQ_Handler			SPI2_IRQHandler
#define HWCON_PROBE_Z_SPI_BAUD_CLOCK      	SPI_BAUDRATEPRESCALER_32

#define HWCON_PROBE_Z_NSS_PIN				GPIO_PIN_12
#define HWCON_PROBE_Z_NSS_PORT				GPIOB
#define HWCON_PROBE_Z_NSS_CLK_ENABLE()		__GPIOB_CLK_ENABLE()

#define HWCON_PROBE_Z_MISO_PIN				GPIO_PIN_14
#define HWCON_PROBE_Z_MISO_PORT				GPIOB
#define HWCON_PROBE_Z_MISO_CLK_ENABLE()		__GPIOB_CLK_ENABLE()
#define HWCON_PROBE_Z_MISO_AF				GPIO_AF5_SPI2

#define HWCON_PROBE_Z_MOSI_PIN				GPIO_PIN_15
#define HWCON_PROBE_Z_MOSI_PORT				GPIOB
#define HWCON_PROBE_Z_MOSI_CLK_ENABLE()		__GPIOB_CLK_ENABLE()
#define HWCON_PROBE_Z_MOSI_AF				GPIO_AF5_SPI2

#define HWCON_PROBE_Z_SCK_PIN				GPIO_PIN_13
#define HWCON_PROBE_Z_SCK_PORT				GPIOB
#define HWCON_PROBE_Z_SCK_CLK_ENABLE()		__GPIOB_CLK_ENABLE()
#define HWCON_PROBE_Z_SCK_AF				GPIO_AF5_SPI2

/*
 * --  Channel AUX ------------------------------------------------------------
 */
#define HWCON_AUX_DRDY_PIN				GPIO_PIN_3
#define HWCON_AUX_DRDY_PORT             GPIOD
#define HWCON_AUX_DRDY_CLK_ENABLE()     __GPIOD_CLK_ENABLE()
#define HWCON_AUX_DRDY_EXTI				EXTI3_IRQn
#define HWCON_AUX_DRDY_EXTI_Handler		EXTI3_IRQHandler

#define HWCON_AUX_SPI					g_spi1
#define HWCON_AUX_SPI_CLK_ENABLE()		__SPI1_CLK_ENABLE()
#define HWCON_AUX_SPI_IRQ				SPI1_IRQn
#define HWCON_AUX_SPI_IRQ_Handler		SPI1_IRQHandler
#define HWCON_AUX_SPI_BAUD_CLOCK        SPI_BAUDRATEPRESCALER_256

#define HWCON_AUX_SPI_NSS_PIN			GPIO_PIN_4
#define HWCON_AUX_SPI_NSS_PORT			GPIOA
#define HWCON_AUX_SPI_NSS_CLK_ENABLE()	__GPIOA_CLK_ENABLE()
#define HWCON_AUX_SPI_NSS_AF 			GPIO_AF5_SPI1

#define HWCON_AUX_SPI_MISO_PIN			GPIO_PIN_6
#define HWCON_AUX_SPI_MISO_PORT			GPIOA
#define HWCON_AUX_SPI_MISO_CLK_ENABLE()	__GPIOA_CLK_ENABLE()
#define HWCON_AUX_SPI_MISO_AF			GPIO_AF5_SPI1

#define HWCON_AUX_SPI_MOSI_PIN			GPIO_PIN_7
#define HWCON_AUX_SPI_MOSI_PORT			GPIOA
#define HWCON_AUX_SPI_MOSI_CLK_ENABLE()	__GPIOA_CLK_ENABLE()
#define HWCON_AUX_SPI_MOSI_AF			GPIO_AF5_SPI1

#define HWCON_AUX_SPI_SCK_PIN			GPIO_PIN_5
#define HWCON_AUX_SPI_SCK_PORT			GPIOA
#define HWCON_AUX_SPI_SCK_CLK_ENABLE()	__GPIOA_CLK_ENABLE()
#define HWCON_AUX_SPI_SCK_AF			GPIO_AF5_SPI1

/*
 * --  RTCOMM SPI Master description  -----------------------------------------
 */
#define HWCON_RTCOMM_SPI					SPI3
#define HWCON_RTCOMM_SPI_BAUD_CLOCK			SPI_BAUDRATEPRESCALER_2
#define HWCON_RTCOMM_SPI_CLK_ENABLE()		__SPI3_CLK_ENABLE()

#define HWCON_RTCOMM_MOSI_PIN				GPIO_PIN_12
#define HWCON_RTCOMM_MOSI_PORT				GPIOC
#define HWCON_RTCOMM_MOSI_CLK_ENABLE()		__GPIOC_CLK_ENABLE()
#define HWCON_RTCOMM_MOSI_AF				GPIO_AF6_SPI3

#define HWCON_RTCOMM_MISO_PIN				GPIO_PIN_11
#define HWCON_RTCOMM_MISO_PORT				GPIOC
#define HWCON_RTCOMM_MISO_CLK_ENABLE()		__GPIOC_CLK_ENABLE()
#define HWCON_RTCOMM_MISO_AF				GPIO_AF6_SPI3

#define HWCON_RTCOMM_SCK_PIN				GPIO_PIN_10
#define HWCON_RTCOMM_SCK_PORT				GPIOC
#define HWCON_RTCOMM_SCK_CLK_ENABLE()		__GPIOC_CLK_ENABLE()
#define HWCON_RTCOMM_SCK_AF					GPIO_AF6_SPI3

#define HWCON_RTCOMM_NSS_PIN				GPIO_PIN_15
#define HWCON_RTCOMM_NSS_PORT				GPIOA
#define HWCON_RTCOMM_NSS_CLK_ENABLE()		__GPIOA_CLK_ENABLE()
#define HWCON_RTCOMM_NSS_AF					GPIO_AF6_SPI3

#define HWCON_RTCOMM_SPI_CLK_PHASE			SPI_PHASE_1EDGE
#define HWCON_RTCOMM_SPI_CLK_POL			SPI_POLARITY_LOW

/* Definition for SPIx's DMA */
#define HWCON_RTCOMM_SPI_DMA_CLK_ENABLE()	__DMA1_CLK_ENABLE()

#define HWCON_RTCOMM_SPI_DMA_TX_CHANNEL     DMA_CHANNEL_0
#define HWCON_RTCOMM_SPI_DMA_TX_STREAM      DMA1_Stream5

/* Definition for SPIx's NVIC */
#define HWCON_RTCOMM_SPI_DMA_TX_IRQn        DMA1_Stream5_IRQn
#define HWCON_RTCOMM_SPI_DMA_TX_IRQHandler  DMA1_Stream5_IRQHandler

#define HWCON_RTCOMM_RRQ_PIN				GPIO_PIN_4
#define HWCON_RTCOMM_RRQ_PORT				GPIOD
#define HWCON_RTCOMM_RRQ_CLK_ENABLE()		__GPIOD_CLK_ENABLE()

/*
 * --  Control I2C description  -----------------------------------------------
 */
#define HWCON_CTRL_I2C						I2C1
#define HWCON_CTRL_I2C_CLK_ENABLE()			__I2C1_CLK_ENABLE()
#define HWCON_CTRL_SDA_CLK_ENABLE()  		__GPIOB_CLK_ENABLE()
#define HWCON_CTRL_SCL_CLK_ENABLE()  		__GPIOB_CLK_ENABLE()
#define HWCON_CTRL_I2C_FORCE_RESET()		__HAL_RCC_I2C1_FORCE_RESET()
#define HWCON_CTRL_I2C_RELASE_RESET()		__HAL_RCC_I2C1_RELEASE_RESET()

#define HWCON_CTRL_SCL_PIN                	GPIO_PIN_6
#define HWCON_CTRL_SCL_PORT          		GPIOB
#define HWCON_CTRL_SCL_AF                 	GPIO_AF4_I2C1
#define HWCON_CTRL_SDA_PIN                	GPIO_PIN_7
#define HWCON_CTRL_SDA_PORT          		GPIOB
#define HWCON_CTRL_SDA_AF                 	GPIO_AF4_I2C1

#define HWCON_CTRL_I2C_EV_IRQn              I2C1_EV_IRQn
#define HWCON_CTRL_I2C_EV_IRQHandler        I2C1_EV_IRQHandler
#define HWCON_CTRL_I2C_ER_IRQn              I2C1_ER_IRQn
#define HWCON_CTRL_I2C_ER_IRQHandler        I2C1_ER_IRQHandler

/*
 * --  Timer for trigger out generation  --------------------------------------
 */
#define TRIGGER_OUT_TIM_CLK_ENABLE()    __TIM3_CLK_ENABLE()
#define TRIGGER_OUT_TIM_CLK_DISABLE()   __TIM3_CLK_DISABLE()
/* check this */
#define TRIGGER_OUT_TIM_APB				1
#define TRIGGER_OUT_TIM_IRQn			TIM3_IRQn
#define TRIGGER_OUT_TIM_IRQHandler		TIM3_IRQHandler
#define TRIGGER_OUT_TIM_INSTANCE		TIM3

/*
 * --  Timer for trigger in protection ----------------------------------------
 */
#define TRIGGER_IN_TIM_CLK_ENABLE()     __TIM4_CLK_ENABLE()
#define TRIGGER_IN_TIM_CLK_DISABLE()    __TIM4_CLK_DISABLE()
/* check this */
#define TRIGGER_IN_TIM_APB				1
#define TRIGGER_IN_TIM_IRQn             TIM4_IRQn
#define TRIGGER_IN_TIM_IRQHandler       TIM4_IRQHandler
#define TRIGGER_IN_TIM_INSTANCE         TIM4

/*
 * --  Test timer -------------------------------------------------------------
 */

#define HWCON_TEST_TIMER0_CLK_ENABLE()		__TIM2_CLK_ENABLE()
#define HWCON_TEST_TIMER0_APB				1
#define HWCON_TEST_TIMER0_IRQn				TIM2_IRQn
#define HWCON_TEST_TIMER0_IRQHandler		TIM2_IRQHandler
#define HWCON_TEST_TIMER0					TIM2


#if defined(TEST_MS_BUS_INCS)
#define HWCON_TEST_TIMER0_ENABLE
#endif

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/
/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of hwcon.h
 ******************************************************************************/
#endif /* HWCON_H_ */
