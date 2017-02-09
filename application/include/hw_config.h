/*
 * hw_config.h
 *
 *  Created on: Mar 12, 2015
 *      Author: nenad
 */

#ifndef HW_CONFIG_H_
#define HW_CONFIG_H_

#include "stm32f4xx_hal.h"

/*
 * --  IRQ Priorities  --------------------------------------------------------
 *
 * NOTE:
 * Zero level priority is the highest IRQ priority.
 */

#define IRQ_PRIO_TRIGGER_IN             1
#define IRQ_PRIO_ACQ_x_SPI              1
#define IRQ_PRIO_ACQ_x_EXTI             1
#define IRQ_PRIO_TRIGGER_OUT_TIM        2
#define IRQ_PRIO_SYSTICK                3
#define IRQ_PRIO_NEON					4
#define IRQ_PRIO_SPI_MS_DMA             4
#define IRQ_PRIO_ACQ_AUX_SPI            4
#define IRQ_PRIO_ACQ_AUX_EXTI           4
#define IRQ_PRIO_USB_OTG				6
#define IRQ_PRIO_I2C_CTRL               8

/*
 * --  Notification GPIO  -----------------------------------------------------
 */
#define NOTIFY_PIN						GPIO_PIN_4
#define NOTIFY_PORT						GPIOD
#define NOTIFY_CLK_ENABLE()				__GPIOD_CLK_ENABLE()

/*
 * --  SYNC GPIO  -------------------------------------------------------------
 */
#define SYNC_PIN						GPIO_PIN_7
#define SYNC_PORT					    GPIOD
#define SYNC_CLK_ENABLE()			    __GPIOD_CLK_ENABLE()

/*
 * --  Trigger GPIO OUT -------------------------------------------------------
 */
#define TRIGGER_OUT_PIN                 GPIO_PIN_7
#define TRIGGER_OUT_PORT                GPIOC
#define TRIGGER_OUT_CLK_ENABLE()        __GPIOC_CLK_ENABLE()

/*
 * --  Trigger GPIO IN --------------------------------------------------------
 */
#define TRIGGER_IN_PIN                  GPIO_PIN_5
#define TRIGGER_IN_PORT                 GPIOD
#define TRIGGER_IN_CLK_ENABLE()         __GPIOD_CLK_ENABLE()
#define TRIGGER_IN_EXTI_IRQn            EXTI9_5_IRQn
#define TRIGGER_IN_EXTI_Handler         EXTI9_5_IRQHandler

/*
 * --  Acquisition channel common  --------------------------------------------
 */
#define ACQ_SPI_CLK_PHASE				SPI_PHASE_2EDGE
#define ACQ_SPI_CLK_POL					SPI_POLARITY_LOW

/*
 * --  Channel X --------------------------------------------------------------
 */
#define ACQ_0_DRDY_PIN					GPIO_PIN_2
#define ACQ_0_DRDY_PORT                 GPIOD
#define ACQ_0_DRDY_CLK_ENABLE()         __GPIOD_CLK_ENABLE()
#define ACQ_0_DRDY_EXTI					EXTI2_IRQn
#define ACQ_0_DRDY_EXTI_Handler			EXTI2_IRQHandler

#define ACQ_0_SPI						g_spi4
#define ACQ_0_SPI_CLK_ENABLE()			__SPI4_CLK_ENABLE()
#define ACQ_0_SPI_IRQ					SPI4_IRQn
#define ACQ_0_SPI_IRQ_Handler			SPI4_IRQHandler
#define ACQ_0_SPI_BAUD_CLOCK            SPI_BAUDRATEPRESCALER_64

#define ACQ_0_SPI_NSS_PIN				GPIO_PIN_11
#define ACQ_0_SPI_NSS_GPIO_PORT			GPIOE
#define ACQ_0_SPI_NSS_GPIO_ENABLE()		__GPIOE_CLK_ENABLE()
#define ACQ_0_SPI_NSS_AF				GPIO_AF5_SPI4

#define ACQ_0_SPI_MISO_PIN				GPIO_PIN_13
#define ACQ_0_SPI_MISO_GPIO_PORT		GPIOE
#define ACQ_0_SPI_MISO_GPIO_ENABLE()	__GPIOE_CLK_ENABLE()
#define ACQ_0_SPI_MISO_AF				GPIO_AF5_SPI4

