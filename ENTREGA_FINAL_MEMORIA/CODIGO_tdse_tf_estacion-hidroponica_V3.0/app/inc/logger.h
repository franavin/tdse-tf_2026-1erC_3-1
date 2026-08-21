#ifndef LOGGER_H_
#define LOGGER_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/********************** macros ***********************************************/

#define LOGGER_CONFIG_ENABLE                    (1)
#define LOGGER_CONFIG_MAXLEN                    (64)
#define LOGGER_CONFIG_USE_SEMIHOSTING           (1)

#if 1 == LOGGER_CONFIG_ENABLE
#define LOGGER_LOG(...)\
	__asm("CPSID i");	/* disable interrupts*/\
    {\
        logger_msg_len = snprintf(logger_msg, (LOGGER_CONFIG_MAXLEN - 1), __VA_ARGS__);\
        logger_log_print_(logger_msg);\
    }\
	__asm("CPSIE i");	/* enable interrupts*/
#else
#define LOGGER_LOG(...)
#endif

#if 1 == LOGGER_CONFIG_ENABLE
#define LOGGER_INFO(...)\
	__asm("CPSID i");	/* disable interrupts*/\
	{\
		logger_log_print_("[info] ");\
    	logger_msg_len = snprintf(logger_msg, (LOGGER_CONFIG_MAXLEN - 1), __VA_ARGS__);\
    	logger_log_print_(logger_msg);\
    	logger_log_print_("\n");\
	}\
	__asm("CPSIE i");	/* enable interrupts*/
#else
#define LOGGER_INFO(...)
#endif

#define GET_NAME(var)  #var

/********************** typedef **********************************************/

extern char* const logger_msg;
extern int logger_msg_len; // only for debug information

/********************** external functions declaration ***********************/

void logger_log_print_(char* const msg);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* LOGGER_H_ */

/********************** end of file ******************************************/
