# Termohigrometro con medida diferencial de temperatura aplicable al campo de la refrigeración

Este proyecto consiste en el desarrollo de una herramienta capaz de obtener información diferencial de la temperatura, manteniendo una temperatura y humedad de referencia. La información se muestra tanto en una pantalla integrada del dispositivo como en una computadora mediante comunicación UART.

---

## Índice

- [Introducción](#introducción)
- [Características](#características)
- [Valores de Operación](#valores-de-operación)
- [Requisitos de Hardware](#requisitos-de-hardware)
- [Instalación y Uso](#instalación-y-uso)

---

## Introducción

El dispositivo tiene como objetivo facilitar la verificación del correcto salto térmico en equipos de refrigeración y calefacción mediante bomba de calor. Para lograrlo, se miden las condiciones ambientales de temperatura y humedad del entorno usando el sensor **SHT30**. Además, se realiza una medición diferencial de temperatura utilizando dos termistores **NTC** para evaluar la variación de la temperatura del aire que circula por el radiador. 

El sistema también integra:
- Un **RTC** para registrar el momento exacto de cada medición.
- Comunicación **UART** para transmitir los datos a una computadora.
- **LEDs** que proporcionan una indicación visual rápida sobre el estado del salto térmico, facilitando su uso en campo.

El usuario podrá seleccionar el modo de funcionamiento del dispositivo mediante pulsadores.

---

## Características

- **Medición Diferencial de Temperatura:**  
  Utiliza dos termistores NTC para comparar la temperatura en la salida y toma de aire, permitiendo observar cómo varía la temperatura a lo largo del tiempo.

- **Sensor de Temperatura y Humedad:**  
  Emplea el sensor SHT30 para obtener las condiciones ambientales en tiempo real.

- **Visualización en Tiempo Real:**  
  - **Pantalla Integrada:** Muestra la información de las mediciones directamente en el dispositivo.
  - **Comunicación UART:** Transfiere los datos, junto con la hora de la medición obtenida del RTC, a una computadora.

- **Indicadores LED:**  
  Permiten una rápida visualización del estado del salto térmico, ideal para aplicaciones en campo.

- **Modos de Funcionamiento:**  
  Seleccionables mediante pulsadores, los modos son:
  - Termohigrómetro
  - Termohigrómetro modo refrigeración
  - Termohigrómetro modo calefacción

---

## Valores de Operación

Según la información proporcionada por la Cámara Argentina de Calefacción, Aire Acondicionado y Ventilación (CACAAV) y basados en los manuales de capacitación brindados por LG, los valores esperados son:

- **Modo Frío:**  
  El salto térmico debe ser mayor a **8°C**.

- **Modo Calefacción:**  
  El salto térmico debe ser mayor a **14°C**.

---

## Requisitos de Hardware

- **Sensores:**
  - Sensor **SHT30** para medir temperatura y humedad.
  - Dos termistores **NTC** para la medición diferencial de temperatura.

- **Visualización:**
  - Pantalla integrada para mostrar datos en tiempo real.
  - Módulo **UART** para la comunicación con una computadora.

- **Sincronización:**
  - **RTC** para registrar el momento exacto de cada medición.

- **Indicadores:**
  - Diodos **LED** para la rápida verificación del estado del salto térmico.

- **Interacción:**
  - **Pulsadores** para seleccionar el modo de funcionamiento deseado.

---

## Instalación y Uso

1. **Conexión del Hardware:**  
   Conecta los sensores y módulos al microcontrolador según el esquema de conexiones especificado en la documentación del proyecto.

2. **Configuración de la Comunicación:**  
   Asegúrate de configurar correctamente la comunicación UART para la transmisión de datos a la computadora.

3. **Carga del Firmware:**  
   Compila y carga el firmware disponible en este repositorio en el microcontrolador.

4. **Selección del Modo de Funcionamiento:**  
   Utiliza los pulsadores para elegir entre los modos:
   - Termohigrómetro
   - Termohigrómetro modo refrigeración
   - Termohigrómetro modo calefacción

5. **Visualización:**  
   Verifica que los datos se muestren en la pantalla integrada y se transmitan a la computadora.

---