#define ACQ_0_SPI_MOSI_PIN				GPIO_PIN_14
#define ACQ_0_SPI_MOSI_GPIO_PORT		GPIOE
#define ACQ_0_SPI_MOSI_GPIO_ENABLE()	__GPIOE_CLK_ENABLE()
#define ACQ_0_SPI_MOSI_AF				GPIO_AF5_SPI4

#define ACQ_0_SPI_SCK_PIN				GPIO_PIN_12
#define ACQ_0_SPI_SCK_GPIO_PORT			GPIOE
#define ACQ_0_SPI_SCK_GPIO_ENABLE()		__GPIOE_CLK_ENABLE()
#define ACQ_0_SPI_SCK_AF				GPIO_AF5_SPI4

/*
 * --  Channel Y --------------------------------------------------------------
 */
#define ACQ_1_DRDY_PIN					GPIO_PIN_1
#define ACQ_1_DRDY_PORT                 GPIOD
#define ACQ_1_DRDY_CLK_ENABLE()         __GPIOD_CLK_ENABLE()
#define ACQ_1_DRDY_EXTI					EXTI1_IRQn
#define ACQ_1_DRDY_EXTI_Handler			EXTI1_IRQHandler

#define ACQ_1_SPI						g_spi5
#define ACQ_1_SPI_CLK_ENABLE()			__SPI5_CLK_ENABLE()
#define ACQ_1_SPI_IRQ					SPI5_IRQn
#define ACQ_1_SPI_IRQ_Handler			SPI5_IRQHandler
#define ACQ_1_SPI_BAUD_CLOCK            SPI_BAUDRATEPRESCALER_64

#define ACQ_1_SPI_NSS_PIN				GPIO_PIN_4
#define ACQ_1_SPI_NSS_GPIO_PORT			GPIOE
#define ACQ_1_SPI_NSS_GPIO_ENABLE()		__GPIOE_CLK_ENABLE()
#define ACQ_1_SPI_NSS_AF				GPIO_AF6_SPI5

#define ACQ_1_SPI_MISO_PIN				GPIO_PIN_5
#define ACQ_1_SPI_MISO_GPIO_PORT		GPIOE
#define ACQ_1_SPI_MISO_GPIO_ENABLE()	__GPIOE_CLK_ENABLE()
#define ACQ_1_SPI_MISO_AF				GPIO_AF6_SPI5

#define ACQ_1_SPI_MOSI_PIN				GPIO_PIN_6
#define ACQ_1_SPI_MOSI_GPIO_PORT		GPIOE
#define ACQ_1_SPI_MOSI_GPIO_ENABLE()	__GPIOE_CLK_ENABLE()
#define ACQ_1_SPI_MOSI_AF				GPIO_AF6_SPI5

#define ACQ_1_SPI_SCK_PIN				GPIO_PIN_2
#define ACQ_1_SPI_SCK_GPIO_PORT			GPIOE
#define ACQ_1_SPI_SCK_GPIO_ENABLE()		__GPIOE_CLK_ENABLE()
#define ACQ_1_SPI_SCK_AF				GPIO_AF6_SPI5

/*
 * --  Channel Z --------------------------------------------------------------
 */
#define ACQ_2_DRDY_PIN					GPIO_PIN_0
#define ACQ_2_DRDY_PORT                 GPIOD
#define ACQ_2_DRDY_CLK_ENABLE()         __GPIOD_CLK_ENABLE()
#define ACQ_2_DRDY_EXTI					EXTI0_IRQn
#define ACQ_2_DRDY_EXTI_Handler			EXTI0_IRQHandler

#define ACQ_2_SPI						g_spi2
#define ACQ_2_SPI_CLK_ENABLE()			__SPI2_CLK_ENABLE()
#define ACQ_2_SPI_IRQ					SPI2_IRQn
#define ACQ_2_SPI_IRQ_Handler			SPI2_IRQHandler
#define ACQ_2_SPI_BAUD_CLOCK            SPI_BAUDRATEPRESCALER_32

#define ACQ_2_SPI_NSS_PIN				GPIO_PIN_12
#define ACQ_2_SPI_NSS_GPIO_PORT			GPIOB
#define ACQ_2_SPI_NSS_GPIO_ENABLE()		__GPIOB_CLK_ENABLE()
#define ACQ_2_SPI_NSS_AF				GPIO_AF5_SPI2

#define ACQ_2_SPI_MISO_PIN				GPIO_PIN_14
#define ACQ_2_SPI_MISO_GPIO_PORT		GPIOB
#define ACQ_2_SPI_MISO_GPIO_ENABLE()	__GPIOB_CLK_ENABLE()
#define ACQ_2_SPI_MISO_AF				GPIO_AF5_SPI2

