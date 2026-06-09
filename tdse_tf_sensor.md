## Estación Hidropónica - Módulo de Sensores
**-Módulo de código C:** Temporizado  
**-Período de escrutinio:** 1 ms
***************************************************************************************************************************************************************************************************************************
## 1. Descripción del Modelo
En el siguiente modulo lo que tratamos de escrutar o sensar es el estado físico en el que se encuentran los diversos periféricos de entrada 
de la estación hidropónica. 
Dado que las lecturas físicas no son perfectas y pueden generar rebotes o ruidos eléctricos, en el modulo debemos de implementar máquinas de estados 
que tengan en cuenta estas anomalías y las puedan filtrar.

El módulo tendrá un tiempo de accionamiento de 1 ms, en este tiempo lo que hará es evaluar el estado "físico" de las entradas y gestionar los ticks 
del sistema para poder asegurar la información.
Una vez que el sistema detecte que el estado físico es estable durante un tiempo determinado usando las constantes `DEL_XX_MAX`, el módulo sensor 
emite una "Signal" (un mensaje o evento de sistema) hacia el módulo de procesamiento.

***************************************************************************************************************************************************************************************************************************
## 2. Estados

### 2.1 Teclado Matricial (CONF y MODE) y Relé (RLY)
* **`ST_XX_UP` / `ST_RLY_FB_OPEN`**: Es el estado de reposo cuando no hay accionamiento (tecla sin presionar o relé abierto).
* **`ST_XX_FALLING` / `ST_RLY_FB_CLOSING`**: Es el estado transitorio donde se valida si se inició la acción.
* **`ST_XX_DOWN` / `ST_RLY_FB_CLOSED`**: Es el estado estable cuando la acción está confirmada.
* **`ST_XX_RISING` / `ST_RLY_FB_OPENING`**: Es el estado transitorio que valida si la acción finalizó.

### 2.2 Sensor de Nivel Analógico (LVL) y Temperatura (AHT20)
* **`ST_LVL_OK` / `ST_AHT_OK`**: Es el estado seguro (nivel de agua correcto o temperatura normal).
* **`ST_LVL_FALLING` / `ST_AHT_RISING`**: Es el estado transitorio donde se valida el cruce del umbral de falla.
* **`ST_LVL_CRITICAL` / `ST_AHT_HIGH`**: Es el estado estable de alerta confirmada.
* **`ST_LVL_RISING` / `ST_AHT_FALLING`**: Es el estado transitorio que valida la normalización del sistema.

***************************************************************************************************************************************************************************************************************************
## 3. Eventos
Los eventos representan el comportamiento físico de las entradas para con el microcontrolador:

* **Teclado:** `EV_BTN_XX_UP` (tecla liberada) / `EV_BTN_XX_DOWN` (tecla presionada). *Donde XX puede ser CONF (Confirmar), MODE (Modo), UP (Arriba) o DOWN (Abajo).*
* **Nivel de Agua:** `EV_ADC_LVL_HIGH` (tensión por encima del umbral) / `EV_ADC_LVL_LOW` (tensión por debajo del umbral).
* **Temperatura AHT20:** `EV_I2C_TEMP_LOW` (clima normal) / `EV_I2C_TEMP_HIGH` (clima caluroso).
* **Relés:** `EV_GPIO_RLY_LOW` (ausencia de tensión de feedback) / `EV_GPIO_RLY_HIGH` (tensión detectada).

***************************************************************************************************************************************************************************************************************************
## 4. Variables de Control
Para comprobar los eventos físicos a lo largo del tiempo, el módulo utiliza una variable de control la cual condiciona las transiciones de estado:
* **`tick`**: Variable temporizadora de cuenta regresiva (`uint32_t`).
* **`DEL_BTN_MAX` = 40**: Constante de validación anti-rebote para las teclas (40 ms).
* **`DEL_LVL_MAX` = 100**: Constante de validación para caída de nivel de agua (100 ms).
* **`DEL_AHT_MAX` = 2000**: Constante de validación térmica para evitar encendidos erráticos (2000 ms).
* **`DEL_RLY_MAX` = 20**: Constante de validación para el rebote del contacto mecánico del relé (20 ms).

