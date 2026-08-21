## Estación Hidropónica Simulada - Módulo de Sistema
**-Módulo de código C:** Temporizado  
**-Período de escrutinio:** 1 ms
***************************************************************************************************************************************************************************************************************************
## 1. Descripción del Modelo
Procesa aquellas señales recibidas del módulo de sensores y envía señales de orden al módulo con los actuadores. 
Es aquí donde se constituye el núcleo lógico de la aplicación, implementando una Máquina de Estados que gestiona 
los modos de operación generales de la estación (NORMAL, SET_UP, FALLA). 

Además, gestiona de manera interna variables de control no bloqueantes (temporizadores) para determinar los ciclos de riego y esperas, 
utilizando estas variables como condiciones de guarda (`[guard]`) para autorizar o forzar las transiciones entre estados. Finalmente, 
valida en lazo cerrado el funcionamiento del hardware esperando el feedback de los relés al encender las bombas.


***************************************************************************************************************************************************************************************************************************
## 2. Estados
* **`ST_SYS_NORMAL_IDLE`**: Estado de reposo dentro de la operación normal, es donde se monitorea el clima y descuenta el tiempo de espera hasta el próximo riego.
* **`ST_SYS_NORMAL_WATERING`**: Estado transitorio de riego activo, se envía la orden de encender la bomba y descuenta el tiempo de riego.
* **`ST_SYS_SET_UP`**: Estado de configuración para la estación. El ciclo automático se detiene y nos permite la navegación por el menú interactivo.
* **`ST_SYS_ERROR`**: Estado seguro de falla (puede ser por tanque vacío o falla detectada en el relé). El sistema se bloquea y activa alarmas.
  
## 3. Eventos
Señales generadas y recibidas desde el Módulo Sensor ("Escrutar"):
* **`EV_SYS_LVL_CRIT` / `EV_SYS_LVL_OK`**: El nivel de agua analógico cayó a valores críticos o se normalizó, respectivamente.
* **`EV_SYS_TEMP_HIGH` / `EV_SYS_TEMP_OK`**: La temperatura superó el umbral configurado o retornó a valores seguros.
* **`EV_SYS_MODE_DOWN`**: El usuario presionó la tecla correspondiente al cambio de modo (`#`).
* **`EV_SYS_CONF_DOWN`**: El usuario presionó la tecla correspondiente a la confirmación/guardado (`*`).
* **`EV_SYS_NEXT_DOWN`**: El usuario presionó la tecla de navegación hacia arriba (`2`).
* **`EV_SYS_PREV_DOWN`**: El usuario presionó la tecla de navegación hacia abajo (`8`).
* **`EV_SYS_RLY_CLOSED`**: Señal de feedback desde el actuador indicando que el contacto del relé cerró físicamente.
* **`EV_SYS_TICK`**: Evento interno provisto por el ejecutor cíclico en cada vuelta para actualizar los temporizadores del sistema.

## 4. Variables de Control (Timers / Guards)
Variables decrementales (`uint32_t`) modificadas por el sistema y usadas como `[guard]` para habilitar transiciones:
* **`t_espera`**: Controla el tiempo entre riegos.
* **`t_riego`**: Controla la duración del riego encendido.
* **`t_timeout_rly`**: Temporizador de seguridad para verificar si el relé respondió a tiempo.

## 5. Acciones
### Acciones Internas
* **`t_espera--` / `t_riego--` / `t_timeout_rly--`**: Decremento de variables temporizadoras asociado al evento `EV_SYS_TICK`.
* **`t_espera = MAX_ESPERA` / `t_riego = MAX_RIEGO` / `t_timeout_rly = MAX_TIMEOUT`**: Inicialización y reseteo de los tiempos.
* **`t_timeout_rly = -1`**: Desactiva la cuenta regresiva de seguridad (se ejecuta cuando llega el feedback correcto).

