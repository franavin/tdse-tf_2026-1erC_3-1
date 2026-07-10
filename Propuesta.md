 <img src="https://www.fi.uba.ar/images/logo-fiuba.png" alt="image2" width="50%">


# **Estación Hidropónica**


**Autores: Avincetto, Franco Joaquín; Nishihara, Leonardo**

**Padrón: 106747; 88627**

**Fecha: 1er cuatrimestre 2026**

### **1\. Selección del proyecto a implementar**

#### **1.1 Objetivo del proyecto y resultados esperados**
El objetivo principal es diseñar e implementar un Producto Mínimo Viable (prototipo) bare-metal basado en un microcontrolador STM32 que automatice y monitoree una estación de cultivo hidropónico. Con la representación física sobre una placa base soldada, el sistema supervisará variables ambientales y de nivel de líquidos, y controlará de forma temporizada los ciclos de riego y ventilación. 

Esperamos lograr un sistema del tipo "Event-Triggered" en el cual utilizaremos un patrón Super-Loop (con tick de Systick de ~1ms) para garantizar tareas no bloqueantes. El modelo lógico se basará en diagramas de estado para alternar entre distintos modos de operación (NORMAL, SET_UP, FALLA), implementando comunicación serie (I2C/SPI), adquisición de datos (ADC con DMA) y telemetría remota (Bluetooth).

#### **1.2 Proyectos similares**
En base al objetivo principal, se proponen posibles alternativas de implementación que requieren el uso de hardware específico y se ponderan los aspectos a tener en cuenta para seleccionar el enfoque más adecuado para el Trabajo Final. 

1. **Base:** Estación hidropónica con monitoreo de nivel de agua (simulado con potenciómetro y filtro activo Sallen-Key) y clima (sensor SHT3x). Incluye actuación básica (relés/LEDs para bombas y ventilador) y telemetría por Bluetooth (HM-10).
2. **Base + Memoria y Display:** Al proyecto base se le suma una pantalla OLED (SPI/I2C) para un menú interactivo local y una memoria EEPROM (I2C) para almacenar distintas "recetas" de cultivo (parámetros de riego y temperatura).
3. **Base + Control de Nutrientes (pH/EC):** Al proyecto base se le suman más potenciómetros y filtros activos para simular la lectura analógica de pH y conductividad eléctrica (EC), junto con actuadores adicionales para simular bombas dosificadoras de nutrientes.

Para comparar estas alternativas, se tienen en cuenta cinco aspectos característicos, los cuales se ponderan del 1 al 10:

1. **Disponibilidad del hardware (9):** Se evalúa si el proyecto puede implementarse utilizando componentes e integrados que se consigan fácilmente en el mercado regional.
2. **Impacto en el proyecto (8):** Se pondera cuánto aporta a la funcionalidad, robustez del sistema y cumplimiento de los requisitos de la materia.
3. **Costo (8):** Se evalúa el costo total del hardware extra requerido para cada opción (displays, memorias, amplificadores operacionales extra).
4. **Dificultad técnica / Viabilidad (10):** Se contempla el tiempo requerido, la complejidad de implementación del código (máquinas de estado, buses de comunicación) y el diseño de la placa base física.
5. **Interés personal del equipo (7):** Se pondera la motivación e interés del equipo en desarrollar y programar cada alternativa.

La siguiente tabla (Tabla 1.2.1) muestra los valores ponderados asignados a cada proyecto considerado:

<table>
    <thead>
        <tr>
            <th rowspan="2">Criterio</th>
            <th colspan="2">Proyecto Base</th>
            <th colspan="2">Base + Memoria y Display</th>
            <th colspan="2">Base + Control Nutrientes</th>
        </tr>
        <tr>
            <th>Puntaje</th>
            <th>Puntaje Ponderado</th>
            <th>Puntaje</th>
            <th>Puntaje Ponderado</th>
            <th>Puntaje</th>
            <th>Puntaje Ponderado</th>
        </tr>
    </thead>
    <tbody>
        <tr class="header-row">
            <td align="center">Disponibilidad de Hardware <br>(peso: 9)</td>
            <td align="center">10</td>
            <td align="center">90</td>
            <td align="center">9</td>
            <td align="center">81</td>
            <td align="center">8</td>
            <td align="center">72</td>
        </tr>
        <tr>
            <td align="center">Impacto en el proyecto <br>(peso: 8)</td>
            <td align="center">6</td>
            <td align="center">48</td>
            <td align="center">9</td>
            <td align="center">72</td>
            <td align="center">8</td>
            <td align="center">64</td>
        </tr>
        <tr class="header-row">
            <td align="center">Costo <br>(peso: 8)</td>
            <td align="center">9</td>
            <td align="center">72</td>
            <td align="center">7</td>
            <td align="center">56</td>
            <td align="center">6</td>
            <td align="center">48</td>
        </tr>
        <tr>
            <td align="center">Dificultad técnica / Viabilidad <br>(peso: 10)</td>
            <td align="center">9</td>
            <td align="center">90</td>
            <td align="center">8</td>
            <td align="center">80</td>
            <td align="center">6</td>
            <td align="center">60</td>
        </tr>
        <tr class="header-row">
            <td align="center">Interés personal <br>(peso: 7)</td>
            <td align="center">6</td>
            <td align="center">42</td>
            <td align="center">10</td>
            <td align="center">70</td>
            <td align="center">7</td>
            <td align="center">49</td>
        </tr>
        <tr class="highlight-green">
            <td><strong>Puntaje Total</strong></td>
            <td align="center">-</td>
            <td align="center">342</td>
            <td align="center">-</td>
            <td align="center"><strong>359</strong></td>
            <td align="center">-</td>
            <td class="highlight-red">293</td>
        </tr>
    </tbody>
