<div align="center">

<img width="535"  alt="image" src="https://github.com/Embebidos-Fran-Marcos-Nacho/tdse-tf_1-2/blob/08290a7a62c8a7d3fcd22fc57871dafbbf35ab15/logo-fiuba.png" />

**UNIVERSIDAD DE BUENOS AIRES**  
**Facultad de Ingeniería**  
**TA134 – Sistemas Embebidos**  
Curso 3 – Grupo 1

# Estación Hidropónica 


## Autores
Avincetto, Franco Joaquín — Legajo 106747  
Nishihara, Leonardo — Legajo 88627 

**Fecha:** 10/07/2026  
**Cuatrimestre de cursada:** 1er cuatrimestre 2026  

*Trabajo realizado entre *
</div>

---


## Resumen
Se propone el diseño e implementación de un Producto Mínimo Viable (prototipo) bare-metal basado en un microcontrolador STM32 para la automatización y monitoreo de una estación de cultivo hidropónico. El sistema se encarga de supervisar variables ambientales y de nivel de líquidos, controlando de forma temporizada los ciclos de riego y ventilación. La arquitectura de firmware se basa en un patrón Super-Loop de tipo "Event-Triggered" con máquinas de estado no bloqueantes, incorporando adquisición de datos analógicos y telemetría por Bluetooth.

---

## Registro de versiones

*Historial de revisiones del documento.*

La Tabla 0.1 resume el historial de revisiones y entregas de esta memoria.

| Versión | Cambios realizados | Fecha |
| :---: | --- | :---: |
| 1.0 | Entrega de la memoria con la estructura básica de la memoria | 10/07/2026 |




<em>Tabla 0.1 — Registro de versiones del documento.</em><br><br>

---



# Índice General

