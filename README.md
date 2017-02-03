# README #

## How do I get set up? ###

### Defines and macros
  - STM32F411xE
  - USE_HAL_DRIVER
  - USE_USB_FS
  
### Libraries
  - libm
  
### Source directories
  - remove: neon/eds/port
  - remove: lib/ST32_USB_Device_Library/Class
  - remove: application/environment/startup
  - add: neon/eds/port/stm32f4xx-none-gcc
  - add: lib/STM32_USB_Device_Library/Class/CDC
  - add: application/environment/startup/gcc with filter set only to
         startup_stm32f411xe.S

### Linker file
  - application/environment/linker/stm32f411re_flash.ld
