/* Project includes. */
#include "main.h"


/* App includes. */
#include "app.h"
#include "logger.h"

/* Application & Tasks includes */
#include "board.h"
#include "app_it.h"
#include "task_sensor.h"
#include "task_system.h"
#include "task_actuator.h"
#include "task_display.h"
#include "bluetooth.h"

/********************** macros and definitions *******************************/
#define TASK_X_NOE_INI      0ul
#define TASK_X_LET_INI      0ul
#define TASK_X_BCET_INI     1000ul
#define TASK_X_WCET_INI     0ul

typedef struct {
    void (*task_init)(void *);      // Puntero a la función de inicialización
    void (*task_update)(void *);    // Puntero a la función de actualización
    void *parameters;               // Parámetros adicionales
} task_cfg_t;

typedef struct {
    uint32_t NOE;       // Number of execution (numeral)
    uint32_t LET;       // Last execution time (microseconds)
    uint32_t BCET;      // Best-case execution time (microseconds)
    uint32_t WCET;      // Worst-case execution time (microseconds)
} task_dta_t;

/********************** internal data declaration ****************************/

const task_cfg_t task_cfg_list[] = {
    {task_sensor_init,      task_sensor_update,     NULL},
    {task_system_init,      task_system_update,     NULL},
    {task_actuator_init,    task_actuator_update,   NULL},
    {task_display_init,     task_display_update,    NULL},
    {bluetooth_init,	bluetooth_update,			NULL}
};

#define TASK_QTY    (sizeof(task_cfg_list)/sizeof(task_cfg_t))

/********************** internal data definition *****************************/

const char *p_app   = "Estacion Hidroponica V1";
const char *p_app_  = "Bare Metal - Event-Triggered Systems (ETS)";
const char *p_app__ = "(Update by Time Code, period = 1mS)";

/********************** external data declaration ****************************/
uint32_t g_app_cnt;
uint32_t g_app_runtime_us;

task_dta_t task_dta_list[TASK_QTY];

/********************** external functions definition ************************/
void app_init(void)
{
    uint32_t index;

    LOGGER_INFO(" ");
    LOGGER_INFO("%s is running - Tick [mS] = %lu", GET_NAME(app_init), HAL_GetTick());

    LOGGER_INFO(" %s is a %s", GET_NAME(app), p_app);
    LOGGER_INFO(" %s is a %s", GET_NAME(app), p_app_);
    LOGGER_INFO(" %s is a %s", GET_NAME(app), p_app__);

    /* Init & Print out: Application execution counter */
    g_app_cnt = 0;
    LOGGER_INFO(" %s = %lu", GET_NAME(g_app_cnt), g_app_cnt);

    /* Inicializar el contador de ciclos del procesador ARM (DWT) */
    cycle_counter_init();

    /* Recorrer e inicializar todas las tareas de la lista */
    for (index = 0; TASK_QTY > index; index++)
    {
        /* Ejecuta la función task_XXX_init() correspondiente */
        (*task_cfg_list[index].task_init)(task_cfg_list[index].parameters);

        /* Inicializar variables de medición de tiempo */
        task_dta_list[index].NOE = TASK_X_NOE_INI;
        task_dta_list[index].LET = TASK_X_LET_INI;
        task_dta_list[index].BCET = TASK_X_BCET_INI;
        task_dta_list[index].WCET = TASK_X_WCET_INI;
    }

    /* Inicializar las interrupciones de la aplicación (SysTick) */
    app_it_init();
}

void app_update(void)
{
    uint32_t index;
    bool b_time_update_required = false;

    /* Se desactivan las interrupciones temporalmente */
    __asm("CPSID i");
    if (0 < g_app_tick_cnt)
    {
        /* se descuenta el Tick Counter que aumenta en el SysTick */
        g_app_tick_cnt--;
        b_time_update_required = true;
    }
    __asm("CPSIE i");

    /* Si pasó 1 ms que arranque el ciclo */
    while (b_time_update_required)
    {
        g_app_cnt++;
        g_app_runtime_us = 0;

        /* Recorrer y actualizar todas las tareas */
        for (index = 0; TASK_QTY > index; index++)
        {
            cycle_counter_reset();

            /* Ejecuta la función task_XXX_update() correspondiente */
            (*task_cfg_list[index].task_update)(task_cfg_list[index].parameters);

            /* Registrar métricas de tiempo de ejecución de la tarea */
            task_dta_list[index].NOE++;
            task_dta_list[index].LET = cycle_counter_get_time_us();

            if (task_dta_list[index].BCET > task_dta_list[index].LET)
            {
                task_dta_list[index].BCET = task_dta_list[index].LET;
            }

            if (task_dta_list[index].WCET < task_dta_list[index].LET)
            {
                task_dta_list[index].WCET = task_dta_list[index].LET;
            }

            g_app_runtime_us += task_dta_list[index].LET;
        }

        /* --- AHORRO DE ENERGÍA --- */
        /* Apaga el procesador hasta el próximo tick de 1ms para ahorrar consumo */
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

        /* Aca vuelve a revisar si hay ticks antes de apagarse*/
        __asm("CPSID i");
        if (0 < g_app_tick_cnt)
        {
            g_app_tick_cnt--;
            b_time_update_required = true;
        }
        else
        {
            b_time_update_required = false;
        }
        __asm("CPSIE i");
    }
}

/********************** end of file ******************************************/

