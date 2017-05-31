# README #

## How do I get set up? ###

See the below sections to setup the project for STM32F411VE using Eclipse IDE
and arm-none-eabi toolchain.

### Defines and macros
  - STM32F411xE
  - USE_HAL_DRIVER
  
### Libraries
  None
  
### Source directories
  - remove: neon/eds/port
  - remove: neon/drivers
  - remove: application/environment/startup
  - remove: hal/BSP
  - remove: hal/CMSIS/{Device,DSP_Lib,Lib,RTOS}
  - add: neon/eds/port/stm32f4xx-none-gcc

### Linker file
  - environment/stm32f411re_flash.ld
