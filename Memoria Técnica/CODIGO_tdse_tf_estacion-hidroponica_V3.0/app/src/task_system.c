/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"

#include "aht20.h"
#include "task_actuator_attribute.h"
#include "task_actuator_interface.h"
#include "task_display_attribute.h"
#include "task_display_interface.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"
#include "eeprom.h"
#include "bluetooth.h"
#include <stdio.h>

/********************** macros and definitions *******************************/
#define DEL_SYS_MIN			0ul
#define DEL_SYS_MED			250ul
#define DEL_SYS_MAX			500ul

/* Modes to excite Task System */
typedef enum task_system_mode {NORMAL, SETUP, MODO_ERROR, MODE_QTY} task_system_mode_t;

#define SYSTEM_DTA_QTY	MODE_QTY

/********************** internal data declaration ****************************/
task_system_dta_t task_system_dta_list[SYSTEM_DTA_QTY];

//task_system_dta_t task_system_dta;
/********************** internal functions declaration ***********************/
void task_system_normal_statechart(void);
void task_system_setup_statechart(void);
void task_system_error_statechart(void);

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
    eeprom_init();

    // LEER LA MEMORIA
    uint32_t t_riego_guardado = eeprom_read_uint32(0x00);
    uint32_t t_espera_guardado = eeprom_read_uint32(0x04);
    uint32_t t_temp_on_guardado = eeprom_read_uint32(0x08);
    uint32_t t_temp_off_guardado = eeprom_read_uint32(0x0C);


    if (t_riego_guardado == 0xFFFFFFFF || t_espera_guardado == 0 || t_temp_on_guardado > 100){
            LOGGER_INFO("  EEPROM SUCIA o CORRUPTA: Forzando valores default");
            t_riego_guardado = 3000ul;
            t_espera_guardado = 10000ul;
            t_temp_on_guardado = 28ul;   // Temp Max Default
            t_temp_off_guardado = 24ul;  // Temp Min Default
        } else {
            LOGGER_INFO("  EEPROM LEIDA: Riego=%lu, Espera=%lu", t_riego_guardado, t_espera_guardado);
        }


    for (int i = 0; i < MODE_QTY; i++){
        task_system_dta_list[i].receta_tiempo_riego = t_riego_guardado;
        task_system_dta_list[i].receta_tiempo_espera = t_espera_guardado;
        task_system_dta_list[i].receta_temp_on = t_temp_on_guardado;
        task_system_dta_list[i].receta_temp_off = t_temp_off_guardado;
        task_system_dta_list[i].flag = false;
        task_system_dta_list[i].event = EV_SYS_IDLE;
    }

    task_system_dta_t *p_dta = &task_system_dta_list[NORMAL];

    p_dta->state = ST_SYS_NORMAL_IDLE;
    p_dta->t_espera = p_dta->receta_tiempo_espera;
    p_dta->t_riego = 0;
    p_dta->menu_linea_actual = 0;
    p_dta->t_ciclo_info = 2000ul;
    p_dta->fase_info = 0;
    p_dta->t_brillo = 5000ul;

    p_dta->sistema_pausado = true;

	// Mensaje de Bienvenida en el LCD
	put_event_task_display(0, 0, " ESTACION LISTA ");
	put_event_task_display(0, 1, " MODO: PAUSADO  ");
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

		case MODO_ERROR:

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

    if (p_dta->flag && p_dta->event == EV_SYS_NEXT_DOWN)
        {
            p_dta->flag = false; // Consumimos el evento
            p_dta->menu_linea_actual++;

            if (p_dta->menu_linea_actual > 2) {
                p_dta->menu_linea_actual = 0; // Volver a la pantalla principal
            }

            // Dibujar la pantalla correspondiente
            if (p_dta->menu_linea_actual == 0) {
            	put_event_task_display(0, 0, " ESTACION LISTA ");
				if (p_dta->state == ST_SYS_NORMAL_IDLE) {
					if (p_dta->sistema_pausado) put_event_task_display(0, 1, " MODO: PAUSADO  ");
					else put_event_task_display(0, 1, " MODO: ESPERA   ");
				}
				else put_event_task_display(0, 1, " MODO: REGANDO  ");
            }
            else if (p_dta->menu_linea_actual == 1) {
            	float temp_celsius = aht20_get_temperature();
				float hum_relativa = aht20_get_humidity();

				// Formateamos el texto para el LCD
				char buf[17];
				put_event_task_display(0, 0, " CLIMA ACTUAL:  ");

				// %f imprime variables float. El .1 significa "un decimal" y el .0 "cero decimales".
				// Los dos %% sirven para imprimir el símbolo de porcentaje real en la pantalla.
				snprintf(buf, sizeof(buf), " T:%.1fC H:%.0f%% ", temp_celsius, hum_relativa);

				put_event_task_display(0, 1, buf);
            }
            else if (p_dta->menu_linea_actual == 2) {
                put_event_task_display(0, 0, " RIEGO MANUAL:  ");
                put_event_task_display(0, 1, " > Apretar CONF ");
            }
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
            bluetooth_send_alert("\r\n!!! ALERTA CRITICA: TANQUE VACIO !!!\r\n");
            task_system_set_mode(MODO_ERROR);
        }
        // --- ERROR: BOMBA TRABADA ---
		else if (p_dta->flag && p_dta->event == EV_SYS_PUMP_FAULT)
		{
			p_dta->flag = false;
			put_event_task_actuator(EV_ACT_PUMP_OFF, ID_ACT_RLY_PUMP);
			put_event_task_actuator(EV_ACT_ALARM_ON, ID_ACT_BUZZER);
			put_event_task_display(0, 0, " ERROR CRITICO! ");
			put_event_task_display(0, 1, " BOMBA TRABADA  ");
			bluetooth_send_alert("\r\n!!! ALERTA: BOMBA TRABADA !!!\r\n");
			task_system_set_mode(MODO_ERROR);
		}
		// --- ERROR: VENTILADOR TRABADO ---
		else if (p_dta->flag && p_dta->event == EV_SYS_FAN_FAULT)
		{
			p_dta->flag = false;
			put_event_task_actuator(EV_ACT_FAN_OFF, ID_ACT_RLY_FAN);
			put_event_task_actuator(EV_ACT_ALARM_ON, ID_ACT_BUZZER);
			put_event_task_display(0, 0, " ERROR CRITICO! ");
			put_event_task_display(0, 1, " VENT. TRABADO  ");
			bluetooth_send_alert("\r\n!!! ALERTA: VENTILADOR TRABADO !!!\r\n");
			task_system_set_mode(MODO_ERROR);
		}
        else if (p_dta->flag && p_dta->event == EV_SYS_MODE_DOWN)
        {
            p_dta->flag = false;
            put_event_task_display(0, 0, "-- MENU SETUP --");
            put_event_task_display(0, 1, " > 1. Riego     ");

            task_system_set_mode(SETUP);
        }
        else if (p_dta->flag && p_dta->event == EV_SYS_CONF_DOWN)
		{
			p_dta->flag = false;
			if (p_dta->menu_linea_actual == 0) {
				p_dta->sistema_pausado = !p_dta->sistema_pausado;
				if (p_dta->sistema_pausado) {
					put_event_task_display(0, 1, " MODO: PAUSADO  ");
				} else {
					put_event_task_display(0, 1, " MODO: ESPERA   ");
				}
			}
			// Si apretamos CONFIRMAR y estamos mirando la pantalla 2 (Riego Manual)
			else if (p_dta->menu_linea_actual == 2) {
				p_dta->sistema_pausado = false;
				p_dta->t_espera = 0;
				p_dta->menu_linea_actual = 0;
				put_event_task_display(0, 0, " ESTACION LISTA ");
			}
		}
        else if (p_dta->flag && p_dta->event == EV_SYS_TICK)
        {
            p_dta->flag = false;
            if (p_dta->t_espera % 1000 == 0) {
				float t = aht20_get_temperature();
				if (t >= (float)p_dta->receta_temp_on) {
					put_event_task_actuator(EV_ACT_FAN_ON, ID_ACT_RLY_FAN);
				} else if (t <= (float)p_dta->receta_temp_off) {
					put_event_task_actuator(EV_ACT_FAN_OFF, ID_ACT_RLY_FAN);
				}
			}
            if (!p_dta->sistema_pausado)
			{
				if (p_dta->t_espera == 0)
				{
					// Arrancar el riego
					put_event_task_actuator(EV_ACT_PUMP_ON, ID_ACT_RLY_PUMP);
					p_dta->t_riego = p_dta->receta_tiempo_riego;
					if (p_dta->menu_linea_actual == 0) {
						put_event_task_display(0, 1, " MODO: REGANDO  ");
					}
					p_dta->state = ST_SYS_NORMAL_WATERING;
				}
				else
				{
					p_dta->t_espera--;
				}
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
            task_system_set_mode(MODO_ERROR);
        }
        else if (p_dta->flag && p_dta->event == EV_SYS_PUMP_FAULT)
		{
			p_dta->flag = false;
			put_event_task_actuator(EV_ACT_PUMP_OFF, ID_ACT_RLY_PUMP);
			put_event_task_actuator(EV_ACT_ALARM_ON, ID_ACT_BUZZER);
			put_event_task_display(0, 0, " ERROR CRITICO! ");
			put_event_task_display(0, 1, " BOMBA TRABADA  ");
			bluetooth_send_alert("\r\n!!! ALERTA: BOMBA TRABADA !!!\r\n");
			task_system_set_mode(MODO_ERROR);
		}
		else if (p_dta->flag && p_dta->event == EV_SYS_FAN_FAULT)
		{
			p_dta->flag = false;
			put_event_task_actuator(EV_ACT_FAN_OFF, ID_ACT_RLY_FAN);
			put_event_task_actuator(EV_ACT_ALARM_ON, ID_ACT_BUZZER);
			put_event_task_display(0, 0, " ERROR CRITICO! ");
			put_event_task_display(0, 1, " VENT. TRABADO  ");
			bluetooth_send_alert("\r\n!!! ALERTA: VENTILADOR TRABADO !!!\r\n");
			task_system_set_mode(MODO_ERROR);
		}
        else if (p_dta->flag && p_dta->event == EV_SYS_TICK)
        {
            p_dta->flag = false;
            if (p_dta->t_riego % 1000 == 0) {
				float t = aht20_get_temperature();
				if (t >= (float)p_dta->receta_temp_on) {
					put_event_task_actuator(EV_ACT_FAN_ON, ID_ACT_RLY_FAN);
				} else if (t <= (float)p_dta->receta_temp_off) {
					put_event_task_actuator(EV_ACT_FAN_OFF, ID_ACT_RLY_FAN);
				}
			}
            if (p_dta->t_riego == 0)
            {
                // Terminar el riego
                put_event_task_actuator(EV_ACT_PUMP_OFF, ID_ACT_RLY_PUMP);
                p_dta->t_espera = p_dta->receta_tiempo_espera;

				if (p_dta->menu_linea_actual == 0) {
					put_event_task_display(0, 1, " MODO: ESPERA   ");
				}
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
			p_dta->state = ST_SYS_NORMAL_IDLE;
			p_dta->event = EV_SYS_IDLE;
			p_dta->flag = false;

			break;
	}
}

void task_system_setup_statechart(void)
{
    task_system_dta_t *p_dta;
    p_dta = &task_system_dta_list[SETUP];

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
                task_system_set_mode(NORMAL);
            }
            else if (p_dta->event == EV_SYS_NEXT_DOWN)
            {
                if (p_dta->fase_info == 0) {
                    // Navegación del menú raíz (5 opciones)
                    p_dta->menu_linea_actual++;
                    if (p_dta->menu_linea_actual > 4) p_dta->menu_linea_actual = 0;

                    if (p_dta->menu_linea_actual == 0) put_event_task_display(0, 1, " > 1. Riego     ");
                    else if (p_dta->menu_linea_actual == 1) put_event_task_display(0, 1, " > 2. Espera    ");
                    else if (p_dta->menu_linea_actual == 2) put_event_task_display(0, 1, " > 3. Temp Max  ");
                    else if (p_dta->menu_linea_actual == 3) put_event_task_display(0, 1, " > 4. Temp Min  ");
                    else if (p_dta->menu_linea_actual == 4) put_event_task_display(0, 1, " > 5. Guardar   ");
                }
                else {
                    char buf[21];
                    if (p_dta->fase_info == 1) {
                        p_dta->receta_tiempo_riego += 1000;
                        snprintf(buf, sizeof(buf), " T. Riego: %lus ", (p_dta->receta_tiempo_riego / 1000));
                    } else if (p_dta->fase_info == 2) {
                        p_dta->receta_tiempo_espera += 1000;
                        snprintf(buf, sizeof(buf), " T. Espera: %lus ", (p_dta->receta_tiempo_espera / 1000));
                    } else if (p_dta->fase_info == 3) {
                        if (p_dta->receta_temp_on < 60) p_dta->receta_temp_on++;
                        snprintf(buf, sizeof(buf), " Temp.: %lu C   ", p_dta->receta_temp_on);
                    } else if (p_dta->fase_info == 4) {
                        // Bloqueo: La Temp Mínima no puede alcanzar a la Máxima
                        if (p_dta->receta_temp_off < (p_dta->receta_temp_on - 1)) p_dta->receta_temp_off++;
                        snprintf(buf, sizeof(buf), " Temp.: %lu C   ", p_dta->receta_temp_off);
                    }
                    put_event_task_display(0, 1, buf);
                }
            }
            else if (p_dta->event == EV_SYS_PREV_DOWN)
            {
                if (p_dta->fase_info == 0) {
                    if (p_dta->menu_linea_actual == 0) p_dta->menu_linea_actual = 4;
                    else p_dta->menu_linea_actual--;

                    if (p_dta->menu_linea_actual == 0) put_event_task_display(0, 1, " > 1. Riego     ");
                    else if (p_dta->menu_linea_actual == 1) put_event_task_display(0, 1, " > 2. Espera    ");
                    else if (p_dta->menu_linea_actual == 2) put_event_task_display(0, 1, " > 3. Temp Max  ");
                    else if (p_dta->menu_linea_actual == 3) put_event_task_display(0, 1, " > 4. Temp Min  ");
                    else if (p_dta->menu_linea_actual == 4) put_event_task_display(0, 1, " > 5. Guardar   ");
                }
                else {
                    char buf[21];
                    if (p_dta->fase_info == 1) {
                        if (p_dta->receta_tiempo_riego > 1000) p_dta->receta_tiempo_riego -= 1000;
                        snprintf(buf, sizeof(buf), " T. Riego: %lus ", (p_dta->receta_tiempo_riego / 1000));
                    } else if (p_dta->fase_info == 2) {
                        if (p_dta->receta_tiempo_espera > 1000) p_dta->receta_tiempo_espera -= 1000;
                        snprintf(buf, sizeof(buf), " T. Espera: %lus ", (p_dta->receta_tiempo_espera / 1000));
                    } else if (p_dta->fase_info == 3) {
                        if (p_dta->receta_temp_on > (p_dta->receta_temp_off + 1)) p_dta->receta_temp_on--;
                        snprintf(buf, sizeof(buf), " Temp.: %lu C   ", p_dta->receta_temp_on);
                    } else if (p_dta->fase_info == 4) {
                        if (p_dta->receta_temp_off > 0) p_dta->receta_temp_off--;
                        snprintf(buf, sizeof(buf), " Temp.: %lu C   ", p_dta->receta_temp_off);
                    }
                    put_event_task_display(0, 1, buf);
                }
            }
            else if (p_dta->event == EV_SYS_CONF_DOWN)
            {
                if (p_dta->fase_info == 0) {
                    if (p_dta->menu_linea_actual == 0) {
                        p_dta->fase_info = 1;
                        put_event_task_display(0, 0, " EDITANDO RIEGO ");
                        char buf[21]; snprintf(buf, sizeof(buf), " T. Riego: %lus ", (p_dta->receta_tiempo_riego / 1000)); put_event_task_display(0, 1, buf);
                    }
                    else if (p_dta->menu_linea_actual == 1) {
                        p_dta->fase_info = 2;
                        put_event_task_display(0, 0, " EDITANDO ESPERA");
                        char buf[21]; snprintf(buf, sizeof(buf), " T. Espera: %lus ", (p_dta->receta_tiempo_espera / 1000)); put_event_task_display(0, 1, buf);
                    }
                    else if (p_dta->menu_linea_actual == 2) {
                        p_dta->fase_info = 3;
                        put_event_task_display(0, 0, " EDIT TEMP MAX  ");
                        char buf[21]; snprintf(buf, sizeof(buf), " Temp.: %lu C   ", p_dta->receta_temp_on); put_event_task_display(0, 1, buf);
                    }
                    else if (p_dta->menu_linea_actual == 3) {
                        p_dta->fase_info = 4;
                        put_event_task_display(0, 0, " EDIT TEMP MIN  ");
                        char buf[21]; snprintf(buf, sizeof(buf), " Temp.: %lu C   ", p_dta->receta_temp_off); put_event_task_display(0, 1, buf);
                    }
                    else if (p_dta->menu_linea_actual == 4) {
                        // Escritura en memoria de todos los parámetros
                        eeprom_write_uint32(0x00, p_dta->receta_tiempo_riego);
                        HAL_Delay(10);
                        eeprom_write_uint32(0x04, p_dta->receta_tiempo_espera);
                        HAL_Delay(10);
                        eeprom_write_uint32(0x08, p_dta->receta_temp_on);
                        HAL_Delay(10);
                        eeprom_write_uint32(0x0C, p_dta->receta_temp_off);

                        put_event_task_actuator(EV_ACT_BEEP_SUCCESS, ID_ACT_BUZZER);
                        put_event_task_display(0, 0, "----------------");
                        put_event_task_display(0, 1, " > Guardado!    ");

                        HAL_Delay(1000);
                        task_system_set_mode(NORMAL);
                    }
                }
                else {
                    p_dta->fase_info = 0;
                    put_event_task_display(0, 0, "-- MENU SETUP --");
                    if (p_dta->menu_linea_actual == 0) put_event_task_display(0, 1, " > 1. Riego     ");
                    else if (p_dta->menu_linea_actual == 1) put_event_task_display(0, 1, " > 2. Espera    ");
                    else if (p_dta->menu_linea_actual == 2) put_event_task_display(0, 1, " > 3. Temp Max  ");
                    else if (p_dta->menu_linea_actual == 3) put_event_task_display(0, 1, " > 4. Temp Min  ");
                }
            }
        }
        break;

    default:
        p_dta->tick  = DEL_SYS_MIN;
        p_dta->state = ST_SYS_NORMAL_IDLE;
        p_dta->event = EV_SYS_IDLE;
        p_dta->flag = false;
        break;
    }
}

