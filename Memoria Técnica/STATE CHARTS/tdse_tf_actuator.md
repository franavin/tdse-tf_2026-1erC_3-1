## Estación Hidropónica Simulada - Módulo de Actuador
**-Módulo de código C:** Temporizado  
**-Período de actualización:** 1 ms
***************************************************************************************************************************************************************************************************************************
## 1. Descripción del Modelo
Este módulo ejecuta las acciones físicas en respuesta a las peticiones (los eventos) enviadas por el Módulo de Sistema.
Controla de manera centralizada la totalidad de los actuadores disponibles: el Display, los LEDs de estado, 
el módulo de los relés, el Buzzer, el módulo Bluetooth y una memoria EEPROM.

Dado que la arquitectura de software de la aplicación es estrictamente no bloqueante, las secuencias que requieren control temporal (como la alarma intermitente del 
buzzer, el titileo de los LEDs de advertencia o los tiempos de refresco de las pantallas) se resuelven mediante máquinas de estado internas que gestionan 
los "ticks" del sistema, evitando por completo el uso de funciones de retraso pasivo (`delay`). Las comunicaciones por buses (I2C, SPI y UART) se disparan 
como tareas asincrónicas delegadas a funciones específicas de los drivers de hardware de la placa de desarrollo.

***************************************************************************************************************************************************************************************************************************
## 2. Estados

### 2.1 Alarma Acústica (Buzzer)
* **`ST_ACT_BUZZ_IDLE`**: Reposo. El buzzer está en silencio.
* **`ST_ACT_BUZZ_BEEPING`**: Transitorio. En este punto el buzzer emite un tono de confirmación (por ej. éxito al guardar parámetros).
* **`ST_ACT_BUZZ_ERR_ON`**: Transitorio cíclico. El buzzer está encendido emitiendo el pulso activo de la alarma de falla.
* **`ST_ACT_BUZZ_ERR_OFF`**: Transitorio cíclico. El buzzer está apagado en el intervalo de silencio de la alarma de falla.

### 2.2 LEDs de Estado (Indicadores de Modo)
* **`ST_ACT_LED_GREEN`**: Operación normal de la estación hidropónica.
* **`ST_ACT_LED_YELLOW`**: Modo configuración activo (para el menú interactivo estático).
* **`ST_ACT_LED_BLINK_ON`**: Transitorio cíclico. El LED amarillo encendido está indicando una actualización o transición de datos en pantalla.
* **`ST_ACT_LED_BLINK_OFF`**: Transitorio cíclico. El LED amarillo apagado dentro de la secuencia de parpadeo temporal.
* **`ST_ACT_LED_RED`**: Estado seguro de falla crítica activa (el sistema está bloqueado).

### 2.3 Interfaz Visual (Display)
* **`ST_ACT_DISP_TELEMETRY`**: Muestra de forma periódica las lecturas del clima y el nivel de agua filtrado.
* **`ST_ACT_DISP_MENU`**: Muestra las líneas del menú interactivo local para la edición de las recetas de cultivo.
* **`ST_ACT_DISP_FAULT`**: Muestra una pantalla fija de error y el bloqueo de seguridad del sistema.

### 2.4 Actuadores de Potencia (Reles)
* **`ST_ACT_RLY_PUMP_OFF`** / **`ST_ACT_RLY_PUMP_ON`**: Canal del relé asignado a la bomba de agua desenergizado o energizado, respectivamente.
* **`ST_ACT_RLY_FAN_OFF`** / **`ST_ACT_RLY_FAN_ON`**: Canal del relé asignado al ventilador del clima desenergizado o energizado, respectivamente.

### 2.5 Periféricos de Bus (Bluetooth y EEPROM)
Mapean el estado de las transmisiones asincrónicas controladas por registros de comunicación:
* **`ST_ACT_COM_IDLE`**: Buses en reposo, esperando comandos de transmisión UART o ciclos I2C.
* **`ST_ACT_COM_BUSY`**: Envío de tramas de telemetría activa o las ráfagas de lectura/escritura en celdas de memoria.

***************************************************************************************************************************************************************************************************************************
## 3. Eventos
Son las órdenes y comandos estructurados que llegan directamente desde el módulo de procesamiento central:

* **`EV_ACT_PUMP_ON` / `EV_ACT_PUMP_OFF`**: Son las ordenes para conmutar el canal de relé de la bomba de agua.
* **`EV_ACT_FAN_ON` / `EV_ACT_FAN_OFF`**: Se da la orden para conmutar el canal de relé del ventilador del clima.
* **`EV_ACT_ALARM_ON` / `EV_ACT_ALARM_OFF`**: Llevan las peticiones del sistema para poder iniciar o detener la alarma acústica intermitente.
* **`EV_ACT_BEEP_SUCCESS`**: Es la petición para emitir un pitido corto de confirmación en el buzzer.
* **`EV_ACT_MENU_SHOW`**: Comando del sistema para cargar la pantalla del menú y pasar todos los indicadores a modo configuración.
* **`EV_ACT_MENU_UPDATE`**: Es la petición necesaria para refrescar los valores numéricos editados y forzar el parpadeo de confirmación.
* **`EV_ACT_MENU_UP` / `EV_ACT_MENU_DOWN`**: Los comandos de navegación para desplazar el cursor del menú por las líneas del display.
* **`EV_ACT_ERROR_SHOW`**: El comando que necesitamos para forzar la visualización de la pantalla de error y cambiar indicadores a modo seguro de falla.
* **`EV_ACT_EEPROM_LOAD` / `EV_ACT_EEPROM_SAVE`**: Para cuando se realizan las transferencias físicas de lectura o escritura en la memoria EEPROM.

***************************************************************************************************************************************************************************************************************************
## 4. Variables de Control
Para gestionar las acciones y efectos en función del tiempo dentro del módulo, se definen los contadores decrementales ligados a la base de tiempo de la aplicación:
* **`tick`**: Variable temporizadora de cuenta regresiva.
* **`DEL_BEEP_MAX`**: Constante de tiempo para la duración del pitido corto de confirmación (por ej. 200 ms).
* **`DEL_ALARM_TOGGLE`**: Constante de intervalo para el parpadeo sonoro de la alarma de falla (por ej. 500 ms).
* **`DEL_LED_BLINK`**: Constante de intervalo para el parpadeo visual del LED de estado (por ej. 300 ms).

***************************************************************************************************************************************************************************************************************************
## 5. Acciones

### Acciones Internas
Controlan el hardware directo en los registros del microcontrolador y los temporizadores propios:
* **`tick = DEL_XXX_MAX`**: Inicialización de los contadores regresivos locales.
* **`tick--`**: Decremento de la variable de control en cada ciclo de ejecución de 1 ms.
* **`BUZZER_ON` / `BUZZER_OFF`**: Control de la señal digital o PWM asignada al transductor acústico.
* **`RLY_PUMP_ON` / `RLY_PUMP_OFF` / `RLY_FAN_ON` / `RLY_FAN_OFF`**: Escritura lógica directa en los pines GPIO del módulo de relés.
* **`LED_GREEN_ON` / `LED_YELLOW_ON` / `LED_RED_ON` / `LEDS_OFF`**: Gestión de los pines analógicos o digitales de los LEDs indicadores.
* **`DISPLAY_DRAW_TELEMETRY` / `DISPLAY_DRAW_MENU` / `DISPLAY_DRAW_ERROR`**: Actualización del buffer de pantalla a transmitir vía SPI/I2C.
* **`DISPLAY_MOVE_CURSOR`**: Desplazamiento para el indicador visual de la línea seleccionada en la pantalla.
* **`I2C_EEPROM_TX` / `I2C_EEPROM_RX`**: Llamadas a las rutinas de transmisión no bloqueantes del periférico I2C.
* **`UART_HM10_TX`**: Disparo de la ráfaga de datos seriales hacia el módulo Bluetooth para la app del celular.

***************************************************************************************************************************************************************************************************************************
## 6. Actuator Statechart - State Transition Table

### 6.1 Máquina de Estados del Buzzer (Temporizada)
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_ACT_BUZZ_IDLE** | EV_ACT_BEEP_SUCCESS | | ST_ACT_BUZZ_BEEPING | BUZZER_ON; tick = DEL_BEEP_MAX |
| | EV_ACT_ALARM_ON | | ST_ACT_BUZZ_ERR_ON | BUZZER_ON; tick = DEL_ALARM_TOGGLE |
| **ST_ACT_BUZZ_BEEPING** | | tick > 0 | ST_ACT_BUZZ_BEEPING | tick-- |
| | | tick == 0 | ST_ACT_BUZZ_IDLE | BUZZER_OFF |
| **ST_ACT_BUZZ_ERR_ON** | EV_ACT_ALARM_OFF | | ST_ACT_BUZZ_IDLE | BUZZER_OFF |
| | | tick > 0 | ST_ACT_BUZZ_ERR_ON | tick-- |
| | | tick == 0 | ST_ACT_BUZZ_ERR_OFF | BUZZER_OFF; tick = DEL_ALARM_TOGGLE |
| **ST_ACT_BUZZ_ERR_OFF**| EV_ACT_ALARM_OFF | | ST_ACT_BUZZ_IDLE | BUZZER_OFF |
| | | tick > 0 | ST_ACT_BUZZ_ERR_OFF | tick-- |
| | | tick == 0 | ST_ACT_BUZZ_ERR_ON | BUZZER_ON; tick = DEL_ALARM_TOGGLE |

