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
#include "display.h"

/********************** macros and definitions *******************************/
#define DEL_DSP_MIN				0ul
#define DEL_DSP_MED				50ul
#define DEL_DSP_MAX				500ul

/********************** internal data declaration ****************************/
task_display_dta_t task_display_dta;

/********************** internal functions declaration ***********************/
void task_display_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_display		= "Task Display (Display Statechart)";
const char *p_task_display_		= "Non-Blocking Code";
const char *p_task_display__	= "(Update by Time Code, period = 1mS)";

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
void task_display_init(void *parameters)
{
	task_display_dta_t 	*p_task_display_dta;
	task_display_st_t	state;
	task_display_ev_t	event;
	bool b_event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - Tick [mS] = %lu", GET_NAME(task_display_init), HAL_GetTick());
	LOGGER_INFO("   %s is a %s", GET_NAME(task_display), p_task_display);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_display), p_task_display_);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_display), p_task_display__);

	/* Update Task Display Data Pointer */
	p_task_display_dta = &task_display_dta;

	/* Init & Print out: Task execution FSM */
	state = ST_DSP_IDLE;
	p_task_display_dta->state = state;

	event = EV_DSP_IDLE;
	p_task_display_dta->event = event;

	b_event = false;
	p_task_display_dta->flag = b_event;

	LOGGER_INFO(" ");
	LOGGER_INFO("   %s = %lu   %s = %lu   %s = %s",
				GET_NAME(state), (uint32_t)state,
				GET_NAME(event), (uint32_t)event,
				GET_NAME(b_event), (b_event ? "true" : "false"));

	/* Init & Print out: LCD Display */
	displayInit( DISPLAY_CONNECTION_GPIO_4BITS );

	displayCharPositionWrite(0, 0);
    p_task_display_dta->row = 0;
	displayStringWrite(p_task_display_dta->ddram[p_task_display_dta->row]);

    displayCharPositionWrite(0, 1);
	p_task_display_dta->row = 1;
	displayStringWrite(p_task_display_dta->ddram[p_task_display_dta->row]);
}

void task_display_update(void *parameters)
{
	/* Run Task Statechart */
	task_display_statechart();
}

void task_display_statechart(void)
{
	task_display_dta_t *p_task_display_dta;

	/* Update Task Display Data Pointer */
	p_task_display_dta = &task_display_dta;

	switch (p_task_display_dta->state)
	{
		case ST_DSP_IDLE:

			if ((true == p_task_display_dta->flag) && (EV_DSP_UPDATE == p_task_display_dta->event))
			{
				p_task_display_dta->state = ST_DSP_UPDATE;
				p_task_display_dta->row = 0;
				p_task_display_dta->column = 0;
			}

			break;

		case ST_DSP_UPDATE:

			if ((true == p_task_display_dta->flag) && (EV_DSP_UPDATE == p_task_display_dta->event))
			{
				p_task_display_dta->flag = false;
				p_task_display_dta->state = ST_DSP_UPDATE;
				p_task_display_dta->row = 0;
				p_task_display_dta->column = 0;

				displayCharPositionWrite(p_task_display_dta->column, p_task_display_dta->row);
			}
			else
			{
				if (COLUMNS > p_task_display_dta->column)
				{
					p_task_display_dta->character = p_task_display_dta->ddram[p_task_display_dta->row][p_task_display_dta->column++];
					displayDataWrite(p_task_display_dta->character);
				}
				else
					{
					p_task_display_dta->row++;
					if (ROWS > p_task_display_dta->row)
					{
						p_task_display_dta->column = 0;
						displayCharPositionWrite(p_task_display_dta->column, p_task_display_dta->row);
					}
					else
						p_task_display_dta->state = ST_DSP_IDLE;
					}
				}

			break;

		default:

			p_task_display_dta->tick  = DEL_DSP_MIN;
			p_task_display_dta->state = ST_DSP_IDLE;
			p_task_display_dta->event = EV_DSP_IDLE;
			p_task_display_dta->flag = false;

			break;
	}
}

/********************** end of file ******************************************/