#define ACQ_2_SPI_MOSI_PIN				GPIO_PIN_15
#define ACQ_2_SPI_MOSI_GPIO_PORT		GPIOB
#define ACQ_2_SPI_MOSI_GPIO_ENABLE()	__GPIOB_CLK_ENABLE()
#define ACQ_2_SPI_MOSI_AF				GPIO_AF5_SPI2

#define ACQ_2_SPI_SCK_PIN				GPIO_PIN_13
#define ACQ_2_SPI_SCK_GPIO_PORT			GPIOB
#define ACQ_2_SPI_SCK_GPIO_ENABLE()		__GPIOB_CLK_ENABLE()
#define ACQ_2_SPI_SCK_AF				GPIO_AF5_SPI2

/*
 * --  Channel AUX ------------------------------------------------------------
 */
#define ACQ_AUX_DRDY_PIN				GPIO_PIN_3
#define ACQ_AUX_DRDY_PORT               GPIOD
#define ACQ_AUX_DRDY_CLK_ENABLE()       __GPIOD_CLK_ENABLE()
#define ACQ_AUX_DRDY_EXTI				EXTI3_IRQn
#define ACQ_AUX_DRDY_EXTI_Handler		EXTI3_IRQHandler

#define ACQ_AUX_SPI						g_spi1
#define ACQ_AUX_SPI_CLK_ENABLE()		__SPI1_CLK_ENABLE()
#define ACQ_AUX_SPI_IRQ					SPI1_IRQn
#define ACQ_AUX_SPI_IRQ_Handler			SPI1_IRQHandler
#define ACQ_AUX_SPI_BAUD_CLOCK          SPI_BAUDRATEPRESCALER_256

#define ACQ_AUX_SPI_NSS_PIN				GPIO_PIN_4
#define ACQ_AUX_SPI_NSS_GPIO_PORT		GPIOA
#define ACQ_AUX_SPI_NSS_GPIO_ENABLE()	__GPIOA_CLK_ENABLE()
#define ACQ_AUX_SPI_NSS_AF 				GPIO_AF5_SPI1

#define ACQ_AUX_SPI_MISO_PIN			GPIO_PIN_6
#define ACQ_AUX_SPI_MISO_GPIO_PORT		GPIOA
#define ACQ_AUX_SPI_MISO_GPIO_ENABLE()	__GPIOA_CLK_ENABLE()
#define ACQ_AUX_SPI_MISO_AF				GPIO_AF5_SPI1

#define ACQ_AUX_SPI_MOSI_PIN			GPIO_PIN_7
#define ACQ_AUX_SPI_MOSI_GPIO_PORT		GPIOA
#define ACQ_AUX_SPI_MOSI_GPIO_ENABLE()	__GPIOA_CLK_ENABLE()
#define ACQ_AUX_SPI_MOSI_AF				GPIO_AF5_SPI1

#define ACQ_AUX_SPI_SCK_PIN				GPIO_PIN_5
#define ACQ_AUX_SPI_SCK_GPIO_PORT		GPIOA
#define ACQ_AUX_SPI_SCK_GPIO_ENABLE()	__GPIOA_CLK_ENABLE()
#define ACQ_AUX_SPI_SCK_AF				GPIO_AF5_SPI1

/*
 * --  SPI Master description  ----------------------------------------------
 */
#define SPI_MS							SPI3
#define SPI_MS_BAUD_CLOCK				SPI_BAUDRATEPRESCALER_2
#define SPI_MS_CLK_ENABLE()				__SPI3_CLK_ENABLE()

#define SPI_MS_MOSI_PIN					GPIO_PIN_12
#define SPI_MS_MOSI_GPIO_PORT			GPIOC
#define SPI_MS_MOSI_GPIO_ENABLE()		__GPIOC_CLK_ENABLE()
#define SPI_MS_MOSI_AF					GPIO_AF6_SPI3

#define SPI_MS_MISO_PIN					GPIO_PIN_11
#define SPI_MS_MISO_GPIO_PORT			GPIOC
#define SPI_MS_MISO_GPIO_ENABLE()		__GPIOC_CLK_ENABLE()
#define SPI_MS_MISO_AF					GPIO_AF6_SPI3

#define SPI_MS_SCK_PIN					GPIO_PIN_10
#define SPI_MS_SCK_GPIO_PORT			GPIOC
#define SPI_MS_SCK_GPIO_ENABLE()		__GPIOC_CLK_ENABLE()
#define SPI_MS_SCK_AF					GPIO_AF6_SPI3

