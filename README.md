# Mochila Inteligente (Concurso ONCE) 🎒👁️

Proyecto de prototipo de una "mochila escolar" inteligente diseñada para asistir a niños con ceguera o visión reducida. 

Este repositorio contiene los códigos fuente para el microcontrolador de la mochila, así como diversas pruebas individuales para cada sensor y módulo utilizado en el proyecto.

## 🌟 Funciones Principales

1. **Gestión de material escolar:** Identifica mediante tecnología RFID qué libros y materiales se introducen en la mochila para asegurar que el niño lleva lo necesario según el horario escolar.
2. **Detección de obstáculos:** Ayuda en el trayecto al colegio detectando obstáculos a los lados y al frente (mediante sensores láser ToF y ultrasonidos), proporcionando avisos hápticos (vibración) y sonoros al usuario.

## 🧰 Arquitectura y Hardware

El "cerebro" del proyecto es una placa **Wemos D1 R32** (basada en el chip ESP32), elegida por su compatibilidad con el formato Arduino Uno y sus capacidades integradas de Wi-Fi y Bluetooth BLE.

### Componentes Utilizados:
* **Microcontrolador:** Wemos D1 R32 (ESP32)
* **Sensores de Entorno:** 
  * 2x **VL53L0X** (Sensores láser Time-of-Flight) para detección de obstáculos a media distancia (~2m).
  * 1x **HC-SR04** (Sensor de ultrasonidos) para distancias críticas cortas.
* **Sensores de Identificación:** 
  * 1x **RC522 (13.56MHz)** (Lector RFID) para identificar los materiales escolares previamente etiquetados con pegatinas RFID.
* **Actuadores (Feedback):** 
  * 2x **Motores de vibración** tipo moneda (3V) para indicaciones direccionales hápticas.
  * 1x **Buzzer pasivo** para alertas sonoras y confirmaciones.
* **Alimentación:** Power Bank USB estándar.

## 📁 Estructura del Repositorio

* `archivo/`: Contiene notas, planificación y el resumen detallado del proyecto (`resumen_proyecto.md`).
* `src/`: Contiene los códigos fuente (sketches) de Arduino.
  * `mochila_once.ino`: **Script principal** que integra la lógica de todos los sensores.
  * `*_test.ino`: Diferentes scripts de prueba unitaria para calibrar y entender cada componente por separado (RFID, VL53L0X, Bluetooth, Buzzer, etc.) antes de integrarlos.

## 🛠️ Software y Herramientas

* **Entorno de Desarrollo:** Arduino IDE
* **Core:** `esp32` de Espressif Systems
* **Librerías Dependientes:**
  * `VL53L0X` de Pololu
  * `MFRC522` para el lector RFID
  * `ESP32 BLE Arduino` (incluida en el core ESP32)

## 🚀 Uso y Configuración

1. Instala el [Arduino IDE](https://www.arduino.cc/en/software).
2. Añade el soporte para placas ESP32 en el *Gestor de Tarjetas*.
3. Instala las librerías necesarias (`VL53L0X` y `MFRC522`) desde el *Gestor de Librerías*.
4. Abre cualquier sketch de la carpeta `src/`.
5. Selecciona la placa "Wemos D1 R32" o "ESP32 Dev Module" en el menú de placas.
6. Compila y sube el código a tu placa.

## 📝 Estado del Proyecto

Actualmente en fase de desarrollo como prueba de concepto (PoC) para un marco de tiempo de 2 semanas. El código modular en `src/` está pensado para aplicar el principio de "divide y vencerás", probando cada pieza de hardware de manera aislada antes de la gran integración final.