void task_system_error_statechart(){
	task_system_dta_t *p_dta;
	    p_dta = &task_system_dta_list[MODO_ERROR];

	    //char str_buffer[21]; // Búfer de texto para imprimir en el LCD
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
				bluetooth_send_alert("\r\nINFO: Agua restablecida. Volviendo a ESPERA.\r\n");
				task_system_set_mode(NORMAL);
			}
			else if (p_dta->flag && p_dta->event == EV_SYS_CONF_DOWN)
			{
				p_dta->flag = false;
				put_event_task_actuator(EV_ACT_ALARM_OFF, ID_ACT_BUZZER);
				put_event_task_display(0, 0, " ESTACION LISTA ");
				put_event_task_display(0, 1, " MODO: ESPERA   ");

				p_dta->t_espera = p_dta->receta_tiempo_espera;

				bluetooth_send_alert("\r\nINFO: Falla reseteada por operador. Volviendo a ESPERA.\r\n");
				task_system_set_mode(NORMAL);
			}
			break;

		default:

			p_dta->tick  = DEL_SYS_MIN;
			p_dta->state = ST_SYS_NORMAL_IDLE;
			p_dta->event = EV_SYS_IDLE;
			p_dta->flag = false;

			break;
	}
}
void task_system_set_mode(task_system_mode_t task_system_mode)
{
	put_event_task_actuator(EV_ACT_LED_OFF, ID_ACT_LED_GREEN);
	put_event_task_actuator(EV_ACT_LED_OFF, ID_ACT_LED_YELLOW);
	put_event_task_actuator(EV_ACT_LED_OFF, ID_ACT_LED_RED);



	task_system_dta_list[task_system_mode].receta_tiempo_riego = task_system_dta_list[g_task_system_mode].receta_tiempo_riego;
	task_system_dta_list[task_system_mode].receta_tiempo_espera = task_system_dta_list[g_task_system_mode].receta_tiempo_espera;
	task_system_dta_list[task_system_mode].sistema_pausado = task_system_dta_list[g_task_system_mode].sistema_pausado;
	task_system_dta_list[task_system_mode].receta_temp_on = task_system_dta_list[g_task_system_mode].receta_temp_on;
	task_system_dta_list[task_system_mode].receta_temp_off = task_system_dta_list[g_task_system_mode].receta_temp_off;
	if (task_system_mode == NORMAL){
		task_system_dta_list[task_system_mode].t_espera = task_system_dta_list[task_system_mode].receta_tiempo_espera;
		task_system_dta_list[task_system_mode].state = ST_SYS_NORMAL_IDLE;
		put_event_task_actuator(EV_ACT_LED_GREEN_ON, ID_ACT_LED_GREEN);

		put_event_task_display(0, 0, " ESTACION LISTA ");
		if (task_system_dta_list[task_system_mode].sistema_pausado) {
			put_event_task_display(0, 1, " MODO: PAUSADO  ");
		} else {
			put_event_task_display(0, 1, " MODO: ESPERA   ");
		}
	} else if (task_system_mode == SETUP){
		task_system_dta_list[task_system_mode].menu_linea_actual = 0;
		task_system_dta_list[task_system_mode].fase_info = 0;
		task_system_dta_list[task_system_mode].state = ST_SYS_SET_UP;
		put_event_task_actuator(EV_ACT_LED_YELLOW_ON, ID_ACT_LED_YELLOW);
	} else if (task_system_mode == MODO_ERROR){
		task_system_dta_list[task_system_mode].state = ST_SYS_ERROR;
		put_event_task_actuator(EV_ACT_LED_RED_ON, ID_ACT_LED_RED);
	}

	//Se hace el salto final para el modo que se elija
	g_task_system_mode = task_system_mode;
}

/********************** end of file ******************************************/
