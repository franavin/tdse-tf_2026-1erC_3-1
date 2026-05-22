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

La siguiente tabla (Tabla 1.2.1) muestra los valores ponderados asignados a cada proyecto considerado:

#### **1.3 Selección de proyecto**

En este caso representaremos el equilibrio perfecto para el proyecto, ya que al elegir utilizar componentes simbólicos para los fluidos (como potenciómetros para simular el nivel de los tanques y relés que representan bombas), se elimina el riesgo físico permitiendo una depuración de código (debugging) cómoda y segura. Al mismo tiempo, conservamos intacta la estructura del software, donde el sistema exigirirá el diseño de un ejecutor cíclico y la comunicación I2C/SPI real con las memorias EEPROM, displays, etc. 

Los principales desafíos técnicos que se abordarán en este proyecto incluyen:
* **Acondicionamiento de señales analógicas:** Diseño de un filtro activo Sallen-Key por hardware para limpiar la señal del potenciómetro antes de su conversión mediante el ADC y DMA del microcontrolador.
* **Modelado robusto:** La creación de la lógica de riego mediante statecharts generados en herramientas como Itemis CREATE, prestando especial atención a la declaración de variables de temporización de largo plazo (usando el tipo de dato `uint32_t` estándar de C) para evitar desbordamientos en ciclos de varias horas.
* **Sistema de interfaz sin bloqueos:** Desarrollo de un menú interactivo fluido con rutinas de antirrebote eficientes para leer las teclas y gestionar los estados (`NORMAL`, `SET_UP`, `FALLA`) sin detener el muestreo de sensores.

###### **1.3.1 Diagrama en bloques**


---
### **2\. Elicitación de requisitos y casos de uso**


| Grupo | ID | Descripción |
| :---- | :---- | :---- |
| Sensores | 1.1 | **Nivel de agua (Simulado):** Lectura analógica por ADC (polling/DMA) de un potenciómetro, previo paso por filtro Sallen-Key. |
|  | 1.2 | **Clima (Real):** Lectura de temperatura y humedad ambiente utilizando el sensor I2C SHT3x.|
|  | 1.3 | **Interfaz de usuario:** Lectura no bloqueante (con anti-rebote) de botones y lectura de Dip Switches para alternar modos. | 
| Actuadores  | 2.1 | **Indicadores de estado (Bombas/Ventilación):** Encendido/apagado de LEDs o Relés temporizados por el Systick sin bloqueos. |
|  | 2.2 | **Alarmas:** Emisión de alertas sonoras mediante un Buzzer accionado por hardware (PWM o GPIO) ante fallas críticas. |
|  | 2.3 | **Telemetría y Display:** Envío de tramas de datos por UART (módulo HM-10) y actualización periódica del menú en display SPI/I2C. |
| Almacenamiento | 3.1 | **Set-Up de Recetas:** Lectura/Escritura de los tiempos de riego y umbrales térmicos en una memoria EEPROM externa. |

<p align="center"><em>Tabla 2: Requisitos del proyecto</em></p>


En las tablas 3.1 a 3.3 se presentan 3 casos de uso para el sistema.

| Elemento | Definición |
| :---- | :---- |
| **Disparador** | El usuario cambia el estado de un Dip Switch para ingresar al modo de configuración. |
| **Precondiciones** | El sistema debe estar energizado y el hardware periférico (Display y Botones) inicializado correctamente. |
| **Flujo principal** | 1. El sistema detecta el cambio en el Dip Switch y transita al estado `SET_UP`. <br> 2. Se detienen los temporizadores de riego (actuadores en reposo). <br> 3. El display muestra el menú interactivo con los parámetros actuales recuperados de la EEPROM. <br> 4. El usuario incrementa/decrementa el "Tiempo de Riego" y el "Umbral de Temperatura" usando los botones. <br> 5. El usuario presiona el botón "Guardar". <br> 6. El sistema escribe los nuevos valores en la EEPROM. <br> 7. El usuario devuelve el Dip Switch a su posición original, retornando al estado `NORMAL`. |
| **Flujos alternativos** | 1.a. Si durante el guardado no se detecta la EEPROM (falla en el bus I2C), el sistema emite 3 pitidos cortos con el Buzzer y mantiene los valores por defecto en memoria RAM. |

<p align="center"><em>Tabla 3.1: Caso de uso 1: Configuración de Parámetros de Cultivo (SET_UP)</em></p>

| Elemento | Definición |
| :---- | :---- |
| **Disparador** | El contador interno (`uint32_t` basado en el tick de 1ms) alcanza el valor configurado para iniciar el ciclo de riego. |
| **Precondiciones** | El sistema debe encontrarse en estado `NORMAL` y el sensor de nivel de agua analógico debe registrar un valor por encima del nivel mínimo. |
| **Flujo principal** | 1. La máquina de estados evalúa la condición de transición (tiempo transcurrido >= intervalo de riego). <br> 2. Se transita al sub-estado `RIEGO_ACTIVO`. <br> 3. Se activa la salida digital correspondiente al LED/Relé de la bomba de agua. <br> 4. El sistema transmite por el módulo HM-10 (Bluetooth) la trama: "Riego Iniciado". <br> 5. Al cumplirse la duración del riego, se desactiva la salida digital y el sistema retorna al sub-estado `ESPERA`. |
| **Flujos alternativos** | 2.a. Si durante el riego el sensor SHT3x detecta una temperatura superior al umbral crítico, se enciende paralelamente el LED/Relé de ventilación sin interrumpir ni bloquear la tarea de riego principal. |

<p align="center"><em>Tabla 3.2: Caso de uso 2: Ejecución del Ciclo Automático de Riego</em></p>

| Elemento | Definición |
| :---- | :---- |
| **Disparador** | El ADC detecta que la tensión proveniente del sensor de nivel simulado (potenciómetro) cae por debajo del umbral de seguridad establecido. |
| **Precondiciones** | El sistema se encuentra operando en modo `NORMAL` y se ha completado exitosamente la conversión analógica-digital de la variable. |
| **Flujo principal** | 1. La rutina de procesamiento evalúa la lectura del ADC. <br> 2. Se detecta el nivel crítico y se fuerza una transición incondicional al estado de `FALLA`. <br> 3. Se desactivan de forma inmediata todas las cargas operativas (apagado de bombas y ventiladores). <br> 4. El Buzzer comienza a emitir una señal acústica intermitente. <br> 5. El display parpadea con el mensaje "ERROR: SIN AGUA". <br> 6. Se envía por Bluetooth la alerta de emergencia. |
| **Flujos alternativos** | 1.a. El sistema permanece enclavado en este estado (modo seguro) y no intentará regar nuevamente hasta que se restaure físicamente el nivel (se ajuste el potenciómetro) y se presione el botón de "Acknowledge/Reset" en la placa base. |

<p align="center"><em>Tabla 3.3: Caso de uso 3: Bloqueo del Sistema por Nivel Crítico de Agua (FALLA)</em></p>
