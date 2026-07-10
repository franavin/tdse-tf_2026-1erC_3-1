 <img src="https://www.fi.uba.ar/images/logo-fiuba.png" alt="image2" width="50%">


# **Estación Hidropónica**


**Autores: Avincetto, Franco Joaquín; Nishihara, Leonardo**

**Padrón: 106747; 88627**

**Fecha: 1er cuatrimestre 2026**

Lo primero que se desarrolló fueron los diagramas de secuencia, en este punto lo que hicimos fue volcar los sensores y actuadores y ver como queríamos que interaccionen 
con el sistema en distintos casos.

<p align="center">
  <img src="https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/Memoria%20T%C3%A9cnica/IMAGENES/diagrama_en_bloques.png" alt="image1">
</p>

<p align="center"><em>Figura 1: Diagrama en bloques del sistema</em></p>

Luego, una vez que teníamos una idea de cómo queríamos que se vaya desarrollando nuestro sistema, fuimos completando los distintos módulos, declarando así los estados, eventos y las demás variables necesarias para lograr la implementación.

[Módulo de Sensores](https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/tdse_tf_sensor.md)

[Módulo del Sistema](https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/tdse_tf_sensor.md)

[Módulo de Actuadores](https://github.com/franavin/tdse-tf_2026-1erC_3-1/blob/main/tdse_tf_actuator.md)

A continuación se detalla el informe de avances del TF a partir de los requerimientos

| Estado | Descripción      |
|-----|---------------------|
| 🟢 | Ya implementado |
| 🟡 | En proceso de implementarse |
| 🔴 | No se implementará |

#### **Sensores**  

| ID | Descripción | Estado |
| :---- | :---- | :---- |
| 1.1 | **Nivel de agua (Simulado):** Lectura analógica por ADC (polling/DMA) de un potenciómetro, previo paso por un filtro activo Sallen-Key por hardware. <br> Se logra implementar mediante el uso de las funciones ADC del microcontrolador la lectura de un potenciometro que en nuestro caso imitaría el nivel del agua que contiene la Estación Hidropónica. | 🟢  |
| 1.2 | **Clima (Real):** Lectura de temperatura y humedad ambiente utilizando el sensor digital SHT3x a través del bus I2C. | 🟡 |
| 1.3 | **Interfaz de entrada local:** Lectura no bloqueante (con rutinas de anti-rebote por software) de botones para la navegación del menú y botones para alternar los modos del sistema (`NORMAL` / `SET_UP`). <br> Logramos concretar una primera interfaz en la cual podemos navegar mediante el uso de 4 botones de una matriz de botones, de esta manera tendremos, un boton para cambiar el modo de setup a configuración, luego dos de navegación que serán también los que incrementen y decrementen los valores, y por último uno de enter/guardado. | 🟢 | 

#### **Actuadores**  

| ID | Descripción | Estado |
| :---- | :-------- | :------ |
| 2.1 | **Actuadores de simulación (Bombas/Ventilación):** Encendido y apagado de LEDs/Relés gestionados por retardos no bloqueantes basados en el tick del Systick (1ms). | 🟡 |
| 2.2 | **Alarmas sonoras:** Emisión de alertas mediante un Buzzer (accionado por PWM o GPIO) ante fallas críticas del sistema o confirmaciones de guardado. | 🟡 |

#### **Interfaz y comunicaciones**  

| ID | Descripción | Estado |
| :---- | :-------- | :------ |
| 3.1 | **Interfaz Visual Local (Display):** Actualización periódica de un menú interactivo en una pantalla OLED (SPI/I2C) para la visualización y configuración de parámetros sin depender de la red. <br> Desarrollamos una interfaz viusal básica para ver cómo queremos que se vea en un primer punto, los datos que muestre y cómo navegar por el mismo.  | 🟢 |
| 3.2 | **Telemetría remota:** Envío de tramas de estado y alertas por UART utilizando un módulo Bluetooth HM-10 para monitoreo desde una aplicación móvil. | 🟡 |

#### **Almacenamiento**  

| ID | Descripción | Estado |
| :---- | :-------- | :------ |
| 4.1 | **Gestión de Recetas (EEPROM):** Lectura y escritura de parámetros de configuración (tiempos de riego, umbrales térmicos) en una memoria EEPROM externa vía I2C para garantizar la persistencia ante cortes de energía. | 🟡  |
