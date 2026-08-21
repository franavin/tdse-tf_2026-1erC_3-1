#ifndef TASK_SYSTEM_ATTRIBUTE_H_
#define TASK_SYSTEM_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Events to excite Task System */
typedef enum task_system_ev {
			EV_SYS_TICK,            // Evento cíclico para temporizadores
			EV_SYS_LVL_CRIT,        // Tanque vacío
			EV_SYS_LVL_OK,          // Tanque con agua
			EV_SYS_MODE_DOWN,       // Botón Modo
			EV_SYS_CONF_DOWN,       // Botón Confirmar
			EV_SYS_NEXT_DOWN,        // Botón Arriba
			EV_SYS_PREV_DOWN,        // Botón Abajo
			EV_SYS_PUMP_FAULT,
			EV_SYS_FAN_FAULT,
			EV_SYS_IDLE             // Sin eventos
							} task_system_ev_t;

/* State of Task System */
typedef enum task_system_st {
			ST_SYS_NORMAL_IDLE,
			ST_SYS_NORMAL_WATERING,
			ST_SYS_SET_UP,
			ST_SYS_ERROR
							} task_system_st_t;

typedef struct
{
	uint32_t			tick;
	task_system_st_t	state;
	task_system_ev_t	event;
	bool				flag;
	uint32_t			t_espera;
	uint32_t			t_riego;
	uint32_t			t_timeout_rly;
	uint32_t			receta_tiempo_riego;
	uint32_t			receta_tiempo_espera;
	uint32_t			receta_temp_on;
	uint32_t			receta_temp_off;
	uint32_t			menu_linea_actual;
	uint32_t 			t_ciclo_info;
	uint32_t 			fase_info;
	uint32_t 			t_brillo;
	bool				sistema_pausado;
} task_system_dta_t;

/********************** external data declaration ****************************/
extern task_system_dta_t task_system_dta_list[];

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_SYSTEM_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
