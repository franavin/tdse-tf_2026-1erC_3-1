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
#include "task_display_attribute.h"
#include "task_display_interface.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"
#include "eeprom.h"
#include <stdio.h>

/********************** macros and definitions *******************************/
#define DEL_SYS_MIN			0ul
#define DEL_SYS_MED			250ul
#define DEL_SYS_MAX			500ul

/* Modes to excite Task System */
typedef enum task_system_mode {NORMAL, SETUP, ERROR, MODE_QTY} task_system_mode_t;

#define SYSTEM_DTA_QTY	MODE_QTY

/********************** internal data declaration ****************************/
task_system_dta_t task_system_dta_list[SYSTEM_DTA_QTY];

task_system_dta_t task_system_dta;
/********************** internal functions declaration ***********************/
void task_system_normal_statechart(void);
void task_system_setup_statechart(void);
void task_system_error_statechar(void);

void task_system_set_mode(task_system_mode_t);

/********************** internal data definition *****************************/
const char *p_task_system 		= "Task System (System Statechart)";
const char *p_task_system_ 		= "Non-Blocking Code";
const char *p_task_system__ 	= "(Update by Time Code, period = 1mS)";

/********************** external data declaration ****************************/
task_system_mode_t g_task_system_mode;

/********************** external functions definition ************************/
void task_system_statechart(void);

void task_system_init(void *parameters)
{
    LOGGER_INFO("  %s is running", GET_NAME(task_system_init));

    init_event_task_system();

    /* Configuración Inicial */
    task_system_dta.state = ST_SYS_NORMAL_IDLE;
    task_system_dta.event = EV_SYS_IDLE;
    task_system_dta.flag  = false;

    eeprom_init();

    uint32_t t_riego_guardado = eeprom_read_uint32(0x00);
    uint32_t t_espera_guardado = eeprom_read_uint32(0x04);

    for (int i = 0; i < MODE_QTY; i++){
    	task_system_dta_list[i].receta_tiempo_riego = t_riego_guardado;
    	task_system_dta_list[i].receta_tiempo_espera = t_espera_guardado;
    	task_system_dta_list[i].flag = false;
    	task_system_dta_list[i].event = EV_SYS_IDLE;
    }

    if (t_riego_guardado == 0xFFFFFFFF || t_espera_guardado == 0xFFFFFFFF){
    	LOGGER_INFO("  EEPROM VACIA: Cargando valores default");
    	task_system_dta.receta_tiempo_espera = 10000ul; // 10 seg
        task_system_dta.receta_tiempo_riego = 3000ul;   // 3 seg
    } else {
    	LOGGER_INFO("  EEPROM LEIDA: Riego=%lu, Espera=%lu",t_riego_guardado,t_espera_guardado);
    	task_system_dta.receta_tiempo_riego = t_riego_guardado;
    	task_system_dta.receta_tiempo_espera = t_espera_guardado;
    }

    // Inicializamos temporizadores y variables
        task_system_dta.t_espera = task_system_dta.receta_tiempo_espera;
        task_system_dta.t_riego = 0;
        task_system_dta.menu_linea_actual = 0;

        task_system_dta.t_ciclo_info = 2000ul;
        task_system_dta.fase_info = 0;
        task_system_dta.t_brillo = 5000ul;

    // Mensaje de Bienvenida en el LCD
    put_event_task_display(0, 0, " ESTACION LISTA ");
    put_event_task_display(0, 1, " MODO: ESPERA   ");
}


void task_system_update(void *parameters)
{
	/* Run Task Statechart */
	switch (g_task_system_mode)
	{
		case NORMAL:

			task_system_normal_statechart();

			break;

		case SETUP:

			task_system_setup_statechart();

			break;

		case ERROR:

			task_system_error_statechart();

			break;

		default:

			task_system_set_mode(NORMAL);

			break;
		}
}

