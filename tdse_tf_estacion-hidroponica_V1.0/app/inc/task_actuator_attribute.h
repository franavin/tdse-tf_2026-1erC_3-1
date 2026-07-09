
#ifndef TASK_ACTUATOR_ATTRIBUTE_H_
#define TASK_ACTUATOR_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Events to excite Task Actuator */
typedef enum task_actuator_ev {
								EV_ACT_IDLE,
								EV_ACT_PUMP_ON,       EV_ACT_PUMP_OFF,
								EV_ACT_FAN_ON,        EV_ACT_FAN_OFF,
								EV_ACT_LED_GREEN_ON,  EV_ACT_LED_YELLOW_ON,
								EV_ACT_LED_RED_ON,    EV_ACT_LED_OFF,
								EV_ACT_ALARM_ON,      EV_ACT_ALARM_OFF,
								EV_ACT_BEEP_SUCCESS
								} task_actuator_ev_t;
/* States of Task Actuator */
typedef enum task_actuator_st {
								ST_ACT_OFF,
								ST_ACT_ON,
								ST_ACT_BLINKING_ON,   // Transitorio (Alarma Sonora/Visual activa)
								ST_ACT_BLINKING_OFF,  // Transitorio (Alarma Sonora/Visual silencio)
								ST_ACT_BEEPING        // Transitorio (Pitido corto)
								} task_actuator_st_t;

/* Identifier of Task Actuator */
typedef enum task_actuator_id {
								ID_ACT_RLY_PUMP,
								ID_ACT_RLY_FAN,
								ID_ACT_LED_GREEN,
								ID_ACT_LED_YELLOW,
								ID_ACT_LED_RED,
								ID_ACT_BUZZER
								} task_actuator_id_t;

typedef struct
{
	task_actuator_id_t	identifier;
	GPIO_TypeDef *		gpio_port;
	uint16_t			pin;
	GPIO_PinState		state_on;
	GPIO_PinState		state_off;
	uint32_t			tick_max;
} task_actuator_cfg_t;

typedef struct
{
	uint32_t			tick;
	task_actuator_st_t	state;
	task_actuator_ev_t	event;
	bool				flag;
} task_actuator_dta_t;

/********************** external data declaration ****************************/
extern task_actuator_dta_t task_actuator_dta_list[];

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_ACTUATOR_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
