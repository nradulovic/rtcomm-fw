#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H


#include <hwcon.h>

#ifdef __cplusplus
 extern "C" {
#endif 

 /*
  * Cortex M vectors
  */
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

/*
 * Peripheral vectors
 */
void HWCON_RTCOMM_SPI_DMA_TX_IRQHandler(void);
void HWCON_TEST_TIMER0_IRQHandler(void);
void HWCON_ACQ_0_DRDY_EXTI_Handler(void);
void HWCON_ACQ_0_SPI_IRQ_Handler(void);
void HWCON_ACQ_1_DRDY_EXTI_Handler(void);
void HWCON_ACQ_1_SPI_IRQ_Handler(void);
void HWCON_ACQ_2_DRDY_EXTI_Handler(void);
void HWCON_ACQ_2_SPI_IRQ_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_IT_H */