***************************************************************************************************************************************************************************************************************************
## 5. Acciones
Las acciones son las respuestas de la máquina de estados ante un Evento válido que cumple con su condición de guarda. Se dividen en manejo interno y señales al sistema central (Signals).

### Acciones Internas
Son las acciones encargadas de manejar el temporizador.
* **`tick = DEL_XX_MAX`**: Inicializa la variable de control y ejecuta cuando se detecta el primer cambio de estado físico para comenzar a medir el tiempo de estabilización.
* **`tick--`**: Decrementa la variable de control. Se ejecuta en cada ciclo de 1 ms mientras el estado físico se mantenga durante el período de validación.

### Acciones Externas
Estas acciones son los mensajes en los que el módulo de escrutar envía hacia el modulo de procesar, reflejando de esta manera su cambio de posición válido y ya confirmado.
* **`raise EV_SYS_...` (Ej. `EV_SYS_MODE_DOWN`, `EV_SYS_UP_DOWN`, `EV_SYS_LVL_CRIT`)**: Emite la señal al sistema indicando que el evento de falla, presión o cierre ocurrió exitosamente (pasó el tiempo de anti-rebote).
* **`raise EV_SYS_...` (Ej. `EV_SYS_MODE_UP`, `EV_SYS_LVL_OK`)**: Emite la señal al sistema indicando que el evento se liberó o normalizó exitosamente (pasó el tiempo de anti-rebote sin fluctuar).

***************************************************************************************************************************************************************************************************************************
## 6. Sensor Statechart - State Transition Table

### 6.1 Teclado Matricial (`MODE` o `CONF`)

#### Tabla de la tecla en estado presionado
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_BTN_XX_UP** | EV_BTN_XX_UP | | ST_BTN_XX_UP | |
| | EV_BTN_XX_DOWN | | ST_BTN_XX_FALLING | tick = DEL_BTN_MAX |
| **ST_BTN_XX_FALLING** | EV_BTN_XX_UP | [tick > 0] | ST_BTN_XX_FALLING | tick-- |
| | | [tick == 0] | ST_BTN_XX_UP | |
| | EV_BTN_XX_DOWN | [tick > 0] | ST_BTN_XX_FALLING | tick-- |
| | | [tick == 0] | ST_BTN_XX_DOWN | raise EV_SYS_XX_DOWN |

#### Tabla de la tecla en estado liberado
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_BTN_XX_DOWN** | EV_BTN_XX_DOWN | | ST_BTN_XX_DOWN | |
| | EV_BTN_XX_UP | | ST_BTN_XX_RISING | tick = DEL_BTN_MAX |
| **ST_BTN_XX_RISING** | EV_BTN_XX_DOWN | [tick > 0] | ST_BTN_XX_RISING | tick-- |
| | | [tick == 0] | ST_BTN_XX_DOWN | |
| | EV_BTN_XX_UP | [tick > 0] | ST_BTN_XX_RISING | tick-- |
| | | [tick == 0] | ST_BTN_XX_UP | raise EV_SYS_XX_UP |

---

### 6.2 Sensor Analógico de Nivel de Agua (`LVL`)

#### Tabla del nivel en estado crítico
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_LVL_OK** | EV_ADC_LVL_HIGH | | ST_LVL_OK | |
| | EV_ADC_LVL_LOW | | ST_LVL_FALLING | tick = DEL_LVL_MAX |
| **ST_LVL_FALLING** | EV_ADC_LVL_HIGH | [tick > 0] | ST_LVL_FALLING | tick-- |
| | | [tick == 0] | ST_LVL_OK | |
| | EV_ADC_LVL_LOW | [tick > 0] | ST_LVL_FALLING | tick-- |
| | | [tick == 0] | ST_LVL_CRITICAL | raise EV_SYS_LVL_CRIT |

