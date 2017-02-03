#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H


#include "hw_config.h"

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

void ACQ_AUX_DRDY_EXTI_Handler(void);
void ACQ_AUX_SPI_IRQ_Handler(void);

void ACQ_0_DRDY_EXTI_Handler(void);
void ACQ_1_DRDY_EXTI_Handler(void);
void ACQ_2_DRDY_EXTI_Handler(void);
void ACQ_0_SPI_IRQ_Handler(void);
void ACQ_1_SPI_IRQ_Handler(void);
void ACQ_2_SPI_IRQ_Handler(void);
/*
 * Other peripheral ISRs
 */
void OTG_FS_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_IT_H */

