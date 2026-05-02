# Proyecto: Mochila Inteligente (Concurso ONCE)

## Descripción General
El proyecto consiste en el prototipo de una "mochila escolar" inteligente diseñada para asistir a niños con ceguera o visión reducida. Sus dos funciones principales son:
1. **Gestión de material escolar:** Identifica qué libros y materiales se introducen en la mochila y asiste al usuario, conectándose a una App móvil que conoce el horario escolar.
2. **Detección de obstáculos:** Ayuda en el trayecto al colegio detectando obstáculos a los lados y al frente, proporcionando avisos hápticos y sonoros.

Para mantener el coste bajo y el prototipo viable en un corto plazo de tiempo (2 semanas), la carga computacional pesada (lógica del horario, base de datos) se delega a un smartphone mediante conexión Bluetooth, utilizando la mochila únicamente como un hub de sensores y actuadores.

---

## Arquitectura y Hardware

### El "Cerebro"
* **Microcontrolador:** **Wemos D1 R32**. Es una placa basada en el chip ESP32 (incluye Wi-Fi y Bluetooth BLE), pero con el tamaño y disposición de pines de un Arduino Uno. Esto facilita la conexión de cables "jumper" sin necesidad de soldar.

### Sensores de Entorno (Detección de Obstáculos)
* **Sensores Láser ToF (Time-of-Flight):** 2x **VL53L0X**. Sensores pequeños y precisos (rango útil práctico hasta ~2 metros) que irán montados uno en cada hombro/correa.
* **Sensor de Ultrasonidos:** **HC-SR04** (Opcional/Complementario según el código). Para distancias críticas muy cortas (< 30 cm).

#### Conexión detallada de los sensores láser VL53L0X (ToF) al Wemos D1 R32 (ESP32)

Ambos sensores VL53L0X comparten el bus I2C, pero cada uno tiene un pin de encendido (XSHUT) distinto para poder asignarles una dirección única al iniciar.

| Pin VL53L0X | Función                | Pin ESP32 (Wemos D1 R32) | Ubicación física (header derecho) |
|-------------|------------------------|--------------------------|-----------------------------------|
| VIN         | Alimentación 3.3V/5V   | 3V3 o 5V                 | 3V3 o 5V                          |
| GND         | Tierra                 | GND                      | GND                               |
| SDA         | Datos I2C              | GPIO21                   | SDA (GPIO21)                      |
| SCL         | Reloj I2C              | GPIO22                   | SCL (GPIO22)                      |
| XSHUT IZQ   | Encendido láser izq.   | GPIO16                   | IO16                              |
| XSHUT DER   | Encendido láser der.   | GPIO17                   | IO17                              |

**Resumen de conexiones (cable a cable):**

- VIN (de ambos sensores)  → 3V3 (o 5V)
- GND (de ambos sensores)  → GND
- SDA (de ambos sensores)  → GPIO21 (SDA)
- SCL (de ambos sensores)  → GPIO22 (SCL)
- XSHUT del láser izquierdo → IO16
- XSHUT del láser derecho  → IO17

**Notas:**
- Los pines SDA y SCL se comparten entre ambos sensores (bus I2C).
- Cada sensor debe tener su propio cable XSHUT para poder inicializarlos con direcciones distintas (ver código).
- Puedes alimentar los VL53L0X con 3.3V o 5V, pero si usas el pin VIN, mejor 3.3V para evitar problemas de compatibilidad.

Esta asignación coincide con el código y el pinout físico de la placa, facilitando el montaje con cables jumper.

### Sensores de Identificación (Material Escolar)
* **Lector RFID:** **RC522 (13.56MHz)**. Módulo económico para leer etiquetas RFID a corta distancia (3-5 cm). 
* **Etiquetas RFID:** Pegatinas pequeñas para adherir a cada libro y cuaderno. Se deslizarán por una zona específica de la mochila al guardarlos.

#### Conexión detallada del lector RC522 (RFID) al Wemos D1 R32 (ESP32)