void task_system_normal_statechart(void)
{
	task_system_dta_t *p_dta;
    p_dta = &task_system_dta_list[NORMAL];

    /* Verificamos los Eventos */
    if (true == any_event_task_system())
    {
        p_dta->flag = true;
        p_dta->event = get_event_task_system();
    }
    else
    {
        // Evento temporal cíclico (1ms) para los timers internos
        p_dta->flag = true;
        p_dta->event = EV_SYS_TICK;
    }

	switch (p_dta->state)
	{
    case ST_SYS_NORMAL_IDLE:
        if (p_dta->flag && p_dta->event == EV_SYS_LVL_CRIT)
        {
            p_dta->flag = false;
            put_event_task_actuator(EV_ACT_PUMP_OFF, ID_ACT_RLY_PUMP);
            put_event_task_actuator(EV_ACT_ALARM_ON, ID_ACT_BUZZER);
            put_event_task_display(0, 0, " ERROR CRITICO! ");
            put_event_task_display(0, 1, " TANQUE VACIO   ");
            //p_dta->state = ST_SYS_ERROR;
            task_system_set_mode(ERROR);
        }
        else if (p_dta->flag && p_dta->event == EV_SYS_MODE_DOWN)
        {
            p_dta->flag = false;
            put_event_task_display(0, 0, "-- MENU SETUP --");
            put_event_task_display(0, 1, " > Edit Riego   ");
            //p_dta->state = ST_SYS_SET_UP;
            task_system_set_mode(SETUP);
        }
        else if (p_dta->flag && p_dta->event == EV_SYS_TICK)
        {
            p_dta->flag = false;
            if (p_dta->t_espera == 0)
            {
                // Arrancar el riego
                put_event_task_actuator(EV_ACT_PUMP_ON, ID_ACT_RLY_PUMP);
                p_dta->t_riego = p_dta->receta_tiempo_riego;
                put_event_task_display(0, 1, " MODO: REGANDO  ");
                p_dta->state = ST_SYS_NORMAL_WATERING;
            }
            else
            {
                p_dta->t_espera--;
            }
        }
        break;

    case ST_SYS_NORMAL_WATERING:
        if (p_dta->flag && p_dta->event == EV_SYS_LVL_CRIT)
        {
            p_dta->flag = false;
            put_event_task_actuator(EV_ACT_PUMP_OFF, ID_ACT_RLY_PUMP);
            put_event_task_actuator(EV_ACT_ALARM_ON, ID_ACT_BUZZER);
            put_event_task_display(0, 0, " ERROR CRITICO! ");
            put_event_task_display(0, 1, " TANQUE VACIO   ");
            //p_dta->state = ST_SYS_ERROR;
            task_system_set_mode(ERROR);
        }
        else if (p_dta->flag && p_dta->event == EV_SYS_TICK)
        {
            p_dta->flag = false;
            if (p_dta->t_riego == 0)
            {
                // Terminar el riego
                put_event_task_actuator(EV_ACT_PUMP_OFF, ID_ACT_RLY_PUMP);
                p_dta->t_espera = p_dta->receta_tiempo_espera;
                put_event_task_display(0, 1, " MODO: ESPERA   ");
                p_dta->state = ST_SYS_NORMAL_IDLE;
            }
            else
            {
                p_dta->t_riego--;
            }
        }
        break;

		default:

			p_dta->tick  = DEL_SYS_MIN;
			p_dta->state = ST_SYS_IDLE;
			p_dta->event = EV_SYS_IDLE;
			p_dta->flag = false;

			break;
	}
}

void task_system_setup_statechart(void)
{
	task_system_dta_t *p_dta;
    p_dta = &task_system_dta_list[SETUP];

    char str_buffer[17]; // Búfer de texto para imprimir en el LCD
    /* Verificamos los Eventos */
    if (true == any_event_task_system())
    {
        p_dta->flag = true;
        p_dta->event = get_event_task_system();
    }
    else
    {
        // Evento temporal cíclico (1ms) para los timers internos
        p_dta->flag = true;
        p_dta->event = EV_SYS_TICK;
    }

    switch (p_dta->state){
	case ST_SYS_SET_UP:
			if (p_dta->flag)
			{
				p_dta->flag = false;

				if (p_dta->event == EV_SYS_MODE_DOWN)
				{
					// Salir del menú sin guardar (Cancelar)
					put_event_task_display(0, 0, " ESTACION LISTA ");
					put_event_task_display(0, 1, " MODO: ESPERA   ");
					//p_dta->state = ST_SYS_NORMAL_IDLE;
					task_system_set_mode(NORMAL);
				}
				else if (p_dta->event == EV_SYS_INC_DOWN)
				{
					// Ejemplo de navegación de menú
					p_dta->receta_tiempo_riego += 1000; // Sube 1 segundo
					char buf[17];
					snprintf(buf, sizeof(buf), " T. Riego: %lus ", (p_dta->receta_tiempo_riego / 1000));
					put_event_task_display(0, 1, buf);
				}
				else if (p_dta->event == EV_SYS_CONF_DOWN)
				{
					// Guardar parámetros
					eeprom_write_uint32(0x00, p_dta-> receta_tiempo_riego);
					eeprom_write_uint32(0x04, p_dta-> receta_tiempo_espera);

					put_event_task_actuator(EV_ACT_BEEP_SUCCESS, ID_ACT_BUZZER);
					put_event_task_display(0, 1, " > Guardado!    ");
					task_system_set_mode(NORMAL);
				}
			}
			break;

		default:

			p_dta->tick  = DEL_SYS_MIN;
			p_dta->state = ST_SYS_IDLE;
			p_dta->event = EV_SYS_IDLE;
			p_dta->flag = false;

			break;
	}
}

