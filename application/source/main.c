
/*=========================================================  INCLUDE FILES  ==*/

#include "main.h"
#include "rtcomm.h"

#if defined(TEST_MS_BUS_INCS)
#include "test_timer0.h"
#endif

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

int main(void)
{
	HAL_Init();

	for (;;);

    return (0);
}

#if defined(TEST_MS_BUS_INCS)
void test_timer0_callback(void)
{

}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//***************************************************************
 * END of main.c
 ******************************************************************************/
