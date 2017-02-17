
/*=========================================================  INCLUDE FILES  ==*/

#include "main.h"
#include "hwcon.h"
#include "rtcomm.h"

#if defined(TEST_MS_BUS_INCS)
#include "test_timer0.h"
#endif

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/

static uint8_t		storage_a[50000];
static uint8_t		storage_b[sizeof(storage_a)];

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

int main(void)
{
	HAL_Init();

	rtcomm_init(&g_rtcomm, storage_a, storage_b, sizeof(storage_a));
	test_timer0_enable();

	for (;;);

    return (0);
}

#if defined(HWCON_TEST_TIMER0_ENABLE)
void test_timer0_callback(void)
{
	static uint32_t	sample_idx;
	uint32_t multi;
	uint8_t * buffer;

	buffer = rtcomm_request(&g_rtcomm);

	for (multi = 0; multi < (sizeof(storage_a) / 100u); multi++) {
		buffer[sample_idx] = sample_idx & 0xfu;
		sample_idx++;
	}

	if (sample_idx == sizeof(storage_a)) {
		sample_idx = 0;
		rtcomm_push(&g_rtcomm);
	}
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of main.c
 ******************************************************************************/
