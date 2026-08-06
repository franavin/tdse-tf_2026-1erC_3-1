#ifndef TASK_DISPLAY_ATTRIBUTE_H_
#define TASK_DISPLAY_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Display Data RAM */
#define ROWS 	2
#define COLUMNS 16

/* Events to excite Task Display */
typedef enum task_display_ev {EV_DSP_IDLE,
							  EV_DSP_UPDATE} task_display_ev_t;

/* State of Task Display */
typedef enum task_display_st {ST_DSP_IDLE,
							  ST_DSP_UPDATE} task_display_st_t;

typedef struct
{
	uint32_t			tick;
	task_display_st_t	state;
	task_display_ev_t	event;
	bool				flag;
	char 				ddram[ROWS][COLUMNS+1]; //buffer
	uint32_t			row;
	uint32_t			column;
	char				character;
} task_display_dta_t;

/********************** external data declaration ****************************/
extern task_display_dta_t task_display_dta;

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_DISPLAY_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
