#ifndef EEPROM_H_
#define EEPROM_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdint.h>
#include <stdbool.h>

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
extern void eeprom_init(void);
extern void eeprom_write_uint32(uint16_t mem_addr, uint32_t data);
extern uint32_t eeprom_read_uint32(uint16_t mem_addr);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* EEPROM_H_ */

/********************** end of file ******************************************/