El módulo RC522 tiene 8 pines. Así debes conectarlos:

| Pin RC522 | Función                | Pin ESP32 (Wemos D1 R32) | Ubicación física (header derecho) |
|-----------|------------------------|--------------------------|-----------------------------------|
| SDA (SS)  | Slave Select (chip select) | GPIO5                   | IO5 (VSPI_SS)                     |
| SCK       | SPI Clock              | GPIO18                   | IO18 (VSPI_SCK)                   |
| MOSI      | SPI Master Out Slave In| GPIO23                   | IO23 (VSPI_MOSI)                  |
| MISO      | SPI Master In Slave Out| GPIO19                   | IO19 (VSPI_MISO)                  |
| IRQ       | Interrupción (no se usa)| —                        | —                                 |
| GND       | Tierra                 | GND                      | GND                               |
| RST       | Reset                  | GPIO14                   | IO14                              |
| 3.3V      | Alimentación           | 3V3                      | 3V3                               |

**Resumen de conexiones (cable a cable):**

- SDA (SS)  → IO5
- SCK       → IO18
- MOSI      → IO23
- MISO      → IO19
- RST       → IO14
- 3.3V      → 3V3
- GND       → GND
- IRQ       → (no conectar)

**Notas:**
- El pin IRQ del RC522 no se utiliza, déjalo sin conectar.
- Usa siempre 3.3V para alimentar el RC522 (no 5V).

Esta asignación coincide con el código y el pinout físico de la placa, facilitando el montaje con cables jumper.

### Actuadores (Feedback al Usuario)
* **Feedback Háptico:** 2x **Módulos de motor de vibración** tipo moneda (Coin Vibration Motor, 3V). Se colocarán en las correas de los hombros para indicar de qué lado está el obstáculo.
* **Feedback Sonoro:** **Zumbador pasivo (Buzzer)**. Para emitir tonos rápidos en caso de peligro inminente o confirmación de lectura de un libro.

### Alimentación y Varios
* **Batería:** **Power Bank USB** estándar para alimentar la placa.
* **Conexiones:** Cables jumper (Macho-Hembra, Hembra-Hembra) y Breadboard (placa de pruebas) para evitar soldaduras.

#### Conexión de los botones (modo "Meter Libros" y "Camino al Colegio")

Los botones se conectan así:

| Botón                | Pin ESP32 (Wemos D1 R32) | Otro extremo del botón |
|----------------------|--------------------------|-----------------------|
| "Meter Libros"       | GPIO32                   | GND                   |
| "Camino al Colegio"  | GPIO33                   | GND                   |

**Notas:**
- El código configura ambos pines como `INPUT_PULLUP`, por lo que **NO necesitas resistencia externa**.
- Un extremo del botón va al pin correspondiente (32 o 33) y el otro extremo a **GND**.
- Cuando el botón está pulsado, el pin se conecta a GND y lee `LOW` (0).
- Cuando el botón está suelto, el pin se mantiene en `HIGH` (1) gracias a la resistencia interna del ESP32.

Este esquema es seguro y sencillo para prototipos y coincide con la configuración del código.

#### Funcionamiento sin botones (modo demo)

Si se prueban sin botones conectados, el comportamiento es este:

- Al encender, la mochila entra directamente en **modo Material**.
- Cada vez que se lee un material correcto, suena un **pitido corto de éxito**.
- Si se lee un material que no toca, suena un **pitido grave de error**.
- Cuando ya están todos los materiales, suena un **pitido prolongado de éxito** y se pasa a **modo Navegación**.
- Al terminar la navegación, se pasa a **modo Espera**.

**Significado de los pitidos en modo Material:**

- **Pitido corto agudo (éxito):** material correcto.
- **Pitido grave (error):** material incorrecto.
- **Pitido prolongado (éxito final):** todos los materiales correctos y se inicia la navegación.

---

## Lista de Compra (Estimada)

