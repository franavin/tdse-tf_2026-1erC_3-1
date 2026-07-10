/*
Implementamos una máquina de estados para el sensor de temperatura y humedad por separado
 */

#include "main.h"
#include "logger.h"
#include "aht20.h"

// Dirección I2C del AHT20 (0x38 desplazado 1 bit a la izquierda)
#define AHT20_ADDR          (0x38 << 1)
#define AHT20_TIMEOUT       10ul

// Tiempos de la máquina de estados
#define AHT20_DELAY_MEASURE 80ul    // El sensor tarda ~80ms en medir
#define AHT20_DELAY_CYCLE   2000ul  // Volvemos a medir cada 2 segundos

extern I2C_HandleTypeDef hi2c1; // Referencia al I2C que configuramos anteriormente

// Estados de la máquina interna del AHT20
typedef enum {
    AHT_ST_IDLE,
    AHT_ST_WAIT_MEASUREMENT
} aht20_state_t;

// Variables internas
static aht20_state_t state = AHT_ST_IDLE;
static uint32_t tick = 0;
static float temperature = 0.0f;
static float humidity = 0.0f;

void aht20_init(void)
{
    LOGGER_INFO("  %s is running", GET_NAME(aht20_init));

    // El AHT20 requiere un comando de inicialización al encender
    uint8_t init_cmd[3] = {0xBE, 0x08, 0x00};
    HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDR, init_cmd, 3, AHT20_TIMEOUT);

    state = AHT_ST_IDLE;
    tick = 100; // Se recomiendan 100ms iniciales de estabilización
}

void aht20_update(void)
{
    uint8_t cmd[3];
    uint8_t data[6];
    uint32_t raw_h, raw_t;

    switch (state)
    {
        case AHT_ST_IDLE:
            if (tick > 0) {
                tick--;
            }
            if (tick == 0) {
                // 1. Enviar comando de Trigger Measurement
                cmd[0] = 0xAC;
                cmd[1] = 0x33;
                cmd[2] = 0x00;

                if (HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDR, cmd, 3, AHT20_TIMEOUT) == HAL_OK) {
                    // Si se envió bien, pasamos a esperar que mida (sin bloquear)
                    tick = AHT20_DELAY_MEASURE; // 80 ms
                    state = AHT_ST_WAIT_MEASUREMENT;
                } else {
                    // Si falló (cable desconectado), reintentar en 1 segundo
                    tick = 1000;
                }
            }
            break;

        case AHT_ST_WAIT_MEASUREMENT:
            if (tick > 0) {
                tick--;
            }
            if (tick == 0) {
                // Pasaron los 80ms, y ya podemos leer los datos
                if (HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDR, data, 6, AHT20_TIMEOUT) == HAL_OK) {

                    // Comprobar que el bit de ocupado (Bit 7) esté en 0
                    if ((data[0] & 0x80) == 0) {
                        // Cálculos según buscamos en el datasheet del sensesor AHT20
                        raw_h = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | ((uint32_t)data[3] >> 4);
                        raw_t = (((uint32_t)data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | (uint32_t)data[5];

                        humidity = ((float)raw_h * 100.0f) / 1048576.0f;
                        temperature = ((float)raw_t * 200.0f) / 1048576.0f - 50.0f;
                    }
                }

                // Volver a medir dentro de 2 segundos para no calentar el sensor
                tick = AHT20_DELAY_CYCLE;
                state = AHT_ST_IDLE;
            }
            break;

        default:
            state = AHT_ST_IDLE;
            break;
    }
}

float aht20_get_temperature(void)
{
    return temperature;
}

float aht20_get_humidity(void)
{
    return humidity;
}
