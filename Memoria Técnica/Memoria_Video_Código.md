<div align="center">

<img width="762" height="292" alt="logo-fiuba" src="https://github.com/user-attachments/assets/9c859836-a677-4f4c-b163-af23d4892aa2" />


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

*Trabajo realizado entre julio de 2026 y agosto de 2026*
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
| 1.0 | Entrega de la estructura básica de la memoria | 10/07/2026 |
| 2.0 | Update código: Versión 2.0 con correcciones | 10/08/2026 |
| 3.0 | Update código:Versión 3.0 con correcciones de errores y nuevas optimizaciones para el control de las aplicaciones del proyecto. | 13/08/2026 |




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
    - [2.3.1 Módulo de control y orquestación de la plaqueta NUCLEO](#231-módulo-de-control-y-orquestación-de-la-plaqueta-nucleo)
    - [2.3.2 Módulo de escrutinio de sensores](#232-módulo-de-escrutinio-de-sensores)
    - [2.3.3 Módulo de medición ambiental](#233-módulo-de-medición-ambiental)
    - [2.3.4 Módulo de actuación e I/O](#234-módulo-de-actuación-e-io)
    - [2.3.5 Módulo de memoria](#235-módulo-de-memoria)
    - [2.3.6 Módulo de telemetría](#236-módulo-de-telemetría)
- [Capítulo 3: Diseño e implementación](#capítulo-3-diseño-e-implementación)
  - [3.1 Arquitectura general](#31-arquitectura-general)
  - [3.2 Diseño de hardware](#32-diseño-de-hardware)
  - [3.3 Diseño de firmware](#33-diseño-de-firmware)
    - [3.3.1 Máquina de estados del Sistema](#331-máquina-de-estados-del-sistema)
    - [3.3.2 Máquinas de estado de los Sensores](#332-máquinas-de-estado-de-los-sensores)
    - [3.3.3 Máquinas de estado de los Actuadores](#333-máquinas-de-estado-de-los-actuadores)
    - [3.3.4 Driver I2C Asincrónico y Memoria](#334-driver-i2c-asincrónico-y-memoria)
    - [3.3.5 Recepción Bluetooth por Interrupción](#335-recepción-bluetooth-por-interrupción)
- [Capítulo 4: Ensayos y resultados](#capítulo-4-ensayos-y-resultados)
  - [4.1 Pruebas funcionales de hardware](#41-pruebas-funcionales-de-hardware)
  - [4.2 Pruebas funcionales de firmware](#42-pruebas-funcionales-de-firmware)
  - [4.3 Pruebas de integración](#43-pruebas-de-integración)
  - [4.4 Medición y análisis de consumo](#44-medición-y-análisis-de-consumo)
  - [4.5 Console and Build Analyzer](#45-console-and-build-analyzer)
  - [4.6 Medición y análisis de WCET por tarea](#46-medición-y-análisis-de-wcet-por-tarea)
  - [4.7 Cálculo del factor de uso de CPU (U)](#47-cálculo-del-factor-de-uso-de-cpu-u)
  - [4.8 Gestión de bajo consumo y justificación](#48-gestión-de-bajo-consumo-y-justificación)
  - [4.9 Cumplimiento de requisitos](#49-cumplimiento-de-requisitos)
  - [4.10 Comparación con sistemas similares](#410-comparación-con-sistemas-similares)
  - [4.11 Documentación del desarrollo realizado](#411-documentación-del-desarrollo-realizado)
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

<div align="center">
  <img width="616" height="269" alt="HidroAut" src="https://github.com/user-attachments/assets/7c17ed1f-85bf-4241-9ebe-7be805feab15" />
  <br>
<em>Figura 1.1 — Cultivo hidropónico de lechugas.</em>
</div>
<br>


## 1.2 Productos comparables
Para la selección de la implementación, se evaluaron tres enfoques principales, ponderados según disponibilidad de hardware, impacto, costo, dificultad técnica e interés del equipo:

* **Proyecto Base:** Monitoreo de nivel de agua (simulado con potenciómetro y filtro activo Sallen-Key) y clima (sensor SHT3x/AHT20). Incluye relés, LEDs para bombas/ventilador y telemetría por Bluetooth (HM-10).

* **Base + Memoria y Display:** Suma una pantalla OLED para un menú interactivo y memoria EEPROM para almacenar "recetas" de cultivo.

* **Base + Control de Nutrientes (pH/EC):** Agrega potenciómetros y filtros para simular lectura de pH/EC, con actuadores extra para dosificadores.

En el mercado actual podremos observar que hay distintos tipos de controladores de cultivos, empezando desde lo más básico como un controlador de riego el cual lo que hace es controlar el cierre de una salida de agua según el intervalo de tiempo que se ajuste, y por otro lado tenemos aquellos los cuáles se le suman características tales como controlador mediante Wi-Fi, detector de lluvia, etc. De estos podemos decir dos cosas comparandolos con nuestro caso.

1. **Temporizadores básicos:** Son soluciones económicas y ampliamente utilizadas. Sin embargo, operan a lazo abierto y carecen de entradas para medir el nivel de agua o el estado del entorno. Al no tener retroalimentación, encenderán la bomba independientemente de si hay agua en el tanque, anulando cualquier tipo de seguridad para el hardware.

<div align="center">
  <img width="337" height="289" alt="Rainbird" src="https://github.com/user-attachments/assets/c30de87e-6b10-4aa4-9348-cfda652558d7" />
<br>
<em>Figura 1.2 — Programador Riego Por Goteo Grifo Automatico Rain Bird.</em>
  </div>
<br>

2. **Enchufes y relés inteligentes:** Proveen conectividad Wi-Fi y control de horarios mediante aplicaciones móviles, donde su principal desventaja es la dependencia crítica de la red Wi-Fi doméstica y servidores. En este caso, un microcorte de internet o una falla en el router local puede dejar al dispositivo inoperante o desconectado de sus rutinas. Además, los de este tipo no suelen contar con interfaces físicas locales (pantallas informativas) para un diagnóstico rápido en el lugar de la instalación.

<div align="center">
  <img width="341" height="333" alt="releINT" src="https://github.com/user-attachments/assets/4944cc5c-b6fb-482a-a538-b443d2c3c64f" />
<br>
<em>Figura 1.3 — Rele Shelly Plus 1.</em>
  </div>
<br>

## 1.3 Justificación del enfoque técnico

El sistema se implementará utilizando una arquitectura de software estrictamente no bloqueante. Se utilizará un microcontrolador STM32 programado en bare-metal bajo el patrón de Super-Loop. Las secuencias que requieren control temporal se resolverán mediante máquinas de estado internas que gestionan los "ticks" del sistema (1 ms), evitando por completo el uso de funciones de retraso pasivo (delay).

Como se mencionó anteriormente, la integración de periféricos con altas latencias se resolvió mediante máquinas de estados no bloqueantes:
* **Comunicaciones I2C**: Se evitaron rutinas bloqueantes para esperar los tiempos de conversión térmica (~80 ms) o los tiempos de escritura de celdas (~5 ms). Se desarrollaron drivers específicos que ceden el control al procesador inmediatamente, retomando la lectura solo cuando el hardware indica disponibilidad.
* **Módulo Bluetooth**: Se atiende exclusivamente mediante interrupciones asincrónicas de hardware y buffers, asegurando que la recepción de un comando no viole las restricciones temporales que se imponen.

## 1.4 Alcance y limitaciones
El alcance del sistema contempla el monitoreo de nivel de agua, temperatura y humedad, la actuación de bombas de agua y ventiladores mediante relés de potencia, alarmas acústicas/visuales y la transmisión de información remota. Todo evento externo e interno se somete a validación mediante filtrado de estado temporal para evadir falsos positivos y rebotes.

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

<p align="center"><em>Tabla 2.3: Caso de uso 2: Ejecución del Ciclo Automático de Riego (NORMAL)</em></p>

| Elemento | Definición |
| :---- | :---- |
| **Disparador** | El canal de ADC (con adquisición por DMA) registra que la tensión del potenciómetro de nivel de agua cae por debajo del límite mínimo de seguridad. |
| **Precondiciones** | El sistema se encuentra operando en modo `NORMAL` y realizando conversiones periódicas de los canales analógicos de forma no bloqueante. |
| **Flujo principal** | 1. El firmware procesa el bloque de datos del ADC y detecta la condición de tanque vacío. <br> 2. La máquina de estados ejecuta una transición incondicional e inmediata al modo de `FALLA`. <br> 3. Se desactivan de forma instantánea todas las salidas digitales hacia los actuadores (bombas y ventilación apagadas). <br> 4. El Display OLED interrumpe su pantalla actual y muestra de forma intermitente el mensaje "ALERTA: SIN AGUA". <br> 5. Se acciona el Buzzer por hardware para emitir una alarma sonora intermitente y se envía la notificación de emergencia vía Bluetooth (HM-10). |
| **Flujos alternativos** | 5.a. El sistema permanece bloqueado en modo seguro hasta que el usuario reestablezca el nivel (simulado con el potenciómetro) y presione el botón físico de "Reset/Acknowledge" en la placa base para retornar al modo `NORMAL`. |

<p align="center"><em>Tabla 2.4: Caso de uso 3: Bloqueo del Sistema por Nivel Crítico (FALLA)</em></p>

## 2.3 Descripción de módulos principales

En los siguientes puntos desarrollaremos los modulos principales aplicados al proyecto. 

### 2.3.1 Módulo de control y orquestación de la plaqueta NUCLEO
* Implementado sobre una placa de desarrollo STM32 NUCLEO F103RB.
* Ejecuta un scheduler (Ejecutor Cíclico) basado en arreglos de punteros a funciones, disparando cada 1 ms por el SysTick.
* Centraliza la Máquina de Estados Finita (FSM) gobernando el comportamiento macro de la estación.
<div align="center">
<img width="500" alt="image PF259875 en feature-description-include-personalized-no-cpn-large" src="https://github.com/user-attachments/assets/22dc6ffa-1342-477c-853c-d37268ec9ac3" />
<br>
<em>Figura 2.1: Placa NUCLEO-F103RB.</em>
  </div>
<br>

### 2.3.2 Módulo de escrutinio de sensores
* Se encarga de abstraer la capa de hardware físico hacia eventos lógicos (por ej. EV_RAW_DOWN).
* Implementa filtros temporales (debounce para falsos positivos/negativos) de longitud variable: retardos cortos para pulsadores mecánicos (40 ms) y retardos prolongados para lecturas analógicas de ADC que son propensas a ruido y "oleajes" (100 ms).

### 2.3.3 Módulo de medición ambiental
* Compuesto por el sensor digital AHT20 operando bajo el bus I2C.
* Utiliza un driver propio basado en estados internos que permite enviar el comando de medición y ceder el uso del procesador durante los 80 ms físicos requeridos para el cálculo termodinámico, evitando el uso de funciones bloqueantes.

<div align="center">
<img width="350" alt="AHT20" src="https://github.com/user-attachments/assets/cc2b5e63-a28b-4fe5-93e3-87fdc730b1f0" />
<br>
<em>Figura 2.2: Módulo sensor de temperatura y humedad AHT20.</em>
  </div>
<br>

### 2.3.4 Módulo de actuación e I/O
* Unificamos el disparo de elementos dispares mediante arrays de configuración.
* Se accionan los relés optoacoplados con lógica de control inversa.
* Gestionamos la atenuación del display LCD 16x2 mediante el uso de una etapa transistorizada (NPN) en paralelo, lo que permite alternar entre el 100% y el 20% de retroiluminación sin requerir módulos PWM complejos.

### 2.3.5 Módulo de memoria
* Utiliza la memoria externa EEPROM AT24C32 en el bus I2C compartiendo líneas con el sensor AHT20.
* Minimiza la tasa de desgaste de escritura al funcionar como sistema de solo respaldo: se lee una sola vez durante el inicio del sistema (Init) y se escribe de manera exclusiva cuando los valores han sido explícitamente confirmados por el usuario.

<div align="center">
<img width="517" alt="EEPROM" src="https://github.com/user-attachments/assets/5d942240-1d98-47d0-aa1e-80d448e1851c" />
<br>
<em>Figura 2.3: Módulo EEPROM AT24C32.</em>
  </div>
<br>

### 2.3.6 Módulo de telemetría
* Integrado mediante el modulo bluetooth HM-10.
* Opera de forma totalmente asincrónica a través del periférico USART de la STM32, donde veremos que su impacto en el rendimiento computacional de la placa es nulo en estado de reposo, procesando tramas únicamente mediante interrupciones.

<div align="center">
<img width="350" alt="HM-10" src="https://github.com/user-attachments/assets/cca3fa04-a837-4e46-8d8e-a35838bd8e81" />
<br>
<em>Figura 2.4: Módulo Bluetooth bajo consumo HM-10.</em>
  </div>
<br>

---

# Capítulo 3: Diseño e implementación
## **3.1 Arquitectura general**
Se aplica un sistema reactivo ("Event-Triggered"), en el que los módulos se comunican internamente levantando y consumiendo eventos (ej. `EV_SYS_TEMP_HIGH`, `EV_ACT_PUMP_ON`). La transición de estados está dictaminada por condiciones de guarda ([guard]) ligadas a contadores temporales internos.

### Dominios de Hardware
* Dominio lógico (3,3 V): Microcontrolador STM32 NUCLEO, señales de los pulsadores, potenciómetro (ADC) y líneas de comunicación (I2C y UART).
* Dominio de actuación y potencia (5 V aislados): Pantalla LCD, alimentación del módulo Bluetooth, y módulo de relés optoacoplados, los cuales actúan como barrera entre la lógica y las cargas de potencia de la bomba de agua.

### Capas de Software
Para poder generar el software necesario para el proyecto nos basamos en la estructrura de aplicación modular, ya que divide de manera independiente cada parte del software haciendolo de esta manera reutilizable ante cualquier cambio que surja. En nuestro proyecto, cada modulo proyecta una funcionalidad diferente pero que se complementan en un punto para generar un sistema de uso continuo. Las capas son las siguientes:

* **Capa de Sensores (Escrutar)**: Realiza una traducción de las magnitudes físicas y eléctricas (pulsaciones, niveles de tensión, tramas UART, lecturas I2C) en "eventos lógicos" limpios.
* **Capa del Sistema (Procesar)**: Una máquina de estados central que toma los eventos lógicos, evalúa los temporizadores de las recetas, y decide el próximo estado del sistema.
* **Capa de Actuadores (Actuar)**: Ejecuta los comandos físicos ordenados por el sistema (conmutar relés, sonar alarmas, dibujar caracteres en el LCD).

Primero realizamos la estructurazión a partir de los componentes que podría llegar a tener el sistema, haciendo un Diagrama en Bloques. Seguido a eso, completamos un primer Diagrama de Secuencia que reflejaba prácticamente lo mismo que el de bloques, y pasamos a definir los eventos y estados que deberían tener en un principio.
En los enlaces siguientes se presentan en detalle las tablas de estados definidas para cada módulo.

[Módulo del Sistema](https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/STATE%20CHARTS/tdse_tf_system.md)

[Módulo de Actuadores](https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/STATE%20CHARTS/tdse_tf_actuator.md)

[Módulo de Sensores](https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/STATE%20CHARTS/tdse_tf_sensor.md)

Una vez logrado eso, completamos un último Diagrama de Secuencia donde en los mensajes contenía la impronta de lo que iba a volcarse en el código.

<img width="1125" height="689" alt="image" src="https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/IMAGENES/diagrama_en_bloques.png" />
<p align="center"><em>Figura 3.1: Diagrama de bloques</em></p>

<img width="1125" height="689" alt="image" src="https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/IMAGENES/Diagrama%20de%20Secuencia%20Final%20-%20Estaci%C3%B3n%20Hidrop%C3%B3nica.png" />
<p align="center"><em>Figura 3.2: Diagrama de Secuencia</em></p>

## 3.2 Diseño de hardware
El hardware constará de una placa base para los siguientes periféricos:



<div align="center">
<img width="1594" height="877" alt="Esquematico" src="https://github.com/user-attachments/assets/29a8cf53-914c-400d-8a19-9a03485117c1" />
<br>
<em>Figura 3.3: Esquemático del sistema.</em>
  </div>
<br>

* Entradas: Sensor de temperatura (AHT20 por I2C), simulación de nivel de agua (potenciómetro con filtro activo Sallen-Key vía ADC con DMA), teclado matricial y feedback de tensión de relés.

* Salidas: Relés para bomba de agua y ventilador, Buzzer de alarma, LEDs indicadores, módulo Bluetooth HM-10 (UART), memoria EEPROM (I2C) y Display OLED (SPI/I2C).

## 3.3 Diseño de firmware
El sistema no utiliza un RTOS, sino una arquitectura bare-metal soportada por un Ejecutor Cíclico (Super-Loop). La interrupción a partir de SysTick levanta un flag cada 1ms exacto, lo que permite que la función principal main() vaya despachando cíclicamente el array de las tareas en task_cfg_list:
1. `task_sensor_update()`
2. `task_system_update()`
3. `task_actuator_update()`
4. `task_display_update()`
5. `bluetooth_update()`

### 3.3.1 Máquina de estados del Sistema
El "cerebro" del sistema se origina a partir de distintos modos de operaciones que están acoplados entre sí, los definimos de manera global:

* **MODO NORMAL**: Gestiona la cuenta regresiva de la receta actual y activa el riego. Paralelamente, implementa un temporizador no bloqueante para el "Carrusel del Display", rotando la información mostrada cada 2 segundos. Este modo, posee los siguientes subestados:

  * `ST_SYS_NORMAL_IDLE`: Monitoreo del clima y espera hasta el próximo riego. 

  * `ST_SYS_NORMAL_WATERING`: Riego activo transitorio (bomba encendida).

* **MODO SETUP**: Detiene la actualización dinámica del display para permitir la navegación manual por las líneas del menú, utilizando variables como menu_linea_actual.

  * `ST_SYS_SET_UP`: Navegación por menú interactivo (ciclo automático detenido).

* **MODO ERROR**: Un "trap state" del cual solo se puede salir mediante la restauración de las variables físicas críticas (por ej. recarga del agua del tanque). 
  * `ST_SYS_ERROR`: Bloqueo del sistema con activación de alarmas (por tanque vacío o falla de relé).

Para saltar de un modo de operación a otro, se creo la función `task_system_set_mode()` la cual tiene como función ser la mensajera entre los modos, copiando las variables temporales (por ej. para las recetas) hacia la estructura del nuevo estado antes de que se realice la transición. Haciendolo de esta manera, se garantiza que el estado entrante siempre disponga de la información más reciente sin utilizar la EEPROM como puente. 

### 3.3.2 Máquinas de estado de los Sensores
El módulo de los sensores evalúa las entradas mediante un bucle de indexación, sin importar que la entrada sea un botón (GPIO digital) o el nivel de agua (ADC), la lectura se traduce a una variable booleana unificada is_active.
Se utilizan para filtrar entradas físicas inestables:

* Teclado y Relé: Transitan por UP/OPEN -> FALLING/CLOSING -> DOWN/CLOSED -> RISING/OPENING.

* Nivel de agua y Temperatura: Transitan desde estado OK hacia CRITICAL/HIGH mediante estados transitorios FALLING/RISING para asegurar la permanencia en el umbral crítico antes de lanzar la alarma.

### 3.3.3 Máquinas de estado de los Actuadores
Controlan periféricos sin usar retardos:

* Buzzer: Alterna entre ON, OFF cíclicamente para alarmas o emite tonos de confirmación.

* LEDs: Estados fijos (Verde, Amarillo, Rojo) o parpadeantes para actualizaciones de pantalla.

* Display: Basado en pantallas (TELEMETRY, MENU, FAULT).

* Buses (BT/EEPROM): Estados que manejan la transmisión y recepción (IDLE, TX_BUSY, RX_READY, ERROR).

### 3.3.4 Driver I2C Asincrónico y Memoria
* Sensor AHT20: Se implementó una máquina de estados interna en aht20.c. En el estado inicial envía el comando de medición I2C (0xAC) e inicia un contador local de 80 ticks (80 ms). Un vez cumplido el tiempo, el estado transita para conseguir los 6 bytes del bus I2C, ensamblando los 20 bits de temperatura y humedad mediante desplazamientos lógicos sin que se bloquee el procesador. 

* Memoria EEPROM: Las variables de tipo entero sin signo de 32 bits (uint32_t) se fragmentan en 4 bytes individuales para su almacenamiento. Para evitar el desgaste excesivo sobre las celdas de la memoria, las lecturas ocurrirán únicamente al invocar `task_system_init()` (durante el arranque), y la escritura (`eeprom_write_uint32()`) se ejecuta exclusivamente tras presionar el botón CONFIRMAR o recibir un comando Bluetooth válido, descartando de esta manera las escrituras periódicas en tiempo real.

### 3.3.5 Recepción Bluetooth por Interrupción
En este caso, utilizando el módulo HM-10 sin ciclos de polling, ya que para esto se habilitó la interrupción global USART y el uso de `HAL_UART_RxCpltCallback`, así que cada vez que ingresa un byte de forma inalámbrica, se almacena en un buffer circular de 32 posiciones (`rx_buffer`). 

Al detectarse el carácter del CR (\r o \n), la bandera msg_ready cambia de estado y al siguiente ciclo del milisegundo, la función de actualización `bluetooth_update()` procesa el mensaje completo (ej. identificando unas tramas del formato "R=5000"). Luego, convertimos el valor a formato numérico con la función `atoi()`, valida las restricciones de seguridad, actualiza directamente la memoria de la receta y responde automáticamente con un mensaje TX de confirmación hacia la aplicación celular.


---

# Capítulo 4: Ensayos y resultados
## 4.1 Pruebas funcionales de hardware
Para comenzar con las pruebas del hardware, se decidió ir incluyendo los distintos procesos/módulos de a poco, empezando con los módulos de actuadores, seguido del módulo de los sensores, y por último el modulo del sistema. 

Lo primero que probamos en el módulo de actuadores, fueron los módulos de relés y el de leds, esta primer prueba era fundamental, ya que la estación hidropónica no sería nada si no se pudieran mover los relés con los que se mueven la bomba del tanque de agua y el ventilador. Algo a destacar para con el módulo de relés es que no lo alimentamos directamente con la plaqueta NUCLEO sino que optamos por utilizar una fuente externa (5V 400mA), sabiendo que es capaz de entregar hasta 100mA como máxima, y el módulo de relés puede llegar a consumir entre unos 50mA por canal. Por lo tanto, antes de comenzar a hacer algo con el resto de módulos, decidimos medir la corriente que consumían los relés conectado con la fuente externa, y es acá donde pudimos observar que el consumo de cada módulo rondaba entre 54mA y 57mA, lo que nos daba un consumo total entre ambos de unos 108mA hasta 114mA, superando ampliamente la cantidad que nos podía dar la placa NUCLEO.

La prueba que deberían pasar estos módulos es que se prendan los LEDs que se incluyen en la plaqueta del módulo y que además se escuche el click mecánico que contienen los relés, y luego, por otro lado para el módulo de semáforo que se prendan los LEDs de colores en una cierta secuencia. Para todo esto utilizamos códigos de pruebas dentro de los códigos ya generados para lo que se convertiría en el sistema final.

Una vez confirmamos que los actuadores respondían de manera efectiva, pasamos a probar los sensores, en este caso los primeros fueron los botones para navegar el menú del display y otro de los más importantes, el potenciómetro con el que íbamos a simular el nivel de agua utilizando el conversor ADC que nos brinda la placa. 

Para el potenciómetro se decidió utilizar uno de valor 1kΩ, ya que estos conversores trabajan cargando un capacitor de retención, donde para que la lectura sea precisa y rápida, la señal externa debe tener una baja impedancia. Por lo tanto, al estar conectado a la fuente de 3.3V de la placa, este nos entregaba un consumo que se aproximaba los 3.3mA que debería tener.

Para la siguiente prueba ya incluimos al módulo del sistema, donde teníamos como principal sujeto de prueba el módulo de las memorias EEPROM y el display, acá verificamos que lo que se guardaba en pantalla, luego quedaba en la memoria y viceversa. Para el módulo de memorias no hizo falta conectarles resistencias de pull-up, ya que estas venían incluidas dentro del modulo en el que se conserva la memoria.

<img width="1125" height="689" alt="image" src="https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/IMAGENES/prueba1.jpg" />
<p align="center"><em>Figura 4.1: Primeras pruebas en protoboard - Funcionamiento del Modo NORMAL</em></p>

<img width="1125" height="689" alt="image" src="https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/IMAGENES/prueba2.jpg" />
<p align="center"><em>Figura 4.2: Primeras pruebas en protoboard - Funcionamiento del Modo SETUP</em></p>

<img width="1125" height="689" alt="image" src="https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/IMAGENES/prueba3.jpg" />
<p align="center"><em>Figura 4.3: Primeras pruebas en protoboard - Funcionamiento del Modo ERROR</em></p>

| Ensayo | Resultado | Estado |
| --- | --- | :---: |
| Integridad de placas (continuidad) | Validación previa a energización | ✅ |
| Conectar amperímetro para observar la carga de los relés | Implementación de fuente externa para poder alimentar los relés | ✅ |
| Enviar señales al módulo de relés y esperar la respuesta física | Se escuchó el ruido mecánico característico de relés y encendido de LEDs | ✅ |
| Variar potenciómetro del nivel de agua para observar el conversor ADC | Verificación mediante multímetro | ✅ |
| Verificación de tensión en comunicaciones físicas para las líneas I2C y UART | Valores dentro de lo previsto |✅|

<p align="center"><em>Tabla 4.1: Ensayos y Resultados de Hardware.</em></p>

## 4.2 Pruebas funcionales de firmware
Para las primeras pruebas del firmware, ya con actuadores y sensores funcionando, incluimos ahora al modulo de sistema, y en conjunto a este conectamos el display, para comenzar a ver una primera impresión de cómo sería el sistema completo. En este punto, dejamos de lado los "códigos de prueba", y ya utilizábamos el código de cómo debería funcionar realmente en el sistema final solo que incluíamos algún `LOGGER_INFO` para hacer troubleshooting con alguno de los módulo. Ejemplo: `[info] >>> SENSOR: ID 3 confirmado como PRESIONADO!` así verificabamos que el DEBOUNCE fue implementado de manera correcta.

En esta prueba comprobamos, que el display respondía a los botones para la navegación del sistema, y los tres modos de operación que iba a incluir nuestro sistema, NORMAL, SETUP y ERROR. Este paso fue fundamental, ya que se corrigieron bastantes cosas para pasar de una máquina de estados a la otra, y así poder hacer que la transición entre un sistema y el otro sea algo completamente cíclico y sin ninguna traba de por medio. 

Una vez que pudimos comprobar que las comunicaciones físicas entre los módulos estaban funcionando, pasamos a incluir al módulo bluetooth a las pruebas y ver que la conexión inalámbrica del sistema funcione bien. Para establecer la comunicación entre los dispositivos se utilizó una aplicación descargada de Google Play Store llamada Serial Bluetooth Terminal, esta aplicación contiene una terminal donde podremos enviar y recibir los mensajes. 

En la prueba de la conexión inalámbrica se tuvieron que realizar numerosas modificaciones, por ejemplo, cuando encendíamos el sistema, y no nos conectábamos al módulo bluetooth, este comenzaba a enviarse comandos "perdidos" entre la placa NUCLEO y el módulo haciendo que se genere un bucle infinito de preguntas y respuestas sin sentido. La solución para ese caso fue implementar un filtro para aquellos mensajes que se generen entre placa y módulo cuando no hay nadie conectado, y lo mismo para cuando si hay alguien conectado.

<div align="center">
  
| Ensayo | Resultado | Estado |
| --- | --- | :---: |
| Debounce botones ON/OFF | Eventos limpios sobre FSM | ✅ |
| FSM de sistema (`NORMAL/SETUP/ERROR`) | Transiciones válidas en logs | ✅ |
| Persistencia flash | Lectura/escritura de estado y calibración | ✅ |
| Telemetría BT | Tramas de envío y recepción correctas | ✅ |

</div>

<p align="center"><em>Tabla 4.2: Ensayos y Resultados de Software.</em></p>

## 4.3 Pruebas de integración
La integración del proyecto es algo que desarrollamos en cada paso a medida que íbamos incluyendo más módulos para probar, como primer instancia teníamos la parte física, el medir las señales que les llegaban, si el consumo de las mismas era el correcto por ejemplo, y como segunda instancia teníamos la ejecución del código, y ver como reaccionaba a los distintos módulos. Pero todo eso lo realizábamos prácticamente en el mismo momento, ya que podía pasar que la conexión entre placa y módulo era correcta, pero luego cuando queríamos ejecutar una acción sobre estos, no ocurría nada por algún desliz en el código o configuración de los pines. 

Por esto, es que decidimos ir probando módulo por módulo, incluyéndolos de a poco y una vez que teníamos el sistema completo funcional, pasamos a la parte de desarrollar el sistema en un PCB del tipo perforado.

<img width="1125" height="689" alt="image" src="https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/IMAGENES/prueba4.jpg" />
<p align="center"><em>Figura 4.4: Últimas pruebas en protoboard - Funcionamiento del Sistema Completo</em></p>

Comenzamos desarrollando una plaqueta la cual esta iba a contener la mayoría de módulos y sensores del sistema, ya que es dónde íbamos a colocar la placa NUCLEO, que es la que comanda todas las operaciones para que el sistema funcione. En este fuimos integrando poco a poco las conexiones con cables del tipo multipar, asegurando primero, las conexiones de GND, de 5V y 3.3V y luego pasar a las conexiones de los pines de funcionalidad que necesitaba cada módulo en particular. Por otro lado, se decidió utilizar un segundo PCB perforado para no saturar tanto las conexiones dentro del PCB principal, y además, en este segundo PCB podríamos aislar las conexiones al relé, el cual tiene la conexión a la fuente externa, y otros módulos como el de memoria EEPROM, y el módulo AHT20, sensor de temperatura y humedad.

<img width="1125" height="689" alt="image" src="https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/IMAGENES/pcb_principal.jpg" />
<p align="center"><em>Figura 4.5: Esquemático del PCB principal del sistema completo</em></p>

<img width="1125" height="689" alt="image" src="https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/IMAGENES/pcb_secundario.jpg" />
<p align="center"><em>Figura 4.6: Esquemático del PCB secundario del sistema completo</em></p>

<img width="1125" height="689" alt="image" src="https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/IMAGENES/pcb_completa.jpg" />
<p align="center"><em>Figura 4.7: Vista en general del sistema completo</em></p>

**Video de integración en funcionamiento**  

[Link al vídeo de integración del proyecto](https://youtu.be/7Y0e8ubY0-0)

[Log generado durante el vídeo](https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/log%20del%20video.txt)

## 4.4 Medición y análisis de consumo
Para la medición y análisis de consumo separamos en dos partes, debido a que se hicieron uso de los dos tipos de voltajes que nos podía entregar la placa del sistema, una alimentación de 5V y la otra de 3.3V. Por otro lado, teníamos el consumo de la fuente externa para el módulo de relés que se había medido previamente sin tener el sistema completa, la cuál de igual manera una vez insertada al resto de módulos no varió su consumo, ya que esta fuente simplemente actúa para encender los relés, este módulo por otro lado también tiene una alimentación de 3.3V del sistema que es para la activación lógica de los pines IN1 y IN2 que están en la placa del módulo de relés.  

Metodología aplicada:
- Medición de consumo total en la entrada de `5 V` del sistema.
- Medición de consumo total en la entrada de `3.3 V` del sistema.
- Alimentación desde fuente externa conectada a pines `5 V` y `GND` para el módulo de relés.
- Medición de tensión en bornes de entrada para estimar potencia (`P = V * I`).

| Modo | $I_{pico}$ @ 5 V [mA] | $P_{pico}$ @ 5 V [W] | Observaciones |
| --- | ---: | ---: | --- |
| PAUSADO | 12 | 0.060 | Escenario de menor consumo; es apenas se inicia el sistema. |
| SETUP | 12.5 | 0.062 | El sistema está semi-pausado; se guardan las recetas en la memoria EEPROM. |
| NORMAL - ESPERA | 13.40 | 0.067 | El sistema ya tiene cargada la receta y con todas las funciones listas.  |
| NORMAL - REGANDO  | 13.90 | 0.069 | El sistema envía las señales para activar los relés. |
| ERROR | 14.1 | 0.070 | El sistema se pausa completamente; se produce un pitido constante mediante Buzzer y envía notificación por bluetooth. |

<p align="center"><em>Tabla 4.3: Consumo total medido a 5 V (valores pico).</em></p><br><br>

| Modo | $I_{pico}$ @ 3.3 V [mA] | $P_{pico}$ @ 3.3 V [W] | Observaciones |
| --- | ---: | ---: | --- |
| PAUSADO | 3.88 | 0.0128 | Escenario de menor consumo; es apenas se inicia el sistema. |
| SETUP | 4.40 | 0.0145 | El sistema está semi-pausado; se guardan las recetas en la memoria EEPROM. |
| NORMAL - ESPERA | 4.80 |  0.0158 | El sistema ya tiene cargada la receta y con todas las funciones listas.  |
| NORMAL - REGANDO  | 5.20 | 0.0172 | El sistema envía las señales para activar los relés. |
| ERROR | 4.24 | 0.0140 | El sistema se pausa completamente; se produce un pitido constante mediante Buzzer y envía notificación por bluetooth. |

<p align="center"><em>Tabla 4.4: Consumo total medido a 3.3 V (valores pico).</em></p><br><br>

**Consumo de relés durante los distintos modos de operación** 

El sistema siempre se carga con los valores que pueda encontrar en la memoria, en este caso traemos una situación en la cuál es la primera vez que se comienza a utilizar la estación por lo que carga unos valores por default, tanto para el tiempo de espera como para el riego son 10s, y para la temperatura máxima son 28°C y la mínima son 24°C. Esto se aclara ya que el sistema se pondría en funcionamiento con estos valores por si no se quisiera pasar por el menú de SETUP. En el caso siguiente, se propone activar el relé del ventilador para saber cuánto consume este solo mientras que el de la bomba se apagará y prenderá a medida que se pase del modo ESPERA a REGANDO, y de esta manera podremos observar el consumo completo de los relés en funcionamiento.

| Modo | $I_{pico}$ @ 5 V [mA] | $P_{pico}$ @ 5 V [W] | Observaciones |
| --- | ---: | ---: | --- |
| PAUSADO | 0 | 0 | Inicio del sistema; relés apagados. |
| SETUP | 0 | 0 | Pasamos del modo PAUSADO a SETUP, relés apagados. |
| NORMAL - ESPERA | 57 |  0.285 | El sistema ya tiene cargada la receta y con todas las funciones listas.  |
| NORMAL - REGANDO  | 112 | 0.560 | El sistema envía las señales para activar los relés. |
| ERROR | 57 | 0.285 | ERROR de TANQUE VACÍO, el relé del ventilador seguirá prendido, por lo tanto sigue consumiendo. |

<p align="center"><em>Tabla 4.5: Consumo total medido a 5 V de fuente externa (valores pico).</em></p><br><br>

Análisis:
- La potencia se calculó como `P = V * I`, usando `V = 5 V` y corriente pico medida en cada modo.
- El peor caso medido con la alimentación de la placa fue `14.1 mA` a `5 V`, equivalente a `0.070 W`.
- El peor caso medido con la alimentación de la fuente externa fue `112 mA` a `5 V`, equivalente a `0.560 W`.
- El sistema se mantiene por debajo de `1 W`, por lo que puede alimentarse sin inconvenientes con fuentes comerciales 220VAC->5 V de baja potencia.
- La diferencia entre los distintos modos de operación es baja para cualquier tipo de alimentación, lo que hace que sea un sistema consistente en su consumo.

## 4.5 Console and Build Analyzer

En las siguientes figuras se muestran el reporte de uso de memoria del build; se observa un uso bajo de RAM y FLASH (≈16,88% y ≈40,81%), dejando margen para futuras extensiones para el sistema.

<div align="center">
  <img width="1125" height="689" alt="Rainbird" src="https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/IMAGENES/console.jpg" />
<br>
<em>Figura 4.8: Salida del comando arm-none-eabi-size tras la compilación.</em>
  </div>
<br>

<img width="1125" height="689" alt="image" src="https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/IMAGENES/build%20analizer.jpg" />
<p align="center"><em>Figura 4.9: Desglose de uso de memoria RAM y FLASH (Build Analyzer).</em></p>

## 4.6 Medición y análisis de WCET por tarea

## Método de Medición Empleado

Para evaluar el comportamiento temporal del sistema bajo la arquitectura de **Sistema Disparado por Eventos (ETS)**, se instrumentó un módulo de perfilado (*profiling*) en el despachador de tareas (*scheduler*).

El método utiliza el temporizador del sistema (`SysTick`) para capturar marcas de tiempo justo antes y después de invocar a cada función `.task_update()`. La estructura registra los siguientes parámetros para cada tarea:

* **`NOE` (Number of Executions):** Cantidad total de iteraciones completadas por la tarea.
* **`LET` (Last Execution Time):** Duración de la última ejecución en microsegundos ($\mu\text{s}$).
* **`BCET` (Best-Case Execution Time):** Menor tiempo de ejecución registrado ($\mu\text{s}$).
* **`WCET` (Worst-Case Execution Time):** Mayor tiempo de ejecución registrado ($\mu\text{s}$).



<div align="center">
  
| Índice | Módulo / Tarea | NOE | BCET ($\mu\text{s}$) | LET ($\mu\text{s}$) | **WCET ($\mu\text{s}$)** | Estado |
| :---: | :--- | :---: | :---: | :---: | :---: | :--- |
| `[0]` | **Task Sensor** | 1.932.164 | 190 | 190 | 26.192 | Regular |
| `[1]` | **Task System** | 1.932.164 | 21 | 34 | **1.058.301** | **Crítico** |
| `[2]` | **Task Actuator** | 1.932.164 | 62 | 62 | 142 | Excelente |
| `[3]` | **Task Display** | 1.932.164 | 13 | 13 | 287 | Excelente |
| `[4]` | **Task Bluetooth** | 1.932.164 | 9 | 9 | **52.530** | Bloqueante |

</div>

<p align="center"><em>Tabla 4.6: Métricas de desempeño y tiempos de ejecución del scheduler.</em></p>


## 4.7 Cálculo del factor de uso de CPU (U)

El **Factor de Uso ($U$)** representa la fracción de capacidad de la CPU consumida por las tareas dentro del período del ciclo de ejecución ($T = 1000\ \mu\text{s} = 1\text{ ms}$). La ecuación general es:

$$U = \sum_{i=0}^{N-1} \frac{C_i}{T_i} = \frac{\sum_{i=0}^{4} C_i}{1000\ \mu\text{s}}$$

### 1. Factor de Uso Nominal (Régimen Permanente / LET)

Tomando los tiempos típicos de ejecución ($LET$):

$$\sum C_{nominal} = 190 + 34 + 62 + 13 + 9 = 308\ \mu\text{s}$$

$$U_{nominal} = \frac{308\ \mu\text{s}}{1000\ \mu\text{s}} = 0{,}308 \implies \mathbf{30{,}8\%}$$

> **Interpretación:** En operación normal, el microcontrolador opera holgadamente con un **$69{,}2\%$ de tiempo ocioso (Idle)**.

### 2. Factor de Uso en el Peor Caso Registrado (Pico / WCET)

Tomando la suma de los máximos absolutos ($WCET$):

$$\sum WCET = 26.192 + 1.058.301 + 142 + 287 + 52.530 = 1.137.452\ \mu\text{s}$$

$$U_{peor\_caso} = \frac{1.137.452\ \mu\text{s}}{1000\ \mu\text{s}} = 1137{,}45 \implies \mathbf{1137{,}45\%}$$

> **Interpretación:** Un $U > 100\%$ demuestra que el sistema sufrió una pérdida total del determinismo temporal durante ese instante.

Apartir de lo calculado se puede realizar el siguiente análisis:

1. **Alta eficiencia nominal:** Durante la mayor parte del tiempo, la arquitectura ETS mantiene una baja carga de trabajo sobre el microcontrolador ($30{,}8\%$), lo que confirma el buen diseño general de los módulos de periféricos (Display y Actuadores).
2. **Bloqueo crítico en `Task System`:** El WCET registrado de **$1{,}058\text{ s}$** quiebra las garantías de tiempo real. Este fallo ocurre por el uso de llamadas bloqueantes del tipo `HAL_Delay()` o esperas síncronas de bus I2C/EEPROM dentro de la tarea.
3. **Inercia en Comunicaciones UART:** La tarea Bluetooth registra un pico de **$52{,}53\text{ ms}$** al responder mensajes, producto de la transmisión por polling en `HAL_UART_Transmit`.


## 4.8 Gestión de bajo consumo y justificación
En la presente iteración del prototipo, la estrategia de ahorro energético se centró en la optimización de los periféricos externos, dado que representan el mayor consumo frente al microcontrolador:

- Se utilizó un módulo Bluetooth HM-10 (BLE - Bluetooth Low Energy), configurado para despertar y procesar datos asincrónicamente.
- Implementación de una pantalla de PAUSA, para poder pausar el sistema y observar o configurar las condiciones óptimas que tenga que tener el sistema en el preciso momento que se quiera.

No se implementó una entrada explícita a modos Sleep/Stop en el microcontrolador, ya que la arquitectura bare-metal mantiene un polling constante de variables críticas (por ejemplo, la seguridad electromecánica de los relés y el nivel de agua). En una futura revisión alimentada exclusivamente por baterías o algún panel solar, se podría integrar el modo Sleep durante los intervalos del ciclo de 1 ms.

## 4.9 Cumplimiento de requisitos
En la siguiente sección demostramos todas las implementaciones que habíamos previsto en un informe anterior, los cuáles en partes generales pudimos concretar todos tal como se describieron o sino con modificaciones mínimas para hacer un sistema más llevadero. 

| Estado | Descripción      |
|-----|---------------------|
| 🟢 | Se implementó |
| 🔴 | No se implementó |

#### **Sensores**  

| ID | Descripción | Estado |
| :---- | :---- | :---- |
| 1.1 | **Nivel de agua (Simulado):** Lectura analógica por ADC (polling/DMA) de un potenciómetro de 1k ohm <br> Se logra implementar mediante el uso de las funciones ADC del microcontrolador la lectura de un potenciometro que en nuestro caso imitaría el nivel del agua que contiene la Estación Hidropónica. | 🟢  |
| 1.2 | **Clima (Real):** Lectura de temperatura y humedad ambiente utilizando el sensor digital AHT20 a través del bus I2C. | 🟢 |
| 1.3 | **Interfaz de entrada local:** Lectura no bloqueante (con rutinas de anti-rebote por software) de botones para la navegación del menú y botones para alternar los modos del sistema (`NORMAL` / `SET_UP`). <br> Logramos concretar una interfaz en la cual podemos navegar mediante el uso de 4 botones de una matriz de botones, de esta manera tenemos, un boton para cambiar el modo de setup a configuración, luego dos de navegación que serán también los que incrementen y decrementen los valores, y por último uno de enter/guardado. | 🟢 | 
<p align="center"><em>Tabla 4.7: Tabla de implementaciones: Sensores.</em></p>

#### **Actuadores**  

| ID | Descripción | Estado |
| :---- | :-------- | :------ |
| 2.1 | **Actuadores de simulación (Bombas/Ventilación):** Encendido y apagado de LEDs/Relés gestionados por retardos no bloqueantes basados en el tick del Systick (1ms). | 🟢 |
| 2.2 | **Alarmas sonoras:** Emisión de alertas mediante un Buzzer (accionado por PWM o GPIO) ante fallas críticas del sistema o confirmaciones de guardado. | 🟢 |
<p align="center"><em>Tabla 4.8: Tabla de implementaciones: Actuadores.</em></p>

#### **Interfaz y comunicaciones**  

| ID | Descripción | Estado |
| :---- | :-------- | :------ |
| 3.1 | **Interfaz Visual Local (Display):** Actualización periódica de un menú interactivo en una pantalla LCD 16x2 para la visualización y configuración de parámetros sin depender de la red. <br> Se desarrolló una interfaz visual que contiene toda la información necesaria para poder utilizar el sistema completo, y además nos comunica las fallas que se produzcan  | 🟢 |
| 3.2 | **Telemetría remota:** Envío de tramas de estado y alertas por UART utilizando un módulo Bluetooth HM-10 para monitoreo desde una aplicación móvil. | 🟢 |
<p align="center"><em>Tabla 4.9: Tabla de implementaciones: Interfaz y comunicaciones.</em></p>

#### **Almacenamiento**  

| ID | Descripción | Estado |
| :---- | :-------- | :------ |
| 4.1 | **Gestión de Recetas (EEPROM):** Lectura y escritura de parámetros de configuración (tiempos de riego, umbrales térmicos) en una memoria EEPROM externa vía I2C para garantizar la persistencia ante cortes de energía. | 🟢  |
<p align="center"><em>Tabla 4.10: Tabla de implementaciones: Almacenamiento.</em></p>

## 4.10 Comparación con sistemas similares

## 4.11 Documentación del desarrollo realizado
El desarrollo técnico se encuentra plenamente documentado en el repositorio, incluyendo:
* Código fuente en C
  * task_sensor.c / .h: Filtros anti-rebote y lectura mixta (ADC/GPIO).
  * task_system.c / .h: Máquina de estados principal y orquestación.
  * task_actuator.c / .h: Manejo de optoacopladores y alarmas.
  * aht20.c / .h: Driver I2C asincrónico para sensor ambiental.
  * eeprom.c / .h: Driver I2C para escritura/lectura no volátil.
  * bluetooth.c / .h: Interrupciones UART y procesamiento de tramas.
* Documentación de hardware: Esquemáticos de conexión entre placa STM32 NUCLEO F103RB y los periféricos necesarios de potencia.

* [Link a la carpeta de códigos](https://github.com/franavin/tdse-tf_2026-1erC_3-1/tree/main/Memoria%20T%C3%A9cnica/CODIGO_tdse_tf_estacion-hidroponica_V3.0)
---

# Capítulo 5: Conclusiones

## 5.1 Resultados obtenidos


## 5.2 Lecciones aprendidas


## 5.3 Próximos pasos



---

# Capítulo 6: Uso de herramientas de IA

Se documenta el uso de IA según requerimiento docente y archivo [`listado de cosas hechas con IA.txt`](https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/listado%20de%20cosas%20hechas%20con%20IA.txt).
## 6.1 Uso individual y conjunto

- Franco:
  - Consultas sobre la estructuración para el código y las funciones específicas necesarias para concretar el código (ej. archivos de memoria eeprom o bluetooth).
  - Consultas por errores y warnings que aparecieron en el código.
  - Consultas sobre comportamientos no deseados a la hora de tener el sistema ejecutándose, tanto en hardware como firmware.
  - Utilización de aplicación Mermaid para realizar los diagramas de secuencia.

- Leonardo:
  - Consulta de GFM (GitHub Flavored Markdown).
  - Le pasamos 2 memorias técnicas de las de ejemplo y le pedimos que extraiga el esqueleto de secciones y subsecciones para poder empezar a completar nuestro documento.
  - Consulta sobre componentes y conexiones.
  - Checkeo de redacción de la memoria técnica.
  - Consuta sobre funciones del LT Spice para dibujar el esqumático.

- Uso común del equipo:
  - apoyo en redacción y ajuste de memoria técnica.
  - apoyo extensivo en programación STM32 (estructura, módulos y ajustes).
  

---

# Capítulo 7: Bibliografía y referencias

1. [STMicroelectronics, UM1724 - User manual - STM32 Nucleo-64 boards](https://www.st.com/resource/en/user_manual/um1724-stm32-nucleo64-boards-mb1136-stmicroelectronics.pdf)
2. [STMicroelectronics, MB1136 - Electrical Schematic - STM32 Nucleo-64 boards](https://www.st.com/resource/en/schematic_pack/mb1136-default-c04_schematic.pdf)
3. [STMicroelectronics, NUCLEO-F103RB Datasheet](https://os.mbed.com/platforms/ST-Nucleo-F103RB/#:~:text=Arduino%2Dcompatible%20headers)
4. [Datasheet AHT20 - Módulo Sensor de Temperatura y Humedad](https://files.seeedstudio.com/wiki/Grove-AHT20_I2C_Industrial_Grade_Temperature_and_Humidity_Sensor/AHT20-datasheet-2020-4-16.pdf)
5. [Datasheet HM-10 - Módulo Bluetooth](https://people.ece.cornell.edu/land/courses/ece4760/PIC32/uart/HM10/DSD%20TECH%20HM-10%20datasheet.pdf)
6. [Datasheet Display LCD 1602A](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/5773/CN0295D%20other%20related%20document.pdf)
7. [Datasheet Módulo de Relés de 4 Canales](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/5773/TS0011%20DATASHEET.pdf)}
8. [Datasheet Memoria EEPROM](https://ww1.microchip.com/downloads/en/devicedoc/atmel-8787-seeprom-at24c04c-08c-datasheet.pdf)
9. [Trabajo usado como referencia Sistema de gestión de órganos de tubos con microcontroladores](https://github.com/mpdcfiuba/tdse-tf_3-4/blob/main/Readme.md)
10.  [Trabajo usado como referencia Dimmer + Switch (Ventilador & Luces)](https://github.com/Embebidos-Fran-Marcos-Nacho/tdse-tf_1-2/blob/Memoria-final-y-video/Memoria%20t%C3%A9cnica/Memoria%20t%C3%A9cnica.md)


Referencias internas del repositorio:

---

**Fin de la Memoria Técnica**  
Autores: Avincetto, Franco Joaquín, Nishihara, Leonardo
Fecha de edición: 