- [Capítulo 1: Introducción general](#capítulo-1-introducción-general)
  - [1.1 Análisis de necesidad y objetivo](#11-análisis-de-necesidad-y-objetivo)
  - [1.2 Productos comparables](#12-productos-comparables)
  - [1.3 Justificación del enfoque técnico](#13-justificación-del-enfoque-técnico)
  - [1.4 Alcance y limitaciones](#14-alcance-y-limitaciones)
- [Capítulo 2: Introducción específica](#capítulo-2-introducción-específica)
  - [2.1 Requisitos](#21-requisitos)
  - [2.2 Casos de uso](#22-casos-de-uso)
  - [2.3 Descripción de módulos principales](#23-descripción-de-módulos-principales)
- [Capítulo 3: Diseño e implementación](#capítulo-3-diseño-e-implementación)
  - [3.1 Arquitectura general](#31-arquitectura-general)
  - [3.2 Diseño de hardware](#32-diseño-de-hardware)
  - [3.3 Diseño de firmware](#33-diseño-de-firmware)
- [Capítulo 4: Ensayos y resultados](#capítulo-4-ensayos-y-resultados)
  - [4.1 Pruebas funcionales de hardware](#41-pruebas-funcionales-de-hardware)
  - [4.2 Pruebas funcionales de firmware](#42-pruebas-funcionales-de-firmware)
  - [4.3 Pruebas de integración](#43-pruebas-de-integración)
  - [4.4 Medición y análisis de consumo](#44-medición-y-análisis-de-consumo)
- [Capítulo 5: Conclusiones](#capítulo-5-conclusiones)
  - [5.1 Resultados obtenidos](#51-resultados-obtenidos)
  - [5.2 Lecciones aprendidas](#52-lecciones-aprendidas)
  - [5.3 Próximos pasos](#53-próximos-pasos)
- [Capítulo 6: Uso de herramientas de IA](#capítulo-6-uso-de-herramientas-de-ia)
  - [6.1 Uso individual y conjunto](#61-uso-individual-y-conjunto)
- [Capítulo 7: Bibliografía y referencias](#capítulo-7-bibliografía-y-referencias)

---

# Capítulo 1: Introducción general

## 1.1 Análisis de necesidad y objetivo
El cultivo hidropónico es una técnica en la cual se prescinde por completo de un suelo en el que poner aquello que se quiere cultivar, utilizando en su lugar minerales disueltos en agua para formar el sustrato. Para que este método sea exitoso se requiere de ciertos cuidados estrictos en el entorno en que se desarrolla, tanto por los ciclos de oxigenación y el riego de las raíces como por la temperatura y humedad del ambiente. 

Un fallo técnico en un cultivo de este tipo puede ser definitivo para la vida del cultivo. Ya sea porque el tanque que contiene los nutrientes se vacía o hasta si hubo algún error en la medición y envía datos falsos sobre la situación del cultivo (falsos positivos). 

Por lo tanto, lo que buscamos como objetivo principal en esta ocasión es diseñar e implementar un prototipo funcional (Producto Mínimo Viable) que automatice y monitoree una estación de cultivo hidropónico. Mediante una representación física sobre una placa base soldada, el sistema supervisará variables críticas como el nivel de líquidos y el clima, y controlará de forma temporizada los ciclos de actuación (riego y ventilación).

## 1.2 Productos comparables
Para la selección de la implementación, se evaluaron tres enfoques principales, ponderados según disponibilidad de hardware, impacto, costo, dificultad técnica e interés del equipo:

* **Proyecto Base:** Monitoreo de nivel de agua (simulado con potenciómetro y filtro activo Sallen-Key) y clima (sensor SHT3x/AHT20). Incluye relés, LEDs para bombas/ventilador y telemetría por Bluetooth (HM-10).

* **Base + Memoria y Display:** Suma una pantalla OLED para un menú interactivo y memoria EEPROM para almacenar "recetas" de cultivo.

* **Base + Control de Nutrientes (pH/EC):** Agrega potenciómetros y filtros para simular lectura de pH/EC, con actuadores extra para dosificadores.

En el mercado actual podremos observar que hay distintos tipos de controladores de cultivos, empezando desde lo más básico como un controlador de riego el cual lo que hace es controlar el cierre de una salida de agua según el intervalo de tiempo que se ajuste, y por otro lado tenemos aquellos los cuáles se le suman características tales como controlador mediante Wi-Fi, detector de lluvia, etc. De estos podemos decir dos cosas comparandolos con nuestro caso.

1. **Temporizadores básicos:** Son soluciones económicas y ampliamente utilizadas. Sin embargo, operan a lazo abierto y carecen de entradas para medir el nivel de agua o el estado del entorno. Al no tener retroalimentación, encenderán la bomba independientemente de si hay agua en el tanque, anulando cualquier tipo de seguridad para el hardware.
2. **Enchufes y relés inteligentes:** Proveen conectividad Wi-Fi y control de horarios mediante aplicaciones móviles, donde su principal desventaja es la dependencia crítica de la red Wi-Fi doméstica y servidores. En este caso, un microcorte de internet o una falla en el router local puede dejar al dispositivo inoperante o desconectado de sus rutinas. Además, los de este tipo no suelen contar con interfaces físicas locales (pantallas informativas) para un diagnóstico rápido en el lugar de la instalación.

## 1.3 Justificación del enfoque técnico

El sistema se implementará utilizando una arquitectura de software estrictamente no bloqueante. Se utilizará un microcontrolador STM32 programado en bare-metal bajo el patrón de Super-Loop. Las secuencias que requieren control temporal se resolverán mediante máquinas de estado internas que gestionan los "ticks" del sistema (1 ms), evitando por completo el uso de funciones de retraso pasivo (delay).

Como se mencionó anteriormente, la integración de periféricos con altas latencias se resolvió mediante máquinas de estados no bloqueantes:
* **Comunicaciones I2C**: Se evitaron rutinas bloqueantes para esperar los tiempos de conversión térmica (~80 ms) o los tiempos de escritura de celdas (~5 ms). Se desarrollaron drivers específicos que ceden el control al procesador inmediatamente, retomando la lectura solo cuando el hardware indica disponibilidad.
* **Módulo Bluetooth**: Se atiende exclusivamente mediante interrupciones asincrónicas de hardware y buffers, asegurando que la recepción de un comando no viole las restricciones temporales que se imponen.

## 1.4 Alcance y limitaciones
El alcance del sistema contempla el monitoreo de nivel y temperatura, la actuación de bombas de agua y ventiladores mediante relés de potencia, alarmas acústicas/visuales y la transmisión de información remota. Todo evento externo e interno se somete a validación mediante filtrado de estado temporal para evadir falsos positivos y rebotes.

Las límitaciones actuales comienzan por un lado con la potencia y el entorno, ya que para comprobar que la etapa de potencia funcionaría utilizamos la observación a partir de la conmutación de los relés a nivel lógico, y no unas bombas de agua conectadas a 220V con válvulas de agua conectada, esto es lo que quedaría para un estudio posterior. 

Esto mismo ocurrió con el nivel del agua del tanque, en vez de utilizar un flotador digital, lo reemplazamos por un potenciometro el cuál representaría el nivel del agua modificando su tensión mediante un conversor ADC. 

---

# Capítulo 2: Introducción específica
El código de la aplicación se divide conceptualmente en tres módulos temporizados (período de escrutinio de 1 ms):

Módulo de Sensores: Encargado de escrutar el estado físico de los periféricos de entrada. Implementa filtros por máquinas de estado para evitar rebotes o ruidos eléctricos. Una vez que la lectura es estable durante un tiempo determinado (DEL_XX_MAX), emite un evento al sistema.

Módulo de Sistema: Constituye el núcleo lógico de la aplicación. Recibe señales del módulo sensor y despacha órdenes de acción al actuador. Gestiona variables temporizadoras de los ciclos de riego e implementa el funcionamiento de lazo cerrado (esperando el feedback de los relés).

Módulo de Actuador: Ejecuta las acciones físicas (Display, LEDs, Relés, Buzzer, Bluetooth y EEPROM). Delega las tareas de comunicación I2C/SPI/UART a funciones asincrónicas en los drivers de hardware.
Esta sección contiene los requisitos originales y los modificados en el informe de avances, además de los casos de uso. 

## 2.1 Requisitos
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

<p align="center"><em>Tabla 2.1: Requisitos del proyecto</em></p>

---
## **2.2\. Casos de Uso**

En las tablas 2.2 a 2.4 se presentan 3 casos de uso para el sistema.

| Elemento | Definición |
| :---- | :---- |
| **Disparador** | El usuario cambia el estado de un Dip Switch para ingresar al modo de configuración de recetas. |
| **Precondiciones** | El sistema debe estar en modo `NORMAL`. El Display y los botones de navegación deben estar operativos. La comunicación I2C con la EEPROM debe estar inicializada. |
| **Flujo principal** | 1. El sistema detecta el cambio en el Dip Switch y transita al estado `SET_UP`. <br> 2. Se detienen los temporizadores de riego activos y los actuadores pasan a estado seguro. <br> 3. El microcontrolador lee de la EEPROM los parámetros de la receta activa y los muestra en el Display. <br> 4. El usuario navega por el menú interactivo con los botones para modificar el "Tiempo de Riego" y el "Umbral de Temperatura". <br> 5. El usuario presiona el botón de confirmación; el microcontrolador escribe los nuevos valores en la EEPROM externa a través del bus I2C. <br> 6. El usuario regresa el Dip Switch a su posición original y el sistema vuelve al modo `NORMAL` aplicando la nueva receta. |
| **Flujos alternativos** | 5.a. Falla de comunicación I2C con la EEPROM: El sistema muestra un mensaje de "ERROR: EEPROM" en el Display, emite un triple pitido con el Buzzer y mantiene los cambios de forma temporal únicamente en la memoria RAM del microcontrolador. |

<p align="center"><em>Tabla 2.2: Caso de uso 1: Configuración de Recetas de Cultivo (SET_UP)</em></p>

| Elemento | Definición |
| :---- | :---- |
| **Disparador** | El temporizador de software no bloqueante (basado en el Systick de 1ms) alcanza el tiempo de intervalo de riego definido en la receta activa. |
| **Precondiciones** | El sistema se encuentra en modo `NORMAL`. El microcontrolador cargó exitosamente los umbrales de la EEPROM al iniciar. El sensor analógico de nivel de agua registra valores seguros. |
| **Flujo principal** | 1. La máquina de estados del firmware evalúa que el tiempo transcurrido es igual o mayor al intervalo de la receta de la EEPROM. <br> 2. El sistema transita al sub-estado `RIEGO_ACTIVO`. <br> 3. Se acciona el LED/Relé que simula la bomba de agua mediante una salida digital no bloqueante. <br> 4. El Display OLED se actualiza reflejando el estado "Riego en proceso...". <br> 5. Se envía una trama de telemetría por el módulo HM-10 (Bluetooth) informando el evento. <br> 6. Al completarse el tiempo de riego estipulado, la máquina de estados apaga el actuador y regresa al sub-estado de espera. |
| **Flujos alternativos** | 1.a. Si la temperatura medida por el sensor SHT3x supera el umbral de la receta mientras se espera el riego, el microcontrolador activa de manera independiente la salida PWM del ventilador (LED/Relé) sin bloquear la ejecución del Super-Loop. |

<p align="center"><em>Tabla 2.2: Caso de uso 2: Ejecución del Ciclo Automático de Riego (NORMAL)</em></p>

| Elemento | Definición |
| :---- | :---- |
| **Disparador** | El canal de ADC (con adquisición por DMA) registra que la tensión del potenciómetro de nivel de agua cae por debajo del límite mínimo de seguridad. |
| **Precondiciones** | El sistema se encuentra operando en modo `NORMAL` y realizando conversiones periódicas de los canales analógicos de forma no bloqueante. |
| **Flujo principal** | 1. El firmware procesa el bloque de datos del ADC y detecta la condición de tanque vacío. <br> 2. La máquina de estados ejecuta una transición incondicional e inmediata al modo de `FALLA`. <br> 3. Se desactivan de forma instantánea todas las salidas digitales hacia los actuadores (bombas y ventilación apagadas). <br> 4. El Display OLED interrumpe su pantalla actual y muestra de forma intermitente el mensaje "ALERTA: SIN AGUA". <br> 5. Se acciona el Buzzer por hardware para emitir una alarma sonora intermitente y se envía la notificación de emergencia vía Bluetooth (HM-10). |
| **Flujos alternativos** | 5.a. El sistema permanece bloqueado en modo seguro hasta que el usuario reestablezca el nivel (simulado con el potenciómetro) y presione el botón físico de "Reset/Acknowledge" en la placa base para retornar al modo `NORMAL`. |

<p align="center"><em>Tabla 2.4: Caso de uso 3: Bloqueo del Sistema por Nivel Crítico (FALLA)</em></p>

## 2.3 Descripción de módulos principales

En los siguientes puntos desarrollaremos los modulos principales aplicados al proyecto. 

### **2.3.1 Módulo de control y orquestación de la plaqueta NUCLEO**
* Implementado sobre una placa de desarrollo STM32 NUCLEO F103RB.
* Ejecuta un scheduler (Ejecutor Cíclico) basado en arreglos de punteros a funciones, disparando cada 1 ms por el SysTick.
* Centraliza la Máquina de Estados Finita (FSM) gobernando el comportamiento macro de la estación.

### **2.3.2 Módulo de escrutinio de sensores**
* Se encarga de abstraer la capa de hardware físico hacia eventos lógicos (por ej. EV_RAW_DOWN).
* Implementa filtros temporales (debounce para falsos positivos/negativos) de longitud variable: retardos cortos para pulsadores mecánicos (40 ms) y retardos prolongados para lecturas analógicas de ADC que son propensas a ruido y "oleajes" (100 ms).

### **2.3.3 Módulo de medición ambiental**
* Compuesto por el sensor digital AHT20 operando bajo el bus I2C.
* Utiliza un driver propio basado en estados internos que permite enviar el comando de medición y ceder el uso del procesador durante los 80 ms físicos requeridos para el cálculo termodinámico, evitando el uso de funciones bloqueantes.

### **2.3.4 Módulo de actuación e I/O**
* Unificamos el disparo de elementos dispares mediante arrays de configuración.
* Se accionan los relés optoacoplados con lógica de control inversa.
* Gestionamos la atenuación del display LCD 16x2 mediante el uso de una etapa transistorizada (NPN) en paralelo, lo que permite alternar entre el 100% y el 20% de retroiluminación sin requerir módulos PWM complejos.

### **2.3.5 Módulo de memoria**
* Utiliza la memoria externa EEPROM AT24C32 en el bus I2C compartiendo líneas con el sensor AHT20.
* Minimiza la tasa de desgaste de escritura al funcionar como sistema de solo respaldo: se lee una sola vez durante el inicio del sistema (Init) y se escribe de manera exclusiva cuando los valores han sido explícitamente confirmados por el usuario.

### **2.3.6 Módulo de telemetría**
* Integrado mediante el modulo bluetooth HM-10.
* Opera de forma totalmente asincrónica a través del periférico USART de la STM32, donde veremos que su impacto en el rendimiento computacional de la placa es nulo en estado de reposo, procesando tramas únicamente mediante interrupciones.

---

# Capítulo 3: Diseño e implementación
## **3.1 Arquitectura general**
Se aplica un sistema reactivo ("Event-Triggered"), en el que los módulos se comunican internamente levantando y consumiendo eventos (ej. EV_SYS_TEMP_HIGH, EV_ACT_PUMP_ON). La transición de estados está dictaminada por condiciones de guarda ([guard]) ligadas a contadores temporales internos.
## **3.2 Diseño de hardware**
El hardware constará de una placa base para los siguientes periféricos:

* Entradas: Sensor de temperatura (AHT20 por I2C), simulación de nivel de agua (potenciómetro con filtro activo Sallen-Key vía ADC con DMA), teclado matricial y feedback de tensión de relés.

* Salidas: Relés para bomba de agua y ventilador, Buzzer de alarma, LEDs indicadores, módulo Bluetooth HM-10 (UART), memoria EEPROM (I2C) y Display OLED (SPI/I2C).

## **3.3 Diseño de firmware**
### **3.3.1 Máquina de estados del Sistema**
Define el modo de operación global:

* ST_SYS_NORMAL_IDLE: Monitoreo del clima y espera hasta el próximo riego.

* ST_SYS_NORMAL_WATERING: Riego activo transitorio (bomba encendida).

* ST_SYS_SET_UP: Navegación por menú interactivo (ciclo automático detenido).

* ST_SYS_ERROR: Bloqueo del sistema con activación de alarmas (por tanque vacío o falla de relé).

### **3.3.2 Máquinas de estado de los Sensores**
Se utilizan para filtrar entradas físicas inestables:

* Teclado y Relé: Transitan por UP/OPEN -> FALLING/CLOSING -> DOWN/CLOSED -> RISING/OPENING.

* Nivel de agua y Temperatura: Transitan desde estado OK hacia CRITICAL/HIGH mediante estados transitorios FALLING/RISING para asegurar la permanencia en el umbral crítico antes de lanzar la alarma.

### **3.3.3 Máquinas de estado de los Actuadores**
Controlan periféricos sin usar retardos:

* Buzzer: Alterna entre ON, OFF cíclicamente para alarmas o emite tonos de confirmación.

* LEDs: Estados fijos (Verde, Amarillo, Rojo) o parpadeantes para actualizaciones de pantalla.

* Display: Basado en pantallas (TELEMETRY, MENU, FAULT).

* Buses (BT/EEPROM): Estados que manejan la transmisión y recepción (IDLE, TX_BUSY, RX_READY, ERROR).
---

# Capítulo 4: Ensayos y resultados
## **4.1 Pruebas funcionales de hardware**
## **4.2 Pruebas funcionales de firmware**
## **4.3 Pruebas de integración**
## **4.4 Medición y análisis de consumo**



---

# Capítulo 5: Conclusiones

## 5.1 Resultados obtenidos



## 5.2 Lecciones aprendidas



## 5.3 Próximos pasos



---

# Capítulo 6: Uso de herramientas de IA

Se documenta el uso de IA según requerimiento docente y archivo `listado de cosas hechas con IA.txt`.

## 6.1 Uso individual y conjunto



---

# Capítulo 7: Bibliografía y referencias



Referencias internas del repositorio:

---

**Fin de la Memoria Técnica**  
Autores: Ignacio Avincetto, Franco Joaquín, Nishihara, Leonardo
Fecha de edición: 
