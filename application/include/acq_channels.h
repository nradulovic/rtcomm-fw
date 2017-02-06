/*
 * acq_channels.h
 *
 *  Created on: May 26, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_ACQ_CHANNELS_H_
#define APPLICATION_INCLUDE_ACQ_CHANNELS_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>

#include "hw_config.h"
#include "trigger.h"
#include "cdi/io.h"
#include "ads1256.h"
#include "port/compiler.h"

/*===============================================================  MACRO's  ==*/


/* Single trigger async
 */
#define ACQ_METHOD_SINGLE_ASYNC         0
#define ACQ_METHOD_MULTI_ASYNC          1
#define ACQ_METHOD_MULTI_SYNC           2

#define ACQ_CONFIG_METHOD               ACQ_METHOD_SINGLE_ASYNC

#define ACQ_REG_STATUS_Id				(0)
#define ACQ_REG_STATUS_Msk              (0x1u << 0)
#define ACQ_REG_MUX_Id					(1)
#define ACQ_REG_MUX_Msk                 (0x1u << 1)
#define ACQ_REG_ADCON_Id				(2)
#define ACQ_REG_ADCON_Msk               (0x1u << 2)
#define ACQ_REG_DRATE_Id				(3)
#define ACQ_REG_DRATE_Msk               (0x1u << 3)
#define ACQ_REG_IO_Id					(4)
#define ACQ_REG_IO_Msk                  (0x1u << 4)
#define ACQ_REG_OFC0_Id					(5)
#define ACQ_REG_OFC0_Msk                (0x1u << 5)
#define ACQ_REG_OFC1_Id					(6)
#define ACQ_REG_OFC1_Msk                (0x1u << 6)
#define ACQ_REG_OFC2_Id					(7)
#define ACQ_REG_OFC2_Msk                (0x1u << 7)
#define ACQ_REG_FSC0_Id					(8)
#define ACQ_REG_FSC0_Msk                (0x1u << 8)
#define ACQ_REG_FSC1_Id					(9)
#define ACQ_REG_FSC1_Msk                (0x1u << 9)
#define ACQ_REG_FSC2_Id					(10)
#define ACQ_REG_FSC2_Msk                (0x1u << 10)

/*============================================================  DATA TYPES  ==*/

struct spi_transfer;

struct acq_channels
{
	/* Prekidna rutina za pocetak citanja ADC-a, DRDY pin
	 */
	void				     (* isr_begin_transfer)(void);

	/* Struktura rezervisana za svaki kanal ponaosob.
	 */
	struct acq_channel
	{
	    struct spi_client           client;
	    struct ads1256_chip         chip;
	    bool                        is_enabled;
	}							chn[3];

	/* Acquisition mode
	 * - ACQ_MODE_CONTINUOUS - Read Data  (RDC - see ADS1256 datasheet)
	 * - ACQ_MODE_REQUEST - Request Data (Normal ADS1256 mode)
	 */
	uint32_t					acq_mode;

	/* Trigger mode
	 * - TRIG_MODE_OUT
	 * - TRIG_MODE_IN_CONTINUOUS
	 * - TRIG_MODE_IN_SINGLE_SHOT
	 */
	uint32_t					trig_mode;

	/* Used for triggered input
	 * - true - take and store the ADC data
	 * - false - ignore ADC data
	 */
	bool						trig_is_allowed;
};

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/


void acq_x_bus_init(void);



void acq_x_reset(
    uint32_t                    channel_no);



void acq_set_trigger_speed(uint32_t speed);



void acq_x_write_reg(
    uint32_t                    channel_no,
    uint32_t                    reg,
    uint8_t                     data);



uint8_t acq_x_read_reg(uint32_t channel_no, uint32_t reg);



void acq_x_channel_enable(
    uint32_t                    channel_no);



void acq_x_channel_disable(
    uint32_t                    channel_no);



bool acq_x_is_enabled(uint32_t channel_no);



void acq_x_trigger_mode(uint32_t trig_mode);



void acq_x_trigger_enable(void);



/* NOTE:
 * mode =
 *  - ACQ_MODE_CONTINUOUS
 *  - ACQ_MODE_REQUEST
 */
void acq_x_mode(uint32_t mode);



void acq_x_trigger_sync_min_delay(void);



void acq_x_isr_enable(void (* fn)(void));



void acq_x_start_rdc(void);



void acq_x_isr_disable(void);



void acq_x_stop_rdc(void);



void acq_x_restart_process(void);



PORT_C_INLINE
void acq_x_drdy_isr(void)
{
	extern struct acq_channels g_acq;

	g_acq.isr_begin_transfer();
}



extern void acq_transfer_finished(struct spi_transfer * transfer);

extern void acq_isr_begin_rdc_trigger_out(void);
extern void acq_isr_begin_rdc_trigger_in_s(void);
extern void acq_isr_begin_rdc_trigger_in_c(void);
extern void acq_isr_begin_rc_trigger_in_c(void);

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of acq_channels.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_ACQ_CHANNELS_H_ */
