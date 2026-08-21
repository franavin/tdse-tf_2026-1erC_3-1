#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/********************** external functions declaration ***********************/
extern void bluetooth_init(void);
extern void bluetooth_update(void);
void bluetooth_send_alert(const char *msg);

#ifdef __cplusplus
}
#endif

#endif /* BLUETOOTH_H_ */
