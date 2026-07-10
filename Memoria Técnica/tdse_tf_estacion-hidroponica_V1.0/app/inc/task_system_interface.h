#ifndef TASK_SYSTEM_INTERFACE_H_
#define TASK_SYSTEM_INTERFACE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
extern void init_event_task_system(void);
extern void put_event_task_system(task_system_ev_t event);
extern task_system_ev_t get_event_task_system(void);
extern bool any_event_task_system(void);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_SYSTEM_INTERFACE_H_ */

/********************** end of file ******************************************/
