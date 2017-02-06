/*
 * ads1256.c
 *
 *  Created on: Jan 19, 2015
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       ADS1256 driver
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include <string.h>

#include "prim_spi.h"
#include "ads1256.h"
#include "port/compiler.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

#if defined(PORT_C_GCC)
static
__attribute__((naked))
void delay_us(uint32_t delay);
#endif

/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

#if defined(PORT_C_GCC)
static
__attribute__((naked))
void delay_us(uint32_t __attribute__((unused)) delay)
{
	__asm__ __volatile__(
	"	 CBZ R0, end				\n"
	"    MOV R1, #19				\n"
	"loop2:						\n"
	"    MOV R2, R0					\n"
    "loop:						\n"
	"    NOP						\n"
	"    SUB R2, R2, #1				\n"
	"    CMP R2, #0					\n"
	"    BNE loop					\n"
	"    SUB R1, R1, #1				\n"
	"    CMP R1, #0					\n"
	"    BNE loop2					\n"
	"end:						\n"
	"    BX lr						\n"
	:
	:
	: "r0", "r1", "r2");
}
#elif defined(PORT_C_ARMCC)
static __asm void delay_us(uint32_t __attribute__((unused)) delay)
{
    CBZ R0, end	
	MOV R1, #19
loop2
	MOV R2, R0
loop
	NOP
	SUB R2, R2, #1
	CMP R2, #0
	BNE loop
	SUB R1, R1, #1
	CMP R1, #0
	BNE loop2
end
	BX lr
}
#endif

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/


void ads1256_init(struct ads1256_chip * chip, struct spi_client * client,
		const struct ads1256_chip_vt * vtable, uint32_t id)
{
	chip->client = client;
	memcpy(&chip->vt, vtable, sizeof(chip->vt));
	chip->transfer.arg.u32 = id;
}

/**********************************************************************
  * @name   ads1256_reset_sync(struct ads1256_chip * chip)
  * @brief  function synchronize and wake up ADC. Look the datasheet for
						more details of the ADC commands. 
  * @retval no ret values
  *********************************************************************/
void ads1256_reset_sync(struct ads1256_chip * chip)
{
	struct spi_transfer         transfer;
	volatile uint32_t 			timeout;

	transfer.buff[0]  = ADS_CMD_RESET;
	transfer.size     = 1;
	transfer.complete = NULL;
	transfer.arg.ip   = 0;
	transfer.flags    = 0;

	timeout = (uint32_t)1000;

	while (chip->vt.drdy_is_active() && timeout--);
	spi_write_sync(chip->client, &transfer);
#if 0
	HAL_Delay(10);
	transfer.buff[0] = ADS_CMD_SYNC;

	timeout = (uint32_t)1000;

	while (chip->vt.drdy_is_active() && timeout--);
	spi_write_sync(chip->client, &transfer);

	HAL_Delay(10);

	transfer.buff[0] = ADS_CMD_WAKEUP;
	spi_write_sync(chip->client, &transfer);
#endif
}


/**********************************************************************
  * @name    ads1256_write_reg_sync()
  * @brief   function loads ADC register synchronous (possible purpose: initialisaton of adc or 
						 adc register write before start of acquisition)
  * @retval  no ret values
  *********************************************************************/

void ads1256_write_reg_sync(
    struct ads1256_chip *       chip,
    uint32_t                    reg,
    uint8_t                     data)
{
    struct spi_transfer         transfer;

	delay_us(20);
	transfer.buff[0]  = (uint8_t)(ADS_CMD_WREG | reg);
	transfer.buff[1]  = 0;
	transfer.buff[2]  = data;
	transfer.size     = 3;
	transfer.complete = NULL;
	transfer.flags    = 0;
	spi_write_sync(chip->client, &transfer);
}


/**********************************************************************
  * @name   	ads1256_write_reg_async()
  * @brief    asynchronous register (reg) load with data (data)
  * @retval   no return values
  *********************************************************************/
