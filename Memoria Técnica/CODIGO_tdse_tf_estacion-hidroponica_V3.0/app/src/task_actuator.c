/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_actuator_attribute.h"
#include "task_actuator_interface.h"

/********************** macros and definitions *******************************/
#define DEL_BEEP_SHORT 	150ul
#define DEL_ALARM_TOGGLE 500ul


#define ACTUATOR_CFG_QTY	(sizeof(task_actuator_cfg_list)/sizeof(task_actuator_cfg_t))
#define ACTUATOR_DTA_QTY	ACTUATOR_CFG_QTY

/********************** internal data declaration ****************************/
const task_actuator_cfg_t task_actuator_cfg_list[] = {
		// Relés (Active LOW: Encendido con RESET)
		    {ID_ACT_RLY_PUMP,   RLY_PUMP_PORT,   RLY_PUMP_PIN,   RLY_PUMP_ON,   RLY_PUMP_OFF},
		    {ID_ACT_RLY_FAN,    RLY_FAN_PORT,    RLY_FAN_PIN,    RLY_FAN_ON,    RLY_FAN_OFF},
		    // LEDs y Buzzer (Active HIGH: Encendido con SET)
		    {ID_ACT_LED_GREEN,  LED_GREEN_PORT,  LED_GREEN_PIN,  LED_GREEN_ON,  LED_GREEN_OFF},
		    {ID_ACT_LED_YELLOW, LED_YELLOW_PORT, LED_YELLOW_PIN, LED_YELLOW_ON, LED_YELLOW_OFF},
		    {ID_ACT_LED_RED,    LED_RED_PORT,    LED_RED_PIN,    LED_RED_ON,    LED_RED_OFF},
		    {ID_ACT_BUZZER,     BUZZER_PORT,     BUZZER_PIN,     BUZZER_ON,     BUZZER_OFF}
		};

task_actuator_dta_t task_actuator_dta_list[ACTUATOR_DTA_QTY];

/********************** internal functions declaration ***********************/
void task_actuator_statechart(uint32_t index);

/********************** internal data definition *****************************/
const char *p_task_actuator 		= "Task Actuator (Actuator Statechart)";
const char *p_task_actuator_ 		= "Non-Blocking Code";
const char *p_task_actuator__ 		= "(Update by Time Code, period = 1mS)";

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
void task_actuator_init(void *parameters)
{
	uint32_t index;
	const task_actuator_cfg_t *p_task_actuator_cfg;
	task_actuator_dta_t *p_task_actuator_dta;
	task_actuator_st_t state;
	task_actuator_ev_t event;
	bool b_event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - Tick [mS] = %lu", GET_NAME(task_actuator_init), HAL_GetTick());
	LOGGER_INFO("   %s is a %s", GET_NAME(task_actuator), p_task_actuator);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_actuator), p_task_actuator_);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_actuator), p_task_actuator__);

	for (index = 0; ACTUATOR_DTA_QTY > index; index++)
	{
		/* Update Task Actuator Configuration & Data Pointer */
		p_task_actuator_cfg = &task_actuator_cfg_list[index];
		p_task_actuator_dta = &task_actuator_dta_list[index];

		/* Init & Print out: Index & Task execution FSM */
		state = ST_ACT_OFF;
		p_task_actuator_dta->state = state;

		event = EV_ACT_IDLE;
		p_task_actuator_dta->event = event;

		b_event = false;
		p_task_actuator_dta->flag = b_event;

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu   %s = %s",
					 GET_NAME(index), index,
					 GET_NAME(state), (uint32_t)state,
					 GET_NAME(event), (uint32_t)event,
					 GET_NAME(b_event), (b_event ? "true" : "false"));

		HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->state_off);
	}
}

void task_actuator_update(void *parameters)
{
	uint32_t index;

	for (index = 0; ACTUATOR_DTA_QTY > index; index++)
	{
		/* Run Task Statechart */
		task_actuator_statechart(index);
	}
}