### 6.2 Máquina de Estados de los LEDs de Estado (Temporizada)
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_ACT_LED_GREEN** | EV_ACT_MENU_SHOW | | ST_ACT_LED_YELLOW | LED_YELLOW_ON |
| | EV_ACT_ERROR_SHOW | | ST_ACT_LED_RED | LED_RED_ON |
| **ST_ACT_LED_YELLOW**| EV_ACT_MENU_UPDATE | | ST_ACT_LED_BLINK_ON | LED_YELLOW_ON; tick = DEL_LED_BLINK |
| | EV_ACT_PUMP_OFF *(Regreso)*| | ST_ACT_LED_GREEN | LED_GREEN_ON |
| **ST_ACT_LED_BLINK_ON**| | tick > 0 | ST_ACT_LED_BLINK_ON | tick-- |
| | | tick == 0 | ST_ACT_LED_BLINK_OFF| LEDS_OFF; tick = DEL_LED_BLINK |
| **ST_ACT_LED_BLINK_OFF**|| tick > 0 | ST_ACT_LED_BLINK_OFF| tick-- |
| | | tick == 0 | ST_ACT_LED_YELLOW | LED_YELLOW_ON |
| **ST_ACT_LED_RED** | EV_ACT_PUMP_OFF *(Reset)*| | ST_ACT_LED_GREEN | LED_GREEN_ON |

### 6.3 Máquina de Estados de la Interfaz Visual (Display OLED)
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_ACT_DISP_TELEMETRY**| EV_ACT_MENU_SHOW | | ST_ACT_DISP_MENU | DISPLAY_DRAW_MENU |
| | EV_ACT_ERROR_SHOW | | ST_ACT_DISP_FAULT | DISPLAY_DRAW_ERROR |
| **ST_ACT_DISP_MENU** | EV_ACT_MENU_UP | | ST_ACT_DISP_MENU | DISPLAY_MOVE_CURSOR |
| | EV_ACT_MENU_DOWN | | ST_ACT_DISP_MENU | DISPLAY_MOVE_CURSOR |
| | EV_ACT_MENU_UPDATE | | ST_ACT_DISP_MENU | DISPLAY_DRAW_MENU |
| | EV_ACT_PUMP_OFF *(Regreso)*| | ST_ACT_DISP_TELEMETRY| DISPLAY_DRAW_TELEMETRY |
| **ST_ACT_DISP_FAULT** | EV_ACT_PUMP_OFF *(Reset)*| | ST_ACT_DISP_TELEMETRY| DISPLAY_DRAW_TELEMETRY |

### 6.4 Máquina de Estados de los Relés (Bomba de Agua)
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_ACT_RLY_PUMP_OFF**| EV_ACT_PUMP_ON | | ST_ACT_RLY_PUMP_ON | RLY_PUMP_ON; UART_HM10_TX |
| **ST_ACT_RLY_PUMP_ON** | EV_ACT_PUMP_OFF | | ST_ACT_RLY_PUMP_OFF| RLY_PUMP_OFF; UART_HM10_TX |

### 6.5 Máquina de Estados de los Relés (Ventilador de Clima)
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_ACT_RLY_FAN_OFF** | EV_ACT_FAN_ON | | ST_ACT_RLY_FAN_ON | RLY_FAN_ON; UART_HM10_TX |
| **ST_ACT_RLY_FAN_ON** | EV_ACT_FAN_OFF | | ST_ACT_RLY_FAN_OFF | RLY_FAN_OFF; UART_HM10_TX |

### 6.6 Control de Periféricos de Almacenamiento (Memoria EEPROM)
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_ACT_COM_IDLE** | EV_ACT_EEPROM_LOAD | | ST_ACT_COM_BUSY | I2C_EEPROM_RX |
| | EV_ACT_EEPROM_SAVE | | ST_ACT_COM_BUSY | I2C_EEPROM_TX |
| **ST_ACT_COM_BUSY** | *(Transferencia Completa)* | | ST_ACT_COM_IDLE | |
