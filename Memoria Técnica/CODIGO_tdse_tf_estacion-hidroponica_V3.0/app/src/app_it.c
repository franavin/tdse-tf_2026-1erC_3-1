/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data declaration ****************************/
volatile uint32_t g_app_tick_cnt;

/********************** external functions definition ************************/
void app_it_init(void)
{
	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
	/* Init Tick Counter */
	g_app_tick_cnt = 0;
    __asm("CPSIE i");	/* enable interrupts */
}

void HAL_SYSTICK_Callback(void)
{
	/* Update Tick Counter */
	g_app_tick_cnt++;
}

/********************** end of file ******************************************/
