#ifndef BOARD_H_
#define BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h" // Incluye las definiciones generadas por el .ioc


// -------------------------------------------------
// 1. TECLADO MATRICIAL (Navegación y Control)
// -------------------------------------------------
#define BTN_MODE_PIN         GPIO_PIN_0
#define BTN_MODE_PORT        GPIOC
#define BTN_MODE_PRESSED     GPIO_PIN_RESET

#define BTN_CONF_PIN         GPIO_PIN_1
#define BTN_CONF_PORT        GPIOC
#define BTN_CONF_PRESSED     GPIO_PIN_RESET

#define BTN_NEXT_PIN          GPIO_PIN_2
#define BTN_NEXT_PORT         GPIOC
#define BTN_NEXT_PRESSED      GPIO_PIN_RESET

#define BTN_PREV_PIN          GPIO_PIN_3
#define BTN_PREV_PORT         GPIOC
#define BTN_PREV_PRESSED      GPIO_PIN_RESET

// -------------------------------------------------
// 2. MÓDULO DE RELÉS (Actuadores de Potencia)
// -------------------------------------------------
#define RLY_PUMP_PIN         GPIO_PIN_6
#define RLY_PUMP_PORT        GPIOA
#define RLY_PUMP_ON          GPIO_PIN_RESET
#define RLY_PUMP_OFF         GPIO_PIN_SET

#define RLY_FAN_PIN          GPIO_PIN_7
#define RLY_FAN_PORT         GPIOA
#define RLY_FAN_ON           GPIO_PIN_RESET
#define RLY_FAN_OFF          GPIO_PIN_SET

// Lazo cerrado (Feedback de los relés)
#define RLY_FB_PUMP_PIN      GPIO_PIN_0
#define RLY_FB_PUMP_PORT     GPIOB
#define RLY_FB_PUMP_CLOSED   GPIO_PIN_RESET

#define RLY_FB_FAN_PIN       GPIO_PIN_1
#define RLY_FB_FAN_PORT      GPIOB
#define RLY_FB_FAN_CLOSED    GPIO_PIN_RESET

// -------------------------------------------------
// 3. INDICADORES Y ALARMAS (LEDs y Buzzer)
// -------------------------------------------------
#define LED_GREEN_PIN        GPIO_PIN_10
#define LED_GREEN_PORT       GPIOC
#define LED_GREEN_ON         GPIO_PIN_SET
#define LED_GREEN_OFF        GPIO_PIN_RESET

#define LED_YELLOW_PIN       GPIO_PIN_11
#define LED_YELLOW_PORT      GPIOC
#define LED_YELLOW_ON        GPIO_PIN_SET
#define LED_YELLOW_OFF       GPIO_PIN_RESET

#define LED_RED_PIN          GPIO_PIN_12
#define LED_RED_PORT         GPIOC
#define LED_RED_ON           GPIO_PIN_SET
#define LED_RED_OFF          GPIO_PIN_RESET

#define BUZZER_PIN           GPIO_PIN_2
#define BUZZER_PORT          GPIOB
#define BUZZER_ON            GPIO_PIN_SET
#define BUZZER_OFF           GPIO_PIN_RESET

// ==============================================================================
// 4. SENSOR ANALÓGICO (Nivel de Agua)
// ==============================================================================
#define SENS_WATER_LVL_PIN   GPIO_PIN_1
#define SENS_WATER_LVL_PORT  GPIOA

// ==============================================================================
// 5. MÓDULO BLUETOOTH HM-10 (UART)
// Conexión cruzada: El TX del micro va al RX del módulo y viceversa
// ==============================================================================
#define BT_USART_TX_PIN      GPIO_PIN_6
#define BT_USART_TX_PORT     GPIOB

#define BT_USART_RX_PIN      GPIO_PIN_7
#define BT_USART_RX_PORT     GPIOB

// ==============================================================================
// 6. INTERFAZ VISUAL (Display)
// ==============================================================================
#define LCD_D4_PIN           GPIO_PIN_5
#define LCD_D4_PORT          GPIOB

#define LCD_D5_PIN           GPIO_PIN_4
#define LCD_D5_PORT          GPIOB

#define LCD_D6_PIN           GPIO_PIN_10
#define LCD_D6_PORT          GPIOB

#define LCD_D7_PIN           GPIO_PIN_8
#define LCD_D7_PORT          GPIOA

#define LCD_RS_PIN           GPIO_PIN_9
#define LCD_RS_PORT          GPIOA

#define LCD_EN_PIN           GPIO_PIN_7
#define LCD_EN_PORT          GPIOC

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
