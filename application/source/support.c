/*
 * support.c
 *
 *  Created on: Jan 22, 2015
 *      Author: nenad
 */

#include "support.h"
#include "app_stat.h"
#include "bsp.h"

#ifdef  USE_FULL_ASSERT

/* Reports the name of the source file and the source line number where the
 * assert_param error has occurred.
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line
	 * number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
	 * line)
	 */
	(void)file;
	(void)line;

	/* Infinite loop */
	while (1);
}
#endif

PORT_C_NORETURN
void hook_at_assert(
    const struct ncomponent_info * component_info,
    const char *     fn,
    uint32_t                    line,
    const char *     expr,
    const char *     msg)
{
	(void)component_info;
	(void)fn;
	(void)line;
	(void)expr;
	(void)msg;

	status_panic(30);

	for (;;);
}