void task_actuator_statechart(uint32_t index)
{
	const task_actuator_cfg_t *p_cfg = &task_actuator_cfg_list[index];
	    task_actuator_dta_t *p_dta = &task_actuator_dta_list[index];

	switch (p_dta->state)
	{
	        case ST_ACT_OFF:
	            if (p_dta->flag)
	            {
	                p_dta->flag = false;

	                // Alarma intermitente
	                if (p_dta->event == EV_ACT_ALARM_ON) {
	                    HAL_GPIO_WritePin(p_cfg->gpio_port, p_cfg->pin, p_cfg->state_on);
	                    p_dta->tick = DEL_ALARM_TOGGLE;
	                    p_dta->state = ST_ACT_BLINKING_ON;
	                }
	                // Pitido Corto del buzzer
	                else if (p_dta->event == EV_ACT_BEEP_SUCCESS) {
	                    HAL_GPIO_WritePin(p_cfg->gpio_port, p_cfg->pin, p_cfg->state_on);
	                    p_dta->tick = DEL_BEEP_SHORT;
	                    p_dta->state = ST_ACT_BEEPING;
	                }
	                // Encendido directo de los relés o los LEDs fijos
	                else if (p_dta->event != EV_ACT_IDLE && p_dta->event != EV_ACT_PUMP_OFF &&
	                         p_dta->event != EV_ACT_FAN_OFF && p_dta->event != EV_ACT_LED_OFF) {
	                    HAL_GPIO_WritePin(p_cfg->gpio_port, p_cfg->pin, p_cfg->state_on);
	                    p_dta->state = ST_ACT_ON;
	                }
	            }
	            break;

	        case ST_ACT_ON:
	            if (p_dta->flag)
	            {
	                p_dta->flag = false;
	                // Acción: Apagado Directo
	                if (p_dta->event == EV_ACT_PUMP_OFF || p_dta->event == EV_ACT_FAN_OFF ||
	                    p_dta->event == EV_ACT_ALARM_OFF || p_dta->event == EV_ACT_LED_OFF) {
	                    HAL_GPIO_WritePin(p_cfg->gpio_port, p_cfg->pin, p_cfg->state_off);
	                    p_dta->state = ST_ACT_OFF;
	                }
	            }
	            break;

	        case ST_ACT_BLINKING_ON:
	            if (p_dta->flag && p_dta->event == EV_ACT_ALARM_OFF) {
	                p_dta->flag = false;
	                HAL_GPIO_WritePin(p_cfg->gpio_port, p_cfg->pin, p_cfg->state_off);
	                p_dta->state = ST_ACT_OFF;
	            } else if (p_dta->tick > 0) {
	                p_dta->tick--;
	                if (p_dta->tick == 0) {
	                    HAL_GPIO_WritePin(p_cfg->gpio_port, p_cfg->pin, p_cfg->state_off);
	                    p_dta->tick = DEL_ALARM_TOGGLE;
	                    p_dta->state = ST_ACT_BLINKING_OFF;
	                }
	            }
	            break;

	        case ST_ACT_BLINKING_OFF:
	            if (p_dta->flag && p_dta->event == EV_ACT_ALARM_OFF) {
	                p_dta->flag = false;
	                HAL_GPIO_WritePin(p_cfg->gpio_port, p_cfg->pin, p_cfg->state_off);
	                p_dta->state = ST_ACT_OFF;
	            } else if (p_dta->tick > 0) {
	                p_dta->tick--;
	                if (p_dta->tick == 0) {
	                    HAL_GPIO_WritePin(p_cfg->gpio_port, p_cfg->pin, p_cfg->state_on);
	                    p_dta->tick = DEL_ALARM_TOGGLE;
	                    p_dta->state = ST_ACT_BLINKING_ON;
	                }
	            }
	            break;

	        case ST_ACT_BEEPING:
	            if (p_dta->tick > 0) {
	                p_dta->tick--;
	                if (p_dta->tick == 0) {
	                    HAL_GPIO_WritePin(p_cfg->gpio_port, p_cfg->pin, p_cfg->state_off);
	                    p_dta->state = ST_ACT_OFF;
	                }
	            }
	            break;

	        default:
	            p_dta->state = ST_ACT_OFF;
	            p_dta->event = EV_ACT_IDLE;
	            p_dta->flag = false;
	            break;
	    }
	}

/********************** end of file ******************************************/
