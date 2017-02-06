
/*=========================================================  INCLUDE FILES  ==*/

#include "main.h"
#include "epa_main.h"
#include "epa_acq.h"
#include "epa_aux.h"
#include "epa_heartbeat.h"
#include "bsp.h"
#include "app_stat.h"
#include "ctrl_bus.h"
#include "notify.h"
#include "acq_sync.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/

/*-- Event storage and heap --------------------------------------------------*/
static uint8_t                  g_generic_heap_storage[4096];					/* Prostor za din. mem. za event-ove */

/*======================================================  GLOBAL VARIABLES  ==*/

struct nheap             		g_generic_heap;									/* Dinamicka memorija za opstu upotrebu */

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/



int main(void)
{
    bsp_init_hal();


    /* NOTE:
	 * Initialize SYNC output pin, this pin will be used to synchronize
	 * the ADC.
	 */
	acq_sync_init();

	/* Initialize the portable core. Portable core handles interrupts, timer
     * and CPU initialization. After initialization the core timer is started
     * so event timer can be used in this example.
     */
    ncore_init();
    ncore_timer_enable();

    /* Initialize a memory for events. Since events are dynamic they require
     * either a heap memory or a pool memory. Currently only Neon Heap and Pool
     * memory are supported. In the future a standard malloc/free support will
     * be added.
     *
     * Function nheap_init() requires a pointer to heap memory structure,
     * pointer to statically allocated storage and it's size.
     */
    nheap_init(&g_generic_heap, g_generic_heap_storage,
    		sizeof(g_generic_heap_storage));

    /* Register the Heap memory for events. New events will allocate memory from
     * this heap.
     */
    nevent_register_mem(&g_generic_heap.mem_class);

    /* Initialize Acquisition EPA. This EPA responsibility is handling of X, Y
     * and Z ADC and sending data to Linux
     */
    nepa_init(&g_epa_acq,       &g_epa_acq_define);

    /* Initialize Auxiliary EPA. This EPA handles ADC which is used for
     * temperature measuring.
     */
    nepa_init(&g_epa_aux,       &g_epa_aux_define);

    /* Status LED EPA. Signals if everything is OK.
     */
    nepa_init(&g_epa_heartbeat,	&g_epa_heartbeat_define);

    /* Initialize the main EPA. This EPA controls will dispatch control events
     * to all other EPAs.
     */
    nepa_init(&g_epa_main, &g_epa_main_define);

    /* Start the scheduler under Event Processing supervision.
     */
    nthread_schedule();

    return (0);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of main.c
 ******************************************************************************/
