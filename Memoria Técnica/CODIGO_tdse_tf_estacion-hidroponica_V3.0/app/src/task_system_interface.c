
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

/********************** macros and definitions *******************************/
#define EMPTY			(255ul)
#define QUEUE_LENGTH	(16ul)
#define ITEM_SIZE		(sizeof(task_system_ev_t))

typedef struct
{
	uint32_t			head;
	uint32_t			tail;
	uint32_t			count;
	task_system_ev_t	queue[QUEUE_LENGTH];
} event_task_system_queue_t;

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
event_task_system_queue_t event_task_system_queue;

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
void init_event_task_system(void)
{
	uint32_t i;

	event_task_system_queue.head = 0;
	event_task_system_queue.tail = 0;
	event_task_system_queue.count = 0;

	for (i = 0; i < QUEUE_LENGTH; i++)
		event_task_system_queue.queue[i] = EMPTY;
}

void put_event_task_system(task_system_ev_t event)
{
	event_task_system_queue.count++;
	event_task_system_queue.queue[event_task_system_queue.head++] = event;

	if (QUEUE_LENGTH == event_task_system_queue.head)
		event_task_system_queue.head = 0;
}

task_system_ev_t get_event_task_system(void)
{
	task_system_ev_t event;

	event_task_system_queue.count--;
	event = event_task_system_queue.queue[event_task_system_queue.tail];
	event_task_system_queue.queue[event_task_system_queue.tail++] = EMPTY;

	if (QUEUE_LENGTH == event_task_system_queue.tail)
		event_task_system_queue.tail = 0;

	return event;
}

bool any_event_task_system(void)
{
  return (event_task_system_queue.head != event_task_system_queue.tail);
}

/********************** end of file ******************************************/
