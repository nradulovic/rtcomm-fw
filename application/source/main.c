
/*=========================================================  INCLUDE FILES  ==*/

#include "main.h"
#include "neon_eds.h"
#include "epa_ctrl.h"
#include "epa_controller.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define do_tasks()				nthread_schedule()

/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static void init_early(void)
{
	/*
	 * Save reset source here?
	 */
}

static void init_peripherals(void)
{
	HAL_Init();
}

static void init_tasks(void)
{
	static uint8_t 				mem_buffer[10240];
	static struct nheap   		mem;

	ncore_init();
	ncore_timer_enable();
	nheap_init(&mem, mem_buffer, sizeof(mem_buffer));
	nevent_register_mem(&mem.mem_class);

	nepa_init(&g_controller_epa, 	&g_controller_epa_define);
	nepa_init(&g_ctrl_epa,   		&g_ctrl_epa_define);
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

int main(void)
{
	init_early();
	init_peripherals();
	init_tasks();
	do_tasks();

    return (0);
}

/* -- External support methods for libraries -------------------------------- */
PORT_C_NORETURN
void hook_at_assert(const struct ncomponent_info * component_info,
    const char * fn, uint32_t line, const char * expr, const char * msg)
{
    (void)component_info;
    (void)fn;
    (void)line;
    (void)expr;
    (void)msg;

    for (;;);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of main.c
 ******************************************************************************/
