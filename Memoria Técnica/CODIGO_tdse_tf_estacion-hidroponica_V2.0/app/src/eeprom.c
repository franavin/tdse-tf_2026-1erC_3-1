/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"

/* Application & Tasks includes */
#include "eeprom.h"

/********************** macros and definitions *******************************/
/* Dirección I2C base típica para EEPROM */
#define EEPROM_DEV_ADDR         0xA0

/* Las memorias > 2Kbits usan direcciones de memoria de 16 bits */
#define EEPROM_MEMADD_SIZE      I2C_MEMADD_SIZE_16BIT
#define EEPROM_TIMEOUT          100ul
#define EEPROM_WRITE_DELAY_MS   5ul

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
/* Vinculación con I2C1 que generamos antes */
extern I2C_HandleTypeDef hi2c1;

/********************** external data declaration ****************************/

/********************** external functions definition ************************/

void eeprom_init(void)
{
    LOGGER_INFO("  %s is running", GET_NAME(eeprom_init));

    /* Comprobación para ver si el dispositivo EEPROM responde en bus I2C */
		if (HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_DEV_ADDR, 3, EEPROM_TIMEOUT) == HAL_OK)
		{
			LOGGER_INFO("   EEPROM Found on I2C bus");
		}
		else
		{
			LOGGER_INFO("   ERROR: EEPROM NOT Found");
		}
	}

void eeprom_write_uint32(uint16_t mem_addr, uint32_t data)
{
    uint8_t buffer[4];

    /* se desglosa la variable de 32 bits en 4 bytes de 8 bits */
    buffer[0] = (uint8_t)((data >> 24) & 0xFF);
    buffer[1] = (uint8_t)((data >> 16) & 0xFF);
    buffer[2] = (uint8_t)((data >> 8) & 0xFF);
    buffer[3] = (uint8_t)(data & 0xFF);

		/* Enviamos los 4 bytes completos una sola vez a la memoria */
		if (HAL_I2C_Mem_Write(&hi2c1, EEPROM_DEV_ADDR, mem_addr, EEPROM_MEMADD_SIZE, buffer, 4, EEPROM_TIMEOUT) == HAL_OK)
		{
			/* Agregamos un delay porque la memoria tarda unos ms en escribir los datos  */
			HAL_Delay(EEPROM_WRITE_DELAY_MS);
		}
		else
		{
			LOGGER_INFO("   ERROR: eeprom_write_uint32 failed");
		}
}

uint32_t eeprom_read_uint32(uint16_t mem_addr)
{
    uint8_t buffer[4];
    uint32_t data = 0xFFFFFFFF; // Valor por defecto si falla la lectura

    /* Leemos los 4bytes consecutivos desde la dirección que se pide */
    if (HAL_I2C_Mem_Read(&hi2c1, EEPROM_DEV_ADDR, mem_addr, EEPROM_MEMADD_SIZE, buffer, 4, EEPROM_TIMEOUT) == HAL_OK)
    {
        /* Volvemos a juntar los datos en una variable de 32bits */
        data = ((uint32_t)buffer[0] << 24) |
               ((uint32_t)buffer[1] << 16) |
               ((uint32_t)buffer[2] << 8)  |
               ((uint32_t)buffer[3]);
    }
    else
    {
        LOGGER_INFO("   ERROR: eeprom_read_uint32 failed");
    }

    return data;
}

/********************** end of file ******************************************/
