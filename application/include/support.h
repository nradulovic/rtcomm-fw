/*
 * support.h
 *
 *  Created on: Mar 12, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_SUPPORT_H_
#define APPLICATION_INCLUDE_SUPPORT_H_

#include "neon_eds.h"

void assert_failed(uint8_t* file, uint32_t line);

PORT_C_NORETURN
void hook_at_assert(
    const struct ncomponent_info * component_info,
    const char *     fn,
    uint32_t                    line,
    const char *     expr,
    const char *     msg);


#endif /* APPLICATION_INCLUDE_SUPPORT_H_ */
