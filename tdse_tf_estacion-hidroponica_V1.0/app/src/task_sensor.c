/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"
#include "task_sensor_attribute.h"

/********************** macros and definitions *******************************/
#define DEL_BTN_MIN		0ul
#define DEL_BTN_MED		25ul
#define DEL_BTN_MAX     40ul
#define DEL_RLY_MAX     20ul
#define DEL_ADC_MAX     100ul

// Variable externa generada por STM32CubeMX para el conversor analógico
extern ADC_HandleTypeDef hadc1;

#define SENSOR_CFG_QTY		(sizeof(task_sensor_cfg_list)/sizeof(task_sensor_cfg_t))
#define SENSOR_DTA_QTY		SENSOR_CFG_QTY

/********************** internal data declaration ****************************/
const task_sensor_cfg_t task_sensor_cfg_list[] ={
	{ID_BTN_MODE,    BTN_MODE_PORT,    BTN_MODE_PIN,    BTN_MODE_PRESSED,   DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_MODE_DOWN},
	{ID_BTN_CONF,    BTN_CONF_PORT,    BTN_CONF_PIN,    BTN_CONF_PRESSED,   DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_CONF_DOWN},
	{ID_BTN_NEXT,    BTN_NEXT_PORT,   BTN_NEXT_PIN,    BTN_NEXT_PRESSED,    DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_NEXT_DOWN},
	{ID_BTN_PREV,     BTN_PREV_PORT,     BTN_PREV_PIN,     BTN_PREV_PRESSED,    DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_PREV_DOWN},
	{ID_RLY_FB_PUMP, RLY_FB_PUMP_PORT, RLY_FB_PUMP_PIN, RLY_FB_PUMP_CLOSED, DEL_RLY_MAX, EV_SYS_IDLE, EV_SYS_RLY_CLOSED},
	{ID_SENS_LVL,    NULL,             0,               GPIO_PIN_RESET,     DEL_ADC_MAX, EV_SYS_LVL_OK, EV_SYS_LVL_CRIT}
};

task_sensor_dta_t task_sensor_dta_list[SENSOR_DTA_QTY];

/********************** internal functions declaration ***********************/
void task_sensor_statechart(uint32_t index);

/********************** internal data definition *****************************/
const char *p_task_sensor 		= "Task Sensor (Sensor Statechart)";
const char *p_task_sensor_ 		= "Non-Blocking Code";
const char *p_task_sensor__ 	= "(Update by Time Code, period = 1mS)";

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
void task_sensor_init(void *parameters)
{
	uint32_t index;
	task_sensor_dta_t *p_task_sensor_dta;
	task_sensor_st_t state;
	task_sensor_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - Tick [mS] = %lu", GET_NAME(task_sensor_init), HAL_GetTick());
	LOGGER_INFO("   %s is a %s", GET_NAME(task_sensor), p_task_sensor);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_sensor), p_task_sensor_);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_sensor), p_task_sensor__);

	for (index = 0; SENSOR_DTA_QTY > index; index++)
	{
		/* Update Task Sensor Data Pointer */
		p_task_sensor_dta = &task_sensor_dta_list[index];


		/* Init & Print out: Index & Task execution FSM */
		state = ST_SNSR_UP;
		p_task_sensor_dta->state = state;

		event = EV_RAW_UP;
		p_task_sensor_dta->event = event;

		p_task_sensor_dta->tick= 0;

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",
				    GET_NAME(index), index,
					GET_NAME(state), (uint32_t)state,
					GET_NAME(event), (uint32_t)event);
	}
}

void task_sensor_update(void *parameters)
{
	uint32_t index;

	aht20_update();

	for (index = 0; SENSOR_DTA_QTY > index; index++)
	{
		/* Run Task Statechart */
		task_sensor_statechart(index);
	}
}

void task_sensor_statechart(uint32_t index)
{
	const task_sensor_cfg_t *p_cfg = &task_sensor_cfg_list[index];;
	task_sensor_dta_t *p_dta = &task_sensor_dta_list[index];

	bool is_active = false;

 // --- 1. LECTURA DEL HARDWARE ---
    if (p_cfg->identifier == ID_SENS_LVL)
    {
        /* Lógica ADC para el pote */
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK)
        {
            uint32_t adc_val = HAL_ADC_GetValue(&hadc1);
            // Si el valor analógico cae por debajo del umbral,
            // se considera ACTIVO (Alarma Crítica)
            if (adc_val < 1000) {
                is_active = true;
            }
        }
    }
    else
    {
        /* Lógica estándar para los pines de los botones y el lazo cerrado */
        if (p_cfg->pressed_state == HAL_GPIO_ReadPin(p_cfg->gpio_port, p_cfg->pin))
        {
            is_active = true;
        }
    }

    // Traducción de la lectura a los eventos defaults
    if (is_active) {
        p_dta-> event = EV_RAW_DOWN;
    } else {
        p_dta-> event = EV_RAW_UP;
    }

    // Statecharts para no que no se tomen falsos positivos/negativos ---
    switch (p_dta->state)
    {
        case ST_SNSR_UP:
            if (EV_RAW_DOWN == p_dta->event)
            {
                p_dta->tick = p_cfg->tick_max;
                p_dta->state = ST_SNSR_FALLING;
            }
            break;

        case ST_SNSR_FALLING:
            if (p_dta->tick > 0) {
                p_dta->tick--;
            }

            if (0 == p_dta->tick)
            {
                if (EV_RAW_DOWN == p_dta->event)
                {
                    // Confirmado. Se emite la señal correspondiente al Sistema (si la hay)
                    if (p_cfg->signal_down != EV_SYS_IDLE) {
                        put_event_task_system(p_cfg->signal_down);
                    }
                    p_dta->state = ST_SNSR_DOWN;
                }
                else
                {
                    p_dta->state = ST_SNSR_UP; // Falsa alarma por ruido electromagnético
                }
            }
            break;

        case ST_SNSR_DOWN:
            if (EV_RAW_UP == p_dta->event)
            {
                p_dta->tick = p_cfg->tick_max;
                p_dta->state = ST_SNSR_RISING;
            }
            break;

        case ST_SNSR_RISING:
            if (p_dta->tick > 0) {
                p_dta->tick--;
            }

            if (0 == p_dta->tick)
            {
                if (EV_RAW_UP == p_dta->event)
                {
                    // Confirmado. Se emite la señal de recuperación que enviamos a el sistema
                    if (p_cfg->signal_up != EV_SYS_IDLE) {
                        put_event_task_system(p_cfg->signal_up);
                    }
                    p_dta->state = ST_SNSR_UP;
                }
                else
                {
                    p_dta->state = ST_SNSR_DOWN;
                }
            }
            break;

        default:
            p_dta->tick  = 0;
            p_dta->state = ST_SNSR_UP;
            p_dta->event = EV_RAW_UP;
            break;
    }
}

/********************** end of file ******************************************/
