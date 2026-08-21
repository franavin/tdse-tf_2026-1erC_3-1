/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_display_attribute.h"
#include "task_display_interface.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
void put_event_task_display(uint32_t char_column, uint32_t char_row, const char *message)
{
	task_display_dta_t *p_task_display_dta;

	p_task_display_dta = &task_display_dta;

	p_task_display_dta->event = EV_DSP_UPDATE;
	p_task_display_dta->flag = true;

	while ((ROWS > char_row) && (COLUMNS > char_column))
	{
		if ('\0' == *message)
			break;
		else
			p_task_display_dta->ddram[char_row][char_column++] = *message++;
	}
}

/********************** end of file ******************************************/