### Acciones Externas (Signals al Módulo Actuador)
* **`raise EV_ACT_PUMP_ON` / `raise EV_ACT_PUMP_OFF`**: Orden para encender o apagar la bomba de agua.
* **`raise EV_ACT_FAN_ON` / `raise EV_ACT_FAN_OFF`**: Orden para encender o apagar el ventilador.
* **`raise EV_ACT_ALARM_ON` / `raise EV_ACT_ALARM_OFF`**: Orden para accionar el buzzer y enviar alerta por BT.
* **`raise EV_ACT_BEEP_SUCCESS`**: Orden para que el buzzer emita un pitido corto de confirmación acústica.
* **`raise EV_ACT_EEPROM_LOAD` / `raise EV_ACT_EEPROM_SAVE`**: Orden de lectura o escritura en I2C.
* **`raise EV_ACT_MENU_SHOW` / `raise EV_ACT_MENU_UPDATE` / `raise EV_ACT_ERROR_SHOW`**: Orden para actualizar las pantallas en el display.
* **`raise EV_ACT_MENU_UP`**: Orden al display para desplazar el cursor hacia arriba o incrementar el valor del parámetro en pantalla.
* **`raise EV_ACT_MENU_DOWN`**: Orden al display para desplazar el cursor hacia abajo o decrementar el valor del parámetro en pantalla.

***************************************************************************************************************************************************************************************************************************
## 6. System Statechart - State Transition Table

### 6.1 Transiciones Globales y de Seguridad
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_SYS_NORMAL_IDLE** | EV_SYS_LVL_CRIT | | **ST_SYS_ERROR** | raise EV_ACT_PUMP_OFF <br> raise EV_ACT_FAN_OFF <br> raise EV_ACT_ALARM_ON <br> raise EV_ACT_ERROR_SHOW |
| **ST_SYS_ERROR** | EV_SYS_LVL_OK | | **ST_SYS_NORMAL_IDLE**| raise EV_ACT_ALARM_OFF <br> raise EV_ACT_MENU_SHOW |
| **ST_SYS_NORMAL_IDLE** | EV_SYS_MODE_DOWN | | **ST_SYS_SET_UP** | raise EV_ACT_EEPROM_LOAD <br> raise EV_ACT_MENU_SHOW |
| **ST_SYS_SET_UP** | EV_SYS_MODE_DOWN | | **ST_SYS_NORMAL_IDLE**| raise EV_ACT_MENU_SHOW |

### 6.2 Ciclo de Riego Automático y Lazo Cerrado (Dentro de NORMAL)
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_SYS_NORMAL_IDLE** | EV_SYS_TICK | [t_espera == 0] | **ST_SYS_NORMAL_WATERING** | raise EV_ACT_PUMP_ON <br> t_riego = MAX_RIEGO <br> t_timeout_rly = MAX_TIMEOUT |
| | EV_SYS_TICK | [t_espera > 0] | **ST_SYS_NORMAL_IDLE** | t_espera-- |
| **ST_SYS_NORMAL_WATERING**| EV_SYS_RLY_CLOSED | | **ST_SYS_NORMAL_WATERING** | t_timeout_rly = -1 |
| | EV_SYS_TICK | [t_riego == 0] | **ST_SYS_NORMAL_IDLE** | raise EV_ACT_PUMP_OFF <br> t_espera = MAX_ESPERA |
| | EV_SYS_TICK | [t_timeout_rly == 0] | **ST_SYS_ERROR** | raise EV_ACT_ALARM_ON <br> raise EV_ACT_ERROR_SHOW |
| | EV_SYS_TICK | [t_riego > 0] | **ST_SYS_NORMAL_WATERING** | t_riego-- <br> *(Si t_timeout_rly > 0)*: t_timeout_rly-- |

### 6.3 Monitoreo Climático y Configuración Interactiva (Estados Paralelos / Concurrentes)
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_SYS_NORMAL_IDLE** | EV_SYS_TEMP_HIGH | | **ST_SYS_NORMAL_IDLE** | raise EV_ACT_FAN_ON |
| | EV_SYS_TEMP_OK | | **ST_SYS_NORMAL_IDLE** | raise EV_ACT_FAN_OFF |
| **ST_SYS_SET_UP** | EV_SYS_NEXT_DOWN | | **ST_SYS_SET_UP** | raise EV_ACT_MENU_UP |
| | EV_SYS_PREV_DOWN | | **ST_SYS_SET_UP** | raise EV_ACT_MENU_DOWN |
| | EV_SYS_CONF_DOWN | | **ST_SYS_SET_UP** | raise EV_ACT_EEPROM_SAVE <br> raise EV_ACT_MENU_UPDATE <br> raise EV_ACT_BEEP_SUCCESS |