| Componente | Cantidad | Coste Est. |
| :--- | :--- | :--- |
| Placa Wemos D1 R32 | 1 | ~12 € |
| Sensor VL53L0X (ToF) | 2 | ~10 € |
| Lector RFID RC522 | 1 | ~6 € |
| Etiquetas RFID (Pegatinas) | 20 | ~8 € |
| Módulos de Motores de Vibración | 2 | ~5 € |
| Kit cables Jumper y Breadboard | 1 | ~13 € |
| Batería Power Bank | 1 | (Reutilizada) |

**Total:** ~50 - 60 €

---

## Software y Herramientas

* **Entorno de Desarrollo:** **Arduino IDE** (Configurado con el "Gestor de Tarjetas" para soportar el core `esp32` de Espressif).
* **Librerías Principales:**
  * `VL53L0X` de Pololu (Para los sensores láser).
  * `MFRC522` (Para el lector de etiquetas RFID).
  * `ESP32 BLE Arduino` (Opcional, para la comunicación con la App del móvil).

---

## Plan de Implementación (Hoja de Ruta)

Dada la restricción de tiempo y nivel de experiencia (2 semanas, 2h/día, perfil sin experiencia previa en programación):

1. **Configuración del Entorno:**
   * Instalación de Arduino IDE, configuración de la URL de placa ESP32 e instalación de librerías.
   * Primer test: "Hola Mundo" (imprimir por puerto serie al conectar el Wemos D1 R32).
2. **Pruebas Individuales (Divide y Vencerás):**
   * **Módulo RFID:** Conectar los 7 pines (SPI) y leer el código UID (Identificador) de una pegatina en el Monitor Serie.
   * **Sensores ToF:** Conectar por I2C (usando el pin XSHUT para cambiar direcciones si se usan varios) y leer la distancia en milímetros.
   * **Actuadores:** Hacer vibrar los motores o sonar el zumbador con señales `HIGH`/`LOW`.
3. **Integración:**
   * Unir el código de todos los sensores en un solo script principal (como la base en `mochila_once.ino`).
   * Añadir lógica de Bluetooth para enviar la información recopilada al teléfono móvil.
4. **App Móvil (Opcional/Futuro):**
   * Desarrollar una App sencilla (por ejemplo, con MIT App Inventor) que reciba datos Bluetooth y use Text-to-Speech o lógica para verificar la mochila contra el horario del colegio.
5. **Montaje Físico y Pruebas Reales:**
   * Fijación firme pero temporal en la mochila con bridas o cinta de doble cara y calibración en movimiento.

## Casos de Uso Predefinidos (Sin App Móvil)

En caso de que no haya tiempo para implementar la App móvil, la lógica puede programarse "a fuego" (hardcoded) en el ESP32 para el día de la demostración. Aquí tienes dos ejemplos de cómo funcionaría:

### 1. Gestión de Material Escolar ("Horario de Hoy")
Se guardan previamente en el código del ESP32 los identificadores (UIDs) de 3 etiquetas RFID que representan el material necesario para un "Lunes":
1. **Libro de Matemáticas** (ej. UID: `0x12 0x34 0x56 0x78`)
2. **Cuaderno de Lengua** (ej. UID: `0xBA 0x98 0x76 0x54`)
3. **Estuche** (ej. UID: `0xAA 0xBB 0xCC 0xDD`)

* **Funcionamiento:** La mochila arranca con un contador interno de los materiales esperados. Cuando el niño desliza una etiqueta, la mochila la compara con su lista interna.
* **Feedback de Acierto:** Si es un material de la lista, el zumbador emite un pitido corto y agudo, marcándolo como guardado. Si no es de la lista, emite un pitido largo y grave de "error".
* **Feedback de Éxito Final:** Una vez leídas las 3 etiquetas, emite una melodía alegre (varios tonos ascendentes) indicando que la mochila tiene todo lo necesario de hoy.

### 2. Navegación Asistida ("Camino al Colegio")
En modo ruta, la mochila simula que está dando instrucciones GPS con retardos de tiempo preprogramados, mientras simultáneamente los sensores detectan obstáculos introducidos en tiempo real (por ejemplo, personas cruzándose en el escenario).