#define SPI_MS_NSS_PIN					GPIO_PIN_15
#define SPI_MS_NSS_GPIO_PORT			GPIOA
#define SPI_MS_NSS_GPIO_ENABLE()		__GPIOA_CLK_ENABLE()
#define SPI_MS_NSS_AF					GPIO_AF6_SPI3

#define SPI_MS_CLK_PHASE				SPI_PHASE_1EDGE
#define SPI_MS_CLK_POL					SPI_POLARITY_LOW

/* Definition for SPIx's DMA */
#define SPI_MS_DMA_CLK_ENABLE()			__DMA1_CLK_ENABLE()

#define SPI_MS_TX_DMA_CHANNEL           DMA_CHANNEL_0
#define SPI_MS_TX_DMA_STREAM            DMA1_Stream5
#define SPI_MS_RX_DMA_CHANNEL           DMA_CHANNEL_0
#define SPI_MS_RX_DMA_STREAM            DMA1_Stream0

/* Definition for SPIx's NVIC */
#define SPI_MS_DMA_TX_IRQn              DMA1_Stream5_IRQn
#define SPI_MS_DMA_TX_IRQHandler        DMA1_Stream5_IRQHandler


/*
 * --  Control I2C description  -----------------------------------------------
 */
#define I2C_CTRL						I2C1
#define I2C_CTRL_CLK_ENABLE()			__HAL_RCC_I2C1_CLK_ENABLE()
#define I2C_CTRL_SDA_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C_CTRL_SCL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2C_CTRL_SCL_PIN                GPIO_PIN_6
#define I2C_CTRL_SCL_GPIO_PORT          GPIOB
#define I2C_CTRL_SCL_AF                 GPIO_AF4_I2C1
#define I2C_CTRL_SDA_PIN                GPIO_PIN_7
#define I2C_CTRL_SDA_GPIO_PORT          GPIOB
#define I2C_CTRL_SDA_AF                 GPIO_AF4_I2C1

#define I2C_CTRL_EV_IRQn                I2C1_EV_IRQn
#define I2C_CTRL_EV_IRQHandler          I2C1_EV_IRQHandler
#define I2C_CTRL_ER_IRQn                I2C1_ER_IRQn
#define I2C_CTRL_ER_IRQHandler          I2C1_ER_IRQHandler

#define I2C_CTRL_FORCE_RESET()          __HAL_RCC_I2C1_FORCE_RESET()
#define I2C_CTRL_RELEASE_RESET()        __HAL_RCC_I2C1_RELEASE_RESET()

/*
 * --  Status LED description  -----------------------------------------------
 */
#define LED_HARTBEAT_PIN                GPIO_PIN_14
#define LED_HARTBEAT_PORT               GPIOD
#define LED_HARTBEAT_CLK_ENABLE()       __GPIOD_CLK_ENABLE()

/*
 * --  Timer for trigger out generation  ----------------------------
 */
#define TRIGGER_OUT_TIM_CLK_ENABLE()    __HAL_RCC_TIM3_CLK_ENABLE()
#define TRIGGER_OUT_TIM_CLK_DISABLE()   __HAL_RCC_TIM3_CLK_DISABLE()
#define TRIGGER_OUT_TIM_IRQn			TIM3_IRQn
#define TRIGGER_OUT_TIM_IRQHandler		TIM3_IRQHandler
#define TRIGGER_OUT_TIM_INSTANCE		TIM3

/*
 * --  Timer for trigger in protection ----------------------------
 */
#define TRIGGER_IN_TIM_CLK_ENABLE()     __HAL_RCC_TIM4_CLK_ENABLE()
#define TRIGGER_IN_TIM_CLK_DISABLE()    __HAL_RCC_TIM4_CLK_DISABLE()
#define TRIGGER_IN_TIM_IRQn             TIM4_IRQn
#define TRIGGER_IN_TIM_IRQHandler       TIM4_IRQHandler
#define TRIGGER_IN_TIM_INSTANCE         TIM4

/*
 * --  Protocol status LED -----------------------------------------
 */
#define PROTOCOL_LED_CLK_ENABLE()		__HAL_RCC_GPIOD_CLK_ENABLE()
#define PROTOCOL_LED_GPIO_PORT			GPIOD
#define PROTOCOL_LED_GPIO_PIN			GPIO_PIN_6

#endif /* HW_CONFIG_H_ */
