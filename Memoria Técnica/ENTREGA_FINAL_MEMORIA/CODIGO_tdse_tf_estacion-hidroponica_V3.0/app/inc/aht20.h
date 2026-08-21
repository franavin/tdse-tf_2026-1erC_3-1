#ifndef AHT20_H_
#define AHT20_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/********************** external functions declaration ***********************/
extern void aht20_init(void);
extern void aht20_update(void); // Se debe llamar cada 1ms

extern float aht20_get_temperature(void);
extern float aht20_get_humidity(void);

#ifdef __cplusplus
}
#endif

#endif /* AHT20_H_ */