</table>
<p align="center"><em>Tabla 1.2.1: Comparación de alternativas de proyecto</em></p>

#### **1.3 Selección de proyecto**

Vistas las consideraciones tomadas en cuenta en el punto anterior, se decidió avanzar con la opción **Base + Memoria y Display**. 

El Proyecto Base cumple con los requisitos mínimos, pero carece de una interfaz interactiva local compleja, lo cual desaprovecha la capacidad de procesamiento de la STM32. Por otro lado, tenemos la opción de agregar el Control de Nutrientes (pH/EC), pero incrementa excesivamente la complejidad del hardware analógico (requiriendo múltiples filtros activos y restando pines al microcontrolador) sin aportar un valor significativo a la arquitectura de software.

Por lo que la opción seleccionada (Base + Memoria y Display) representa el punto óptimo entre todas. La inclusión de un menú interactivo a través de un Display y la gestión de datos persistentes mediante una EEPROM obligan a desarrollar un código modular. 
Los principales desafíos que se van a afrontar son: la sincronización de las comunicaciones I2C/SPI para no entorpecer el muestreo del ADC (con DMA) de la señal filtrada por el circuito Sallen-Key, y la creación de una rutina de antirrebote eficiente para que la navegación por el display sea fluida mientras el sistema atiende simultáneamente las conexiones Bluetooth de la APP.

Los principales desafíos técnicos que se abordarán en este proyecto incluyen:
* **Acondicionamiento de señales analógicas:** Diseño de un filtro activo Sallen-Key por hardware para limpiar la señal del potenciómetro antes de su conversión mediante el ADC y DMA del microcontrolador.
* **Modelado robusto:** La creación de la lógica de riego mediante statecharts generados en herramientas como Itemis CREATE, prestando especial atención a la declaración de variables de temporización de largo plazo (usando el tipo de dato `uint32_t` estándar de C) para evitar desbordamientos en ciclos de varias horas.
* **Sistema de interfaz sin bloqueos:** Desarrollo de un menú interactivo fluido con rutinas de antirrebote eficientes para leer las teclas y gestionar los estados (`NORMAL`, `SET_UP`, `FALLA`) sin detener el muestreo de sensores.

###### **1.3.1 Diagrama en bloques**
En la Figura 1 se muestra el diagrama en bloques del sistema con los principales módulos del proyecto

<p align="center">
  <img src="https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/diagrama_en_bloques.png" alt="image1">
</p>

<p align="center"><em>Figura 1: Diagrama en bloques del sistema</em></p>

---
### **2. Elicitación de requisitos y casos de uso**

En la siguiente tabla se detallan los requisitos funcionales y no funcionales del sistema, enfocados en la arquitectura Bare-Metal del microcontrolador STM32 y el hardware de simulación propuesto.

| Grupo | ID | Descripción |
| :---- | :---- | :---- |
| **Sensores** | 1.1 | **Nivel de agua (Simulado):** Lectura analógica por ADC (polling/DMA) de un potenciómetro, previo paso por un filtro activo Sallen-Key por hardware. |
| | 1.2 | **Clima (Real):** Lectura de temperatura y humedad ambiente utilizando el sensor digital SHT3x a través del bus I2C. |
| | 1.3 | **Interfaz de entrada local:** Lectura no bloqueante (con rutinas de anti-rebote por software) de botones para la navegación del menú y Dip Switches para alternar los modos del sistema (`NORMAL` / `SET_UP`). |
| **Actuadores** | 2.1 | **Actuadores de simulación (Bombas/Ventilación):** Encendido y apagado de LEDs/Relés gestionados por retardos no bloqueantes basados en el tick del Systick (1ms). |
| | 2.2 | **Alarmas sonoras:** Emisión de alertas mediante un Buzzer (accionado por PWM o GPIO) ante fallas críticas del sistema o confirmaciones de guardado. |
| **Interfaz y Comunicaciones** | 3.1 | **Interfaz Visual Local (Display):** Actualización periódica de un menú interactivo en una pantalla OLED (SPI/I2C) para la visualización y configuración de parámetros sin depender de la red. |
| | 3.2 | **Telemetría remota:** Envío de tramas de estado y alertas por UART utilizando un módulo Bluetooth HM-10 para monitoreo desde una aplicación móvil. |
| **Almacenamiento** | 4.1 | **Gestión de Recetas (EEPROM):** Lectura y escritura de parámetros de configuración (tiempos de riego, umbrales térmicos) en una memoria EEPROM externa vía I2C para garantizar la persistencia ante cortes de energía. |

