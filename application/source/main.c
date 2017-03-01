
/*=========================================================  INCLUDE FILES  ==*/

#include "main.h"
#include "hwcon.h"
#include "rtcomm.h"
#include "cdi/io.h"
#include "base/debug.h"
#include "sched/deferred.h"

#if defined(HWCON_TEST_TIMER0_ENABLE)
#include "test_timer0.h"
#endif



/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static void deferred(void * arg);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct acq_buffer	    g_storage[2];
static struct nsched_deferred   g_deferred;

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static void deferred(void * arg)
{
    (void)arg;

    rtcomm_push(&g_rtcomm);
}

/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

int main(void)
{
	HAL_Init();

	rtcomm_init(&g_rtcomm, &g_storage[0], &g_storage[1], sizeof(g_storage[0]));
	nsched_deferred_init(&g_deferred, deferred, NULL);
	test_timer0_enable();

	for (;;);

    return (0);
}

#if defined(HWCON_TEST_TIMER0_ENABLE)
void test_timer0_callback(void)
{
	static uint32_t	sample_idx;
	struct acq_buffer * buffer;

	buffer = rtcomm_request(&g_rtcomm);

	sample_set_int(&buffer->sample[sample_idx], 1, ACQ_CHANNEL_X);
	sample_set_int(&buffer->sample[sample_idx], 2, ACQ_CHANNEL_Y);
	sample_set_int(&buffer->sample[sample_idx], 3, ACQ_CHANNEL_Z);
    sample_idx +=30;

	if (sample_idx == NARRAY_DIMENSION(g_storage[0].sample)) {
		sample_idx = 0;
		nsched_deferred_do(&g_deferred);
	}
}
#endif



PORT_C_NORETURN
void hook_at_assert(
    const struct ncomponent_info * component_info,
    const char *                fn,
    uint32_t                    line,
    const char *                expr,
    const char *                msg)
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