void ads1256_write_reg_async(
    struct ads1256_chip *       chip,
    uint32_t                    reg,
    uint8_t                     data)
{
	struct spi_transfer *       transfer;

	transfer = &chip->transfer;
	transfer->buff[0]  = (uint8_t)(ADS_CMD_WREG | reg);
	transfer->buff[1]  = 0;
	transfer->buff[2]  = data;
	transfer->size     = 3;
	transfer->complete = NULL;
	transfer->flags    = 0;
	spi_write_async(chip->client, transfer);
}


/****************************************************************************
  * @name   ads1256_read_reg_sync()
  * @brief  synhronous register read
  * @retval register content
  ***************************************************************************/
uint8_t ads1256_read_reg_sync(struct ads1256_chip * chip, uint32_t reg)
{
	struct spi_transfer         transfer;

	spi_cs_force_low(chip->client);
	transfer.buff[0]  = (uint8_t)(ADS_CMD_RREG | reg);
	transfer.buff[1]  = 0;
	transfer.size     = 2;
	transfer.complete = NULL;
	transfer.flags    = SPI_TRANSFER_CS_DISABLE;
	spi_write_sync(chip->client, &transfer);
	delay_us(25);
	transfer.size     = 1;

	spi_read_sync(chip->client, &transfer);
	spi_cs_force_high(chip->client);

	return (transfer.buff[0]);
}

/**************************************************************************************************
  * @name 	ads1256_write_then_read_reg_sync()
  * @brief  synchronous sequential read and write operation to the same register  
  * @retval read register value
  ************************************************************************************************/
uint8_t ads1256_write_then_read_reg_sync(struct ads1256_chip * chip, uint32_t reg, uint8_t data)
{
	ads1256_write_reg_sync(chip, reg, data);
	delay_us(20);

	return (ads1256_read_reg_sync(chip, reg));
}

/**********************************************************************
  * @name		 ads1256_read_sync()
  * @brief   synchronous read of four byte sampled data value 
  * @retval  no return values, (sampled data is stored to chip->transfer->buff)
  *********************************************************************/
void ads1256_read_sync(struct ads1256_chip * chip)
{
    struct spi_transfer *       transfer = &chip->transfer;

	transfer->buff[0]  = ADS_CMD_RDATA;
	transfer->size     = 1;
	transfer->complete = NULL;
	transfer->flags    = SPI_TRANSFER_CS_DISABLE;
	spi_cs_force_low(chip->client);
	spi_write_sync(chip->client, transfer);
	delay_us(10);
	transfer->size    = 3;
	spi_read_sync(chip->client, transfer);
	spi_cs_force_high(chip->client);
}

/*******************************************************************
  * @name   ads1256_read_begin_sync()
  * @brief  routine starts synchronous conversion
  * @retval no return values
  *****************************************************************/
void ads1256_read_begin_sync(struct ads1256_chip * chip,
		void                 (* complete)(struct spi_transfer *))
{
	struct spi_transfer *       transfer = &chip->transfer;

	transfer->buff[0]  = ADS_CMD_RDATA;
	transfer->size     = 1;
	transfer->complete = complete;
	transfer->flags    = SPI_TRANSFER_CS_DISABLE;
	spi_cs_force_low(chip->client);
	spi_write_async(chip->client, transfer);
}



void ads1256_read_delay(void)
{
	delay_us(10);
}


void ads1256_sync_min_delay(void)
{
	delay_us(10);
}

/**********************************************************************
  * @name		ads1256_read_finish_sync()
  * @brief  finishes synchronous read and passes pointer to chip transfer 
						data
  * @retval no real return values
  *********************************************************************/
void ads1256_read_finish_sync(
	struct ads1256_chip * 		chip,
	void                     (* complete)(struct spi_transfer *))
{
	struct spi_transfer *       transfer = &chip->transfer;

	transfer->size     = 3;
	transfer->complete = complete;
	transfer->flags    = 0; /* This will now allow read_async to handle CS */

	spi_read_async(chip->client, transfer);
}

/**********************************************************************
  * @name   ads1256_set_cmd_async()
  * @brief  send comand to ads1256 in asynchronous manner
  * @retval mo ret vals
  *********************************************************************/
