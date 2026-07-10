/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"

#include "logger.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

#if 1 == LOGGER_CONFIG_ENABLE
static char logger_msg_buffer_[LOGGER_CONFIG_MAXLEN];
char* const logger_msg = logger_msg_buffer_;
int logger_msg_len;
#endif

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

#if 1 == LOGGER_CONFIG_USE_SEMIHOSTING
void logger_log_print_(char* const msg)
{
	printf(msg);
	fflush(stdout);
}
#else
void logger_log_print_(char* const msg)
{
    return;
}
#endif

/********************** end of file ******************************************/