<p align="center"><em>Tabla 2: Requisitos del proyecto</em></p>

---
#### **3\. Casos de Uso**

En las tablas 3.1 a 3.3 se presentan 3 casos de uso para el sistema.

| Elemento | Definición |
| :---- | :---- |
| **Disparador** | El usuario cambia el estado de un Dip Switch para ingresar al modo de configuración de recetas. |
| **Precondiciones** | El sistema debe estar en modo `NORMAL`. El Display y los botones de navegación deben estar operativos. La comunicación I2C con la EEPROM debe estar inicializada. |
| **Flujo principal** | 1. El sistema detecta el cambio en el Dip Switch y transita al estado `SET_UP`. <br> 2. Se detienen los temporizadores de riego activos y los actuadores pasan a estado seguro. <br> 3. El microcontrolador lee de la EEPROM los parámetros de la receta activa y los muestra en el Display. <br> 4. El usuario navega por el menú interactivo con los botones para modificar el "Tiempo de Riego" y el "Umbral de Temperatura". <br> 5. El usuario presiona el botón de confirmación; el microcontrolador escribe los nuevos valores en la EEPROM externa a través del bus I2C. <br> 6. El usuario regresa el Dip Switch a su posición original y el sistema vuelve al modo `NORMAL` aplicando la nueva receta. |
| **Flujos alternativos** | 5.a. Falla de comunicación I2C con la EEPROM: El sistema muestra un mensaje de "ERROR: EEPROM" en el Display, emite un triple pitido con el Buzzer y mantiene los cambios de forma temporal únicamente en la memoria RAM del microcontrolador. |

<p align="center"><em>Tabla 3.1: Caso de uso 1: Configuración de Recetas de Cultivo (SET_UP)</em></p>

| Elemento | Definición |
| :---- | :---- |
| **Disparador** | El temporizador de software no bloqueante (basado en el Systick de 1ms) alcanza el tiempo de intervalo de riego definido en la receta activa. |
| **Precondiciones** | El sistema se encuentra en modo `NORMAL`. El microcontrolador cargó exitosamente los umbrales de la EEPROM al iniciar. El sensor analógico de nivel de agua registra valores seguros. |
| **Flujo principal** | 1. La máquina de estados del firmware evalúa que el tiempo transcurrido es igual o mayor al intervalo de la receta de la EEPROM. <br> 2. El sistema transita al sub-estado `RIEGO_ACTIVO`. <br> 3. Se acciona el LED/Relé que simula la bomba de agua mediante una salida digital no bloqueante. <br> 4. El Display OLED se actualiza reflejando el estado "Riego en proceso...". <br> 5. Se envía una trama de telemetría por el módulo HM-10 (Bluetooth) informando el evento. <br> 6. Al completarse el tiempo de riego estipulado, la máquina de estados apaga el actuador y regresa al sub-estado de espera. |
| **Flujos alternativos** | 1.a. Si la temperatura medida por el sensor SHT3x supera el umbral de la receta mientras se espera el riego, el microcontrolador activa de manera independiente la salida PWM del ventilador (LED/Relé) sin bloquear la ejecución del Super-Loop. |

<p align="center"><em>Tabla 3.2: Caso de uso 2: Ejecución del Ciclo Automático de Riego (NORMAL)</em></p>

| Elemento | Definición |
| :---- | :---- |
| **Disparador** | El canal de ADC (con adquisición por DMA) registra que la tensión del potenciómetro de nivel de agua cae por debajo del límite mínimo de seguridad. |
| **Precondiciones** | El sistema se encuentra operando en modo `NORMAL` y realizando conversiones periódicas de los canales analógicos de forma no bloqueante. |
| **Flujo principal** | 1. El firmware procesa el bloque de datos del ADC y detecta la condición de tanque vacío. <br> 2. La máquina de estados ejecuta una transición incondicional e inmediata al modo de `FALLA`. <br> 3. Se desactivan de forma instantánea todas las salidas digitales hacia los actuadores (bombas y ventilación apagadas). <br> 4. El Display OLED interrumpe su pantalla actual y muestra de forma intermitente el mensaje "ALERTA: SIN AGUA". <br> 5. Se acciona el Buzzer por hardware para emitir una alarma sonora intermitente y se envía la notificación de emergencia vía Bluetooth (HM-10). |
| **Flujos alternativos** | 5.a. El sistema permanece bloqueado en modo seguro hasta que el usuario reestablezca el nivel (simulado con el potenciómetro) y presione el botón físico de "Reset/Acknowledge" en la placa base para retornar al modo `NORMAL`. |

<p align="center"><em>Tabla 3.3: Caso de uso 3: Bloqueo del Sistema por Nivel Crítico (FALLA)</em></p>