void ads1256_set_cmd_async(struct ads1256_chip * chip, uint8_t cmd)
{
	struct spi_transfer *       transfer;

	transfer = &chip->transfer;
	transfer->buff[0]  = cmd;
	transfer->size     = 1;
	transfer->complete = NULL;
	transfer->flags    = 0;
	spi_write_async(chip->client, transfer);
}

/**********************************************************************
  * @name  ads1256_set_cmd_async()
  * @brief send synchronous command with delay after 
  * @retval no return
  *********************************************************************/
void ads1256_set_cmd_sync(struct ads1256_chip * chip, uint8_t cmd)
{
    struct spi_transfer         transfer;

    transfer.buff[0]  = cmd;
    transfer.size     = 1;
    transfer.complete = NULL;
    transfer.flags    = 0;
	delay_us(5);
	while (chip->vt.drdy_is_active());
	spi_write_sync(chip->client, &transfer);
	delay_us(5);
}


/**********************************************************************
  * @name		ads1256_set_cmd_sync_no_wait()
  * @brief  send synchronous command without delay after
  * @retval no retval
  *********************************************************************/
void ads1256_set_cmd_sync_no_wait(struct ads1256_chip * chip, uint8_t cmd)
{
    struct spi_transfer         transfer;

    transfer.buff[0]  = cmd;
    transfer.size     = 1;
    transfer.complete = NULL;
    transfer.flags    = 0;
	delay_us(5);
	spi_write_sync(chip->client, &transfer);
}

/**********************************************************************
  * @name   ads1256_rdc_read_async()
  * @brief  read data asynchronous (in interrupt) mode 
  * @retval no retval,  (sampled data is stored to memory memory buffer
						which is pointed by chip->transfer->buff) 
  *********************************************************************/
void ads1256_rdc_read_async(
    struct ads1256_chip *       chip,
    void                     (* complete)(struct spi_transfer *))
{
	struct spi_transfer *	    transfer;

	transfer = &chip->transfer;
	transfer->size	   = 3;
	transfer->complete = complete;
	transfer->flags    = 0;
	spi_read_async(chip->client, transfer);
}

/**********************************************************************
  * @name 	ads1256_rdc_read_sync()
  * @brief  synchroonous data read 
  * @retval no retval,  (sampled data is stored to memory memory buffer
						which is pointed by chip->transfer->buff) 
  *********************************************************************/
void ads1256_rdc_read_sync(
    struct ads1256_chip *       chip,
    void                     (* complete)(struct spi_transfer *))
{
	struct spi_transfer         transfer;

	transfer.size     = 3;
	transfer.complete = complete;
	transfer.flags    = 0;
	spi_read_sync(chip->client, &transfer);
}


/**********************************************************************
  * @name  	ads1256_rdc_setup(struct ads1256_chip * chip)
  * @brief  routine puts adc in continious conversion mode
  * @retval no retval
  *********************************************************************/
void ads1256_rdc_setup(struct ads1256_chip * chip)
{
    struct spi_transfer         transfer;

	transfer.buff[0]  = ADS_CMD_RDATAC;
	transfer.size     = 1;
	transfer.complete = NULL;
	transfer.flags    = 0;

	spi_write_sync(chip->client, &transfer);
}

/**************************************************************
  * @name  	ads1256_rdc_finish(struct ads1256_chip * chip)
  * @brief  stop continious conversion mode
  * @retval no retval
  *************************************************************/
void ads1256_rdc_finish(struct ads1256_chip * chip)
{
    struct spi_transfer         transfer;

    transfer.buff[0]  = ADS_CMD_SDATAC;
    transfer.size     = 1;
    transfer.complete = NULL;
    transfer.flags    = 0;

    while (chip->vt.drdy_is_active());

    spi_write_sync(chip->client, &transfer);
}



void ads1256_drdy_isr_enable(struct ads1256_chip * chip)
{
    if (chip->vt.drdy_isr_enable) {
        chip->vt.drdy_isr_enable();
    }
}



void ads1256_drdy_isr_disable(struct ads1256_chip * chip)
{
    if (chip->vt.drdy_isr_disable) {
        chip->vt.drdy_isr_disable();
    }
}


/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of ads1256.c
 ******************************************************************************/