#### Tabla del nivel en estado normalizado
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_LVL_CRITICAL** | EV_ADC_LVL_LOW | | ST_LVL_CRITICAL | |
| | EV_ADC_LVL_HIGH | | ST_LVL_RISING | tick = DEL_LVL_MAX |
| **ST_LVL_RISING** | EV_ADC_LVL_LOW | [tick > 0] | ST_LVL_RISING | tick-- |
| | | [tick == 0] | ST_LVL_CRITICAL | |
| | EV_ADC_LVL_HIGH | [tick > 0] | ST_LVL_RISING | tick-- |
| | | [tick == 0] | ST_LVL_OK | raise EV_SYS_LVL_OK |

---

### 6.3 Sensor de Temperatura I2C (`AHT20`)

#### Tabla de temperatura en estado alto (alerta)
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_AHT_OK** | EV_I2C_TEMP_LOW | | ST_AHT_OK | |
| | EV_I2C_TEMP_HIGH | | ST_AHT_RISING | tick = DEL_AHT_MAX |
| **ST_AHT_RISING** | EV_I2C_TEMP_LOW | [tick > 0] | ST_AHT_RISING | tick-- |
| | | [tick == 0] | ST_AHT_OK | |
| | EV_I2C_TEMP_HIGH | [tick > 0] | ST_AHT_RISING | tick-- |
| | | [tick == 0] | ST_AHT_HIGH | raise EV_SYS_TEMP_HIGH |

#### Tabla de temperatura en estado normalizado
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_AHT_HIGH** | EV_I2C_TEMP_HIGH | | ST_AHT_HIGH | |
| | EV_I2C_TEMP_LOW | | ST_AHT_FALLING | tick = DEL_AHT_MAX |
| **ST_AHT_FALLING** | EV_I2C_TEMP_HIGH | [tick > 0] | ST_AHT_FALLING | tick-- |
| | | [tick == 0] | ST_AHT_HIGH | |
| | EV_I2C_TEMP_LOW | [tick > 0] | ST_AHT_FALLING | tick-- |
| | | [tick == 0] | ST_AHT_OK | raise EV_SYS_TEMP_OK |

---

### 6.4 Retroalimentación de Relés (`RLY_FB`)

#### Tabla del relé en estado cerrado
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_RLY_FB_OPEN**| EV_GPIO_RLY_LOW | | ST_RLY_FB_OPEN | |
| | EV_GPIO_RLY_HIGH | | ST_RLY_FB_CLOSING | tick = DEL_RLY_MAX |
| **ST_RLY_FB_CLOSING**| EV_GPIO_RLY_LOW | [tick > 0] | ST_RLY_FB_CLOSING | tick-- |
| | | [tick == 0] | ST_RLY_FB_OPEN | |
| | EV_GPIO_RLY_HIGH | [tick > 0] | ST_RLY_FB_CLOSING | tick-- |
| | | [tick == 0] | ST_RLY_FB_CLOSED | raise EV_SYS_RLY_CLOSED |

#### Tabla del relé en estado abierto
| Current State | Event | [Guard] | Next State | Actions |
| :--- | :--- | :--- | :--- | :--- |
| **ST_RLY_FB_CLOSED**| EV_GPIO_RLY_HIGH | | ST_RLY_FB_CLOSED | |
| | EV_GPIO_RLY_LOW | | ST_RLY_FB_OPENING | tick = DEL_RLY_MAX |
| **ST_RLY_FB_OPENING**| EV_GPIO_RLY_HIGH | [tick > 0] | ST_RLY_FB_OPENING | tick-- |
| | | [tick == 0] | ST_RLY_FB_CLOSED | |
| | EV_GPIO_RLY_LOW | [tick > 0] | ST_RLY_FB_OPENING | tick-- |
| | | [tick == 0] | ST_RLY_FB_OPEN | raise EV_SYS_RLY_OPEN |