* **Trayecto Predefinido Simulado (Ejemplo de demo):**
  1. *[Segundos 0-10]*: **Avanzar recto**. (Estado normal sin avisos de navegación).
  2. *[Segundo 10]*: **Girar a la derecha**. (Doble vibración en motor derecho y pitido corto agudo).
  3. *[Segundos 10-15]*: **Avanzar recto**.
  4. *[Segundo 15]*: **Girar a la izquierda**. (Doble vibración en motor izquierdo y pitido grave consecutivo).
  5. *[Segundo 20]*: **Llegada al destino**. (Melodía alegre de fin de trayecto).

* **Interacción con Obstáculos Reales (Durante el trayecto):**
  Sin importar la instrucción actual de navegación, los sensores siempre están activos:
  * Si alguien se acerca desde la izquierda (< 60cm), el hombro izquierdo oscila suavemente.
  * Si alguien frena bruscamente delante (obstáculo crítico), vibran ambos de golpe indicando la alerta física de colisión que "pisa" a las instrucciones de la ruta.

## Descripción de los pines de la placa Wemos D1 R32 ESP32

Aquí tienes el archivo JSON con la descripción detallada de los pines de la placa Wemos D1 R32 ESP32, organizado por su ubicación física según la imagen:

```json
{
    "board": "Wemos D1 R32 ESP32",
    "pin_layout": {
        "left_side_power": [
            {"label": "IO0", "function": "GPIO0"},
            {"label": "5V", "function": "Power 5V"},
            {"label": "RST", "function": "Reset"},
            {"label": "3V3", "function": "Power 3.3V"},
            {"label": "5V", "function": "Power 5V"},
            {"label": "GND", "function": "Ground"},
            {"label": "GND", "function": "Ground"},
            {"label": "VN", "function": "Sensor VP (GPIO36)"}
        ],
        "analog_header_left": [
            {"label": "IO2", "functions": ["GPIO2", "ADC12", "TOUCH2"]},
            {"label": "IO4", "functions": ["GPIO4", "ADC10", "TOUCH0"]},
            {"label": "IO35", "functions": ["GPIO35", "ADC7"]},
            {"label": "IO34", "functions": ["GPIO34", "ADC6"]},
            {"label": "IO36", "functions": ["GPIO36", "ADC0"]},
            {"label": "IO39", "functions": ["GPIO39", "ADC3"]}
        ],
        "right_side_top": [
            {"label": "SCL", "function": "GPIO22 / SCL"},
            {"label": "SDA", "function": "GPIO21 / SDA"},
            {"label": "RST", "function": "Reset"},
            {"label": "GND", "function": "Ground"}
        ],
        "digital_header_right": [
            {"label": "IO18", "functions": ["GPIO18", "VSPI_SCK"]},
            {"label": "IO19", "functions": ["GPIO19", "VSPI_MISO"]},
            {"label": "IO23", "functions": ["GPIO23", "VSPI_MOSI"]},
            {"label": "IO5", "functions": ["GPIO5", "VSPI_SS"]},
            {"label": "IO13", "functions": ["GPIO13", "ADC14", "TOUCH4"]},
            {"label": "IO12", "functions": ["GPIO12", "ADC15", "TOUCH5"]},
            {"label": "IO14", "functions": ["GPIO14", "ADC16", "TOUCH6"]},
            {"label": "IO27", "functions": ["GPIO27", "ADC17", "TOUCH7"]},
            {"label": "IO16", "functions": ["GPIO16", "RX2"]},
            {"label": "IO17", "functions": ["GPIO17", "TX2"]},
            {"label": "IO25", "functions": ["GPIO25", "ADC18", "DAC1"]},
            {"label": "IO26", "functions": ["GPIO26", "ADC19", "DAC2"]},
            {"label": "TX0", "functions": ["GPIO1", "TX0"]},
            {"label": "RX0", "functions": ["GPIO3", "RX0"]}
        ]
    }
}
```