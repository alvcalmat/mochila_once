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

### Sensores de Identificación (Material Escolar)
* **Lector RFID:** **RC522 (13.56MHz)**. Módulo económico para leer etiquetas RFID a corta distancia (3-5 cm). 
* **Etiquetas RFID:** Pegatinas pequeñas para adherir a cada libro y cuaderno. Se deslizarán por una zona específica de la mochila al guardarlos.

### Actuadores (Feedback al Usuario)
* **Feedback Háptico:** 2x **Módulos de motor de vibración** tipo moneda (Coin Vibration Motor, 3V). Se colocarán en las correas de los hombros para indicar de qué lado está el obstáculo.
* **Feedback Sonoro:** **Zumbador pasivo (Buzzer)**. Para emitir tonos rápidos en caso de peligro inminente o confirmación de lectura de un libro.

### Alimentación y Varios
* **Batería:** **Power Bank USB** estándar para alimentar la placa.
* **Conexiones:** Cables jumper (Macho-Hembra, Hembra-Hembra) y Breadboard (placa de pruebas) para evitar soldaduras.

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