void task_system_error_statechart(){
	task_system_dta_t *p_dta;
	    p_dta = &task_system_dta_list[SETUP];

	    char str_buffer[17]; // Búfer de texto para imprimir en el LCD
	    /* Verificamos los Eventos */
	    if (true == any_event_task_system())
	    {
	        p_dta->flag = true;
	        p_dta->event = get_event_task_system();
	    }
	    else
	    {
	        // Evento temporal cíclico (1ms) para los timers internos
	        p_dta->flag = true;
	        p_dta->event = EV_SYS_TICK;
	    }

	switch (p_dta->state){
		case ST_SYS_ERROR:
			if (p_dta->flag && p_dta->event == EV_SYS_LVL_OK)
			{
				p_dta->flag = false;
				put_event_task_actuator(EV_ACT_ALARM_OFF, ID_ACT_BUZZER);
				put_event_task_display(0, 0, " ESTACION LISTA ");
				put_event_task_display(0, 1, " MODO: ESPERA   ");
				p_dta->t_espera = p_dta->receta_tiempo_espera;
				//p_dta->state = ST_SYS_NORMAL_IDLE;
				task_system_set_mode(NORMAL);
			}
			break;

		default:

			p_dta->tick  = DEL_SYS_MIN;
			p_dta->state = ST_SYS_IDLE;
			p_dta->event = EV_SYS_IDLE;
			p_dta->flag = false;

			break;
}
void task_system_set_mode(task_system_mode_t task_system_mode)
{
	put_event_task_actuator(EV_ACT_LED_OFF, ID_ACT_LED_GREEN);
	put_event_task_actuator(EV_ACT_LED_OFF, ID_ACT_LED_YELLOW);
	put_event_task_actuator(EV_ACT_LED_OFF, ID_ACT_LED_RED);



	task_system_dta_list[task_system_mode].receta_tiempo_riego = task_system_dta_list[g_task_system_mode].receta_tiempo_riego;
	task_system_dta_list[task_system_mode].receta_tiempo_espera = task_system_dta_list[g_task_system_mode].receta_tiempo_espera;
	if (task_system_mode == NORMAL){
		task_system_dta_list[task_system_mode].t_espera = task_system_dta_list[task_system_mode].receta_tiempo_espera;
		task_system_dta_list[task_system_mode].state = ST_SYS_NORMAL_IDLE;
		put_event_task_actuator(EV_ACT_LED_GREEN_ON, ID_ACT_LED_GREEN);
	} else if (task_system_mode == SETUP){
		task_system_dta_list[task_system_mode].menu_linea_actual = 0;
		task_system_dta_list[task_system_mode].state = ST_SYS_SET_UP;
		put_event_task_actuator(EV_ACT_LED_YELLOW_ON, ID_ACT_LED_YELLOW);
	} else if (task_system_mode == ERROR){
		task_system_dta_list[task_system_mode].state = ST_SYS_ERROR;
		put_event_task_actuator(EV_ACT_LED_RED_ON, ID_ACT_LED_RED);
	}

	//Se hace el salto final para el modo que se elija
	g_task_system_mode = task_system_mode;
}

/********************** end of file ******************************************/
