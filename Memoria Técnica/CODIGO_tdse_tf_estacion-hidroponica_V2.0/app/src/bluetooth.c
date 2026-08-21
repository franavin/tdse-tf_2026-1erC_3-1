
#include "main.h"
#include "logger.h"
#include "bluetooth.h"
#include "task_system_attribute.h"
#include "eeprom.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern UART_HandleTypeDef huart1;

#define BT_BUFFER_SIZE 32

// buffer
static uint8_t rx_byte;
static char rx_buffer[BT_BUFFER_SIZE];
static uint8_t rx_index = 0;
static bool msg_ready = false;

// ----------------------------------------------------------------
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if (rx_byte == '\n' || rx_byte == '\r')
        {
            if (rx_index > 0) {
                rx_buffer[rx_index] = '\0'; // Terminamos el string
                msg_ready = true;           // Levantamos la bandera para procesar
            }
        }
        else
        {
            // Guardamos la letra si hay espacio en el buffer
            if (rx_index < BT_BUFFER_SIZE - 1) {
                rx_buffer[rx_index++] = (char)rx_byte;
            }
        }

        // Volvemos para ver la siguiente letra
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}

void bluetooth_init(void)
{
    LOGGER_INFO("  %s is running", GET_NAME(bluetooth_init));

    rx_index = 0;
    msg_ready = false;
    memset(rx_buffer, 0, BT_BUFFER_SIZE);

    // Iniciamos la escucha por interrupción de la primera letra
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}

void bluetooth_update(void)
{
    // Solo procesamos si llegó un mensaje completo ('\n')
    if (msg_ready)
    {
        uint32_t nuevo_valor = 0;
        char tx_msg[50];

        // 1. Comando de Riego: Ejemplo "R=5000"
        if (strncmp(rx_buffer, "R=", 2) == 0)
        {
            nuevo_valor = atoi(&rx_buffer[2]); // Convertimos el texto a número
            if (nuevo_valor >= 1000) // Validación de seguridad (mínimo 1 seg)
            {
                // Se ctualizan todas las statecharts
                for(int i=0; i<3; i++) {
                    task_system_dta_list[i].receta_tiempo_riego = nuevo_valor;
                }
                eeprom_write_uint32(0x00, nuevo_valor); // Guardamos permanentemente

                snprintf(tx_msg, sizeof(tx_msg), "OK! Riego set: %lu ms\r\n", nuevo_valor);
                HAL_UART_Transmit(&huart1, (uint8_t*)tx_msg, strlen(tx_msg), 100);
            }
        }
        // Espera
        else if (strncmp(rx_buffer, "E=", 2) == 0)
        {
            nuevo_valor = atoi(&rx_buffer[2]);
            if (nuevo_valor >= 1000)
            {
                for(int i=0; i<3; i++) {
                    task_system_dta_list[i].receta_tiempo_espera = nuevo_valor;
                }
                eeprom_write_uint32(0x04, nuevo_valor);

                snprintf(tx_msg, sizeof(tx_msg), "OK! Espera set: %lu ms\r\n", nuevo_valor);
                HAL_UART_Transmit(&huart1, (uint8_t*)tx_msg, strlen(tx_msg), 100);
            }
        }
        else
        {
            // Comando no reconocido
            snprintf(tx_msg, sizeof(tx_msg), "Error: Comando invalido\r\n");
            HAL_UART_Transmit(&huart1, (uint8_t*)tx_msg, strlen(tx_msg), 100);
        }

        // Limpiamos el búfer para el próximo mensaje
        rx_index = 0;
        msg_ready = false;
        memset(rx_buffer, 0, BT_BUFFER_SIZE);
    }
}
