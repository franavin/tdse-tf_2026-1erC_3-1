/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"
#include "systick.h" // Aseguramos la inclusión del header correspondiente

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
/* Provides a blocking delay in microseconds using the SysTick timer */
void systick_delay_us(uint32_t delay_us)
{
	uint32_t start, current, target, elapsed;

    if (0 == delay_us)
    	return;

    /* Get the start value of the SysTick counter */
	start = SysTick->VAL;

	/* Calculate the total number of SysTick counts required for the delay */
	target = delay_us * (SystemCoreClock / 1000000UL);

    /* Loop until the required counts have elapsed */
    while (1)
    {
        /* Get the current value of the SysTick counter */
    	current = SysTick->VAL;

        /* Handle the case where the SysTick counter wraps around, */
        /* counts down to 0 and reloads */
        if (current <= start)
        {
        	elapsed = start - current;
        }
        else
        {
            /* Counter wrapped around, */
        	/* add the reload value to account for the wrap */
        	elapsed = SysTick->LOAD + start - current;
        }

        /* Exit the loop when the desired delay is reached */
        if (elapsed >= target)
        {
        	break;
        }
    }
}

/********************** end of file ******************************************/
