/*
 * protocol.h
 *
 *  Created on: May 31, 2017
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_PROTOCOL_H_
#define APPLICATION_INCLUDE_PROTOCOL_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>
#include <stdbool.h>
#include "cdi/io.h"

/*===============================================================  MACRO's  ==*/
/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

bool protocol_from_en_probe_buffer(const struct io_ctrl_config * config);
uint8_t protocol_from_probe_mux_hi(const struct io_ctrl_config * config);
uint8_t protocol_from_probe_mux_lo(const struct io_ctrl_config * config);

uint8_t protocol_from_workmode(const struct io_ctrl_param * param);
uint32_t protocol_from_vspeed(const struct io_ctrl_param * param);
bool protocol_from_en_autorange(const struct io_ctrl_param * param);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of protocol.h
 ******************************************************************************/
#endif /* APPLICATION_INCLUDE_PROTOCOL_H_ */
