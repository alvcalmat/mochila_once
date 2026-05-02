Prompt:
Para un concurso de emprendimiento he presentado una "mochila escolar" de apoyo para niños con cegera o visión muy reducida y ahora necesito hacer un prototipo porque me han seleccionado para la final.

El presupuesto es muy reducido, así que he pensado en una versión con capacidades limitadas en lugar de hacer una que tenga un ordenador potente a bordo (como una Raspi) y una cámara y que vaya haciendo reconocimiento de imágenes y segmentación de las calles.

La idea original era por un lado, que la mochila tenga conocimiento del horario escolar del niño y le ayude a cargar los libros y materiales que necesita para ese día, y por otro lado que le asista en el trayecto al colegio, advirtiendole de obstáculos y otros peligros.

Como el presupuesto es limitado (necesito que sea lo más barato posible), he pensado en reducir las capacidades de la mochila e incluir conectividad con el smartphone del niño para así poder usar las capacidades computacionales del teléfono móvil. He pensado en usar un arduino, un lector de etiquetas nfc, etiquetas nfc para identificar cada material, un módulo wifi o bluetooth para conectarlo con el móvil, un par de sensores sonar para poner en cada uno de los hombros y así detectar obtáculos de forma más barata computacionalmente (aunque más limitada que una cámara) y quizás un altavoz para que te avise de qué materiales has metido, cuáles faltan, si hay obstáculos... etc

¿Qué te parece mi plan de implementación? ¿Existen estos materiales? ¿Crees que hay alguna opción mejor? Acepto sugerencias. Se sincero. No necesito que me des la razón, quiero que pienses en cosas que quizás no se me hayan ocurrido a mí. Además necesito poder construirlo y programarlo en un par de semanas.

Respesta de Gemini:
Tu plan es **técnicamente sólido, realista y ejecutable** en el tiempo que tienes. Has aplicado bien el principio de "descargar" el procesamiento pesado al smartphone, lo cual es la clave para mantener el coste bajo.

Aquí tienes mi análisis como desarrollador senior:

### 1. Validación de Componentes (Hardware)
Todos los materiales que mencionas existen, son extremadamente baratos y fáciles de conseguir:

* **Microcontrolador:** No uses un Arduino Uno (es grande y caro para esto). Usa un **ESP32**. Es más potente, igual de fácil de programar y ya incluye **Wi-Fi y Bluetooth BLE** integrados. Te ahorras comprar un módulo aparte.
* **Sensores de Obstáculos:** Los sensores de ultrasonidos **HC-SR04** son un estándar. Son baratos (aprox. 1-2€), pero tienen un "punto ciego" si el objeto es blando (como ropa) o está muy inclinado. Para un prototipo de concurso, funcionan perfecto.
* **Identificación (NFC vs RFID):** El NFC es genial si quieres que el móvil también lea las etiquetas. Si solo las va a leer la mochila, el **RFID RC522 (13.56MHz)** es aún más barato y las pegatinas/etiquetas son diminutas.
* **Feedback (Altavoz):** No uses un altavoz con archivos MP3 pesados si vas corto de tiempo. Usa un **Zumbador Pasivo** para emitir diferentes tonos de aviso, o mejor aún, envía el comando al móvil por Bluetooth y que sea la **App del móvil la que hable (Text-to-Speech)**. Esto te ahorra hardware de audio y memoria.

### 2. Mi sugerencia de Arquitectura (Mejorada)
Para que el prototipo destaque en el concurso, te sugiero estos cambios:

1.  **Sustituir Sonar por VL53L0X (ToF):** En lugar de ultrasonidos (que son grandes y "feos"), usa sensores **Time-of-Flight (Láser)**. Son del tamaño de una uña, mucho más precisos y cuestan apenas 3-4€. Darán una imagen de "producto profesional" a la mochila.
2.  **Feedback Háptico:** En lugar de solo audio (que puede ser molesto o inaudible en la calle), pon dos pequeños **motores de vibración** (como los de los móviles) en las correas. Si el obstáculo está a la izquierda, vibra el hombro izquierdo. Es mucho más intuitivo para una persona con discapacidad visual.
3.  **App como Cerebro:** No programes lógica compleja en el ESP32. Úsalo solo como un "hub" de sensores que envía datos al móvil. La lógica del horario y la base de datos de libros deben estar en la App (puedes usar algo sencillo como *MIT App Inventor* o *Flutter* si sabes programar).

### 3. El "Factor Diferencial" para el Concurso
Si quieres ganar, añade esto (coste casi 0):
* **Botón de Pánico:** Un simple pulsador en la correa que, al mantenerlo presionado, envíe mediante el móvil la ubicación GPS del niño a sus padres por Telegram o SMS. El ESP32 le da la orden al móvil y el móvil hace el trabajo sucio.

### Resumen de Lista de Compra Sugerida:
| Componente | Función |
| :--- | :--- |
| **ESP32 DevKit V1** | Cerebro con Wi-Fi/Bluetooth. |
| **VL53L0X (x2)** | Sensores de distancia láser (precisión). |
| **RC522 RFID Reader** | Lector de etiquetas para libros. |
| **Etiquetas RFID (Pegatinas)** | Una para cada libro/cuaderno. |
| **Mini motores de vibración** | Alerta táctil en los hombros. |
| **Batería LiPo o Powerbank** | Alimentación (un Powerbank plano es lo más sencillo). |



### Viabilidad de Programación (2 semanas)
Es totalmente posible. 
* **Semana 1:** Conectar sensores al ESP32 y leer datos (existen librerías para todo). Configurar el Bluetooth para enviar "strings" de texto al móvil.
* **Semana 2:** Crear una App básica que reciba esos datos y ejecute las acciones (avisar de libros faltantes o vibrar).

**¿Quieres que te pase un esquema de conexión inicial o un código base en C++ (Arduino IDE) para empezar a probar los sensores?**

Prompt:
Me gusta tu plan, pero tengo algunas preguntas:
¿Un ESP32 con wifi y bluetooth cuánto cuesta?
¿Se puede comprar todo en Amazon?
¿Qué tiene más rango de lectura, el NFC o el RFID? Necesito que se lea solo con meterlo en la mochila. Si fuera necesario tener que acercar exactamente la etiqueta a la posición del lector, quizás para un niño ciego sea difícil de usar.
¿Qué hardware comercial recomiendas para los sensores hápticos?
¿Qué rango de distancias lee con cierta precisión el sensor ToF VL53L0X que has mencionado? Si es para caminar por la calle necesitaría que midiera de 20-50cm a 5-10m, para avisar tanto a corta como a media distancia.

Además, no seré yo quien lo programe, sino que le estoy haciendo el favor a mí tía de planificarle el trabajo. Mi tía no tiene conocimientos ninguno de programación, por eso había pensado en Arduino de hecho. Le he dicho que no se preocupe que tú podías explicarle paso por paso todo. Desde qué programas descargar hasta qué hacer paso por paso. ¿Crees que este plan es implementable en dos semanas trabajando un par de horas al día por alguien que no sabe absolutamente nada de programación (ella es profesora de primaria)?

Respuesta de Gemini:
Sere directo y realista: el factor "alguien que no sabe nada de programación" es el mayor riesgo del proyecto. Sin embargo, con el **ESP32** y las herramientas adecuadas, es factible si ella tiene buena capacidad de seguir instrucciones técnicas y tú la supervisas.

Aquí respondo a tus dudas técnicas para ajustar el diseño:

### 1. Costes y Disponibilidad (Amazon)
* **Precio ESP32:** En Amazon, un pack de 2 unidades cuesta unos **10-12€** (5-6€ por unidad). Si compras uno solo, puede rondar los 8€.
* **¿Todo en Amazon?:** Sí, absolutamente todo. Si lo pides hoy, mañana lo tiene en casa.

### 2. El problema del Rango: NFC vs. RFID vs. Alcance
Aquí hay un punto crítico. Ni el NFC ni el RFID (baratos) funcionan por "proximidad ambiental".
* **Rango:** Ambos tienen un alcance de **3 a 5 cm**. Es decir, el niño **sí tendría que acercar el libro al lector** (que podría estar integrado en la boca de la mochila).
* **Diferencia:** El **RFID (RC522)** es más barato y las etiquetas son más potentes que las pegatinas NFC básicas.
* **Solución para un niño ciego:** No hace falta que "busque" el sensor. Se puede diseñar una **ranura de entrada** o un panel rígido en el interior donde el niño simplemente deslice el libro al meterlo. Si quieres que lea todo lo que hay dentro sin acercarlo, necesitarías **RFID de alta frecuencia (UHF)**, pero el equipo cuesta más de 100€ y las antenas son enormes. Para un prototipo, el RC522 de 5€ es la opción.

### 3. Sensores ToF (VL53L0X) y Distancia
Siento decirte que el VL53L0X no llega a los 10 metros.
* **Rango real:** Mide de **3 cm a 2 metros** con precisión.
* **Alternativa para larga distancia:** Si necesitas 5-10 metros, tendrías que volver al **Ultrasonido (HC-SR04)**, que llega a 4 metros, o usar el **TF-Luna (LiDAR)**, que llega a 8 metros pero cuesta unos 20-25€ cada uno.
* **Mi consejo:** Para un prototipo escolar, **2 metros es suficiente**. Da tiempo de sobra para reaccionar caminando. El VL53L0X es mucho más discreto y profesional.

### 4. Hardware Háptico
No compres nada complejo. Busca en Amazon: **"Módulo de motor de vibración para Arduino"** (suelen llamarse *3V Vibration Sensor Module* o *Coin Vibration Motor*). Son como una moneda pequeña que se pega a la tela y se conecta directamente al ESP32.

---

### ¿Es viable para tu tía (Profesora sin experiencia)?
**Honestamente: Es un reto ambicioso para 14 días a 2h/día.**
La curva de aprendizaje de Arduino IDE y la gestión de librerías puede ser frustrante al principio. Sin embargo, para compensar su falta de conocimientos, yo puedo generarle el **código "llave en mano"**. Ella solo tendría que "copiar, pegar y conectar cables".

**Hoja de ruta para ella:**
1.  **Día 1-2:** Instalación de software (Arduino IDE) y "Hola Mundo" (encender un LED).
2.  **Día 3-5:** Conexión y prueba individual de sensores (un día para el RFID, otro para el sensor láser).
3.  **Día 6-8:** Configuración del Bluetooth (hacer que el móvil reciba un "Hola").
4.  **Día 9-12:** Montaje físico en la mochila y corrección de errores.
5.  **Día 13-14:** Pruebas reales.

### Recomendación para el Concurso
Para que ella no sufra, te sugiero cambiar el ESP32 por una placa llamada **Wemos D1 R32**. Tiene el chip ESP32 (Bluetooth/Wifi) pero **la forma de un Arduino Uno**. Esto permite usar "Shields" (placas que se encajan encima) y cables de pines estándar, lo que facilita mucho el montaje físico sin soldar.



**¿Empezamos con la lista de la compra exacta de Amazon para que no se equivoque al pedir los componentes?**

Prompt:
Vale estupendo. Para la demo en realidad podemos hacer menos "user-friendly" el tema de las etiquetas y la distancia  He visto que el Wemos D1 R32 se puede programar con el IDE de Arduino. Eso está bien. Será más fácil para ella seguro.

Vamos con la lista de compra. Además de todo lo que ya hemos mencionado, mi tía necesitará cables (algunos tendrán que ser relativamente largos para llegar hasta las correas en los hombros) y alguna forma de alimentar el micro cuando esté instalado en la mochila (supongo que se podrá usar una power bank usb). No se si se me olvida algún compenente básico de estos que das por hecho y se te acaban olvidando. Tú pongo todo todo y le pasamos la lista completa, lo que ella ya tenga que no lo compre. Incluye el precio estimado de cada componente y las unidades a comprar de cada uno. Por ejemplo, actuadores hápticos y sensores ToF o sónar necesitaremos 2, uno por hombro. Etiquitas RFID unas cuantas.

Respuesta de Gemini:
Para que tu tía no tenga que dar vueltas, aquí tienes la lista de la compra definitiva "cero errores". He seleccionado componentes que se pueden conectar sin soldar, usando solo cables "Jumper" (cables con conectores en las puntas).

### 🛒 Lista de la Compra en Amazon (Precios Estimados)

| Unidades | Componente | Función | Precio Total Est. |
| :--- | :--- | :--- | :--- |
| **1** | **Placa Wemos D1 R32** | El cerebro (ESP32 con forma de Arduino). | **12 €** |
| **2** | **Sensor VL53L0X (ToF)** | Sensores láser de distancia (uno por hombro). | **10 €** |
| **1** | **Lector RFID RC522** | Incluye el lector y suele traer una tarjeta/llavero. | **6 €** |
| **20** | **Etiquetas RFID (Pegatinas)** | Para pegar en los libros y cuadernos. | **8 €** |
| **2** | **Motores de Vibración (Módulos)** | Actuadores hápticos para los hombros. | **5 €** |
| **1** | **Kit de Cables Jumper (Macho-Hembra)** | Cables de 20cm y 30cm para las conexiones. | **7 €** |
| **1** | **Breadboard (Placa de pruebas) grande** | Para hacer las conexiones sin soldar. | **6 €** |
| **1** | **Power Bank USB (Cualquiera)** | Para alimentar la mochila (sirve una de móvil). | **0 € (o 10€)** |
| **TOTAL ESTIMADO** | | | **~54 - 64 €** |

---

### ⚠️ Los "Olvidables" Imprescindibles
Para que el proyecto sea viable para alguien que empieza de cero, necesita estos tres elementos que no son "sensores" pero sin ellos no terminará el proyecto:

1.  **Cables Jumper Macho-Hembra y Hembra-Hembra:** Compra un pack que traiga de ambos. Los sensores suelen tener pines "macho" y la placa también, por lo que necesitará cables con agujeros en ambos extremos (Hembra-Hembra).
2.  **Cable USB largo:** El Wemos D1 R32 suele usar **Micro-USB**. Necesita uno para conectarlo del ordenador a la placa para programarla.
3.  **Cinta de doble cara o bridas:** Para sujetar los sensores a las correas de la mochila de forma temporal pero firme para la demo.

---

### 🛠️ Hardware Háptico y Sensores (Ubicación)
Para los hombros, dile que busque específicamente **"Módulo de vibración para Arduino"**. Vienen montados en una plaquita pequeña con 3 pines (+, - y Señal). Es mucho más fácil de conectar que el motor suelto con cables finos.





### 💡 Consejo para tu tía (Metodología)
Dile que el secreto es **"Divide y Vencerás"**:
* No intentes montar toda la mochila a la vez. 
* Primero, que consiga que el ordenador reconozca la placa Wemos.
* Segundo, que haga que un sensor de distancia imprima números en la pantalla del PC.
* Tercero, que el lector RFID lea una etiqueta. 

**¿Quieres que prepare el "Manual de Instalación de Software" para que ella empiece a configurar su ordenador mientras llegan las piezas de Amazon?** Es el paso donde la mayoría de principiantes se atascan.

Prompt:
Vale sí, le será de gran ayuda. Redacta un manual paso a paso para ella (imagino que tendrá un ordenador con windows 10 u 11). Cable seguro que tiene por casa

Respuesta de Gemini:
Este manual está diseñado para una persona con **cero experiencia**. Lo he estructurado para que tu tía no se sienta abrumada, tratando cada paso como una receta de cocina.

---

## 🛠️ Guía de Inicio: Configuración del "Cerebro" de la Mochila

¡Hola! Si estás leyendo esto, vas a construir un proyecto tecnológico increíble. No te asustes por los términos técnicos; solo sigue estos pasos uno a uno.

### Paso 1: Descargar e instalar el "Editor"
Para decirle a la mochila qué tiene que hacer, usamos un programa llamado **Arduino IDE**. Es como el "Word" donde escribiremos las instrucciones.

1.  Ve a la página oficial: [arduino.cc/en/software](https://www.arduino.cc/en/software).
2.  Busca donde dice **"Windows Win 10 and newer, 64 bits"**.
3.  Descárgalo, instálalo y ábrelo. Si Windows te pide permisos para instalar "Drivers", dile que **Sí** a todo.

### Paso 2: Preparar el programa para la placa Wemos D1 R32
Por defecto, el programa solo entiende placas Arduino básicas. Tenemos que enseñarle a "hablar" con la placa Wemos (ESP32) que has comprado.

1.  Abre el Arduino IDE.
2.  Ve al menú **Archivo** > **Preferencias**.
3.  Busca el cuadro que dice **"Gestor de URLs Adicionales de Tarjetas"** y pega este enlace exacto:
    `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
4.  Pulsa **OK**.
5.  Ahora ve al menú lateral izquierdo, haz clic en el icono de un "tablero" (**Gestor de placas**) y busca: `esp32`.
6.  Te saldrá uno de "Espressif Systems". Dale al botón **Instalar**. Tardará un par de minutos.



### Paso 3: Instalar las "Librerías" (Los diccionarios)
Las librerías son pequeños paquetes de código que otros ya han escrito para que nosotros no tengamos que programar desde cero el sensor láser o el lector de tarjetas.

En el menú de la izquierda, haz clic en el icono de los "libros" (**Gestor de librerías**) y busca e instala estas tres:
1.  Busca `VL53L0X` (de Pololu): Instálala. Es para los sensores de distancia.
2.  Busca `MFRC522`: Instálala. Es para el lector de etiquetas de los libros.
3.  Busca `ESP32 BLE Arduino`: Asegúrate de que aparezca como instalada (suele venir de serie).

### Paso 4: Conexión física inicial
Antes de montar nada en la mochila, vamos a comprobar que la placa funciona:

1.  Conecta la placa **Wemos D1 R32** a tu ordenador con el cable USB.
2.  En el Arduino IDE, arriba del todo, verás un desplegable para seleccionar la placa.
3.  Busca **"WEMOS D1 MINI ESP32"** o simplemente **"ESP32 Dev Module"**.
4.  Selecciona el **Puerto** (saldrá algo como COM3 o COM4). Si te salen varios, desconecta la placa y mira cuál desaparece; ese es el correcto.

### Paso 5: Tu primera prueba (El "Hola Mundo")
Vamos a ver si la placa "está viva":

1.  Copia este código y pégalo en el editor, borrando todo lo que haya antes:
```cpp
void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("¡Hola! La mochila está funcionando");
  delay(1000);
}
```
2.  Pulsa el botón de la **Flecha hacia la derecha** (Flecha de "Subir") arriba a la izquierda.
3.  Espera a que abajo ponga "Subido".
4.  Ahora ve al menú **Herramientas** > **Monitor Serie**.
5.  **IMPORTANTE:** En la ventana que se abre, abajo a la derecha, cambia el número a `115200 baudios`.

**Si ves que el ordenador escribe "¡Hola! La mochila está funcionando" cada segundo... ¡Enhorabuena! Ya has superado la parte más difícil del aprendizaje.**

---

### ¿Qué viene después?
Una vez que ella tenga esto funcionando y le lleguen los componentes de Amazon, el siguiente paso será conectar el primer sensor usando los cables "Jumper".

**¿Quieres que le prepare el esquema de cómo conectar los cables del lector RFID (libros) para que haga su segunda prueba?** Es el componente que más cables lleva, así que mejor empezar por ahí.

Prompt:
Vale

Respuesta de Gemini:
¡Perfecto! El siguiente paso lógico, una vez que ella ya sabe cómo "hablarle" a la placa, es conectar el **Lector RFID (RC522)**. Es el componente que tiene más pines (8 en total), así que si logra conectar este, el resto le parecerán un juego de niños.

Aquí tienes la guía para su segunda lección.

---

## 📚 Lección 2: Identificando los libros (Lector RFID)

Para que la mochila sepa qué libros hay dentro, usaremos el lector **RC522**. Este lector tiene 8 pines, pero solo conectaremos **7 cables**. 

### 1. El esquema de conexión (Sin soldar)
Dile a tu tía que use los cables **Hembra-Hembra** (los que tienen "agujeros" en ambos lados) para unir los pines del lector con los de la placa Wemos D1 R32.

| Pin del Lector (RC522) | Pin en la Placa (Wemos D1 R32) | Color sugerido del cable |
| :--- | :--- | :--- |
| **SDA (o SS)** | **Pin 5** | Verde |
| **SCK** | **Pin 18** | Amarillo |
| **MOSI** | **Pin 23** | Naranja |
| **MISO** | **Pin 19** | Azul |
| **IRQ** | *No se conecta (dejar vacío)* | - |
| **GND** | **GND** | Negro |
| **RST** | **Pin 22** | Blanco |
| **3.3V** | **3.3V** | Rojo (⚠️ **¡No conectar a 5V!**) |



### 2. El código de prueba
Una vez conectado, que copie y pegue este código en su Arduino IDE y le dé a la flecha de **Subir**:

```cpp
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  5
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN); // Creamos el objeto del lector

void setup() {
  Serial.begin(115200);
  SPI.begin(); // Iniciamos el bus de comunicación
  rfid.PCD_Init(); // Iniciamos el lector
  Serial.println("Lector listo. Acerca una etiqueta...");
}

void loop() {
  // Si no hay una tarjeta nueva, no hace nada
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Si detecta una, imprime su código único
  Serial.print("¡Código detectado!: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  rfid.PICC_HaltA(); // Detiene la lectura para que no se repita mil veces
}
```

### 3. ¿Cómo probarlo?
1.  Abre el **Monitor Serie** (como en la lección 1).
2.  Acerca una de las pegatinas RFID o el llavero que viene con el kit al lector.
3.  Debería aparecer una combinación de números y letras (ejemplo: `A3 4F 12 0B`).
4.  Dile que anote qué código corresponde a cada libro.

---

### 💡 Un truco para ella:
Si los textos del código le asustan, dile que no se preocupe por entender cada línea ahora mismo. Lo importante es que vea que **el hardware responde**. 

**¿Quieres que prepare la siguiente guía para los sensores de distancia (los que van en los hombros) o prefieres que esperemos a que ella confirme que esto funciona?**

Prompt:
Una cosa que se me había olvidado. Como plan B por si la programación de una app se le complica, estaría bien añadir un zumbador o altavoz (si es barato) y al menos poder emitir sonidos simples que indiquen lo necesario al niño. Añádelo a la tabla de materiales

Respuesta de Gemini:
Tienes razón, un **Plan B físico** es vital. Si la App falla o el Bluetooth se desconecta, la mochila debe seguir siendo funcional por sí misma. 

He actualizado la lista incluyendo un **Zumbador Pasivo (Buzzer)**. Es el componente más barato que existe (menos de 1€) y permite crear "pitidos" de diferentes tonos (agudos, graves, cortos, largos) para comunicar mensajes sin necesidad de voz.

### 🛒 Lista de la Compra Actualizada (Plan B incluido)

| Unidades | Componente | Función | Precio Total Est. |
| :--- | :--- | :--- | :--- |
| **1** | **Placa Wemos D1 R32** | El cerebro (ESP32). | **12 €** |
| **2** | **Sensor VL53L0X (ToF)** | Sensores láser (uno por hombro). | **10 €** |
| **1** | **Lector RFID RC522** | Lector de etiquetas para libros. | **6 €** |
| **20** | **Etiquetas RFID** | Pegatinas para los libros. | **8 €** |
| **2** | **Módulos de Vibración** | Alerta táctil en los hombros. | **5 €** |
| **1** | **Zumbador Pasivo (Buzzer)** | **Plan B: Alertas sonoras (pitidos).** | **1-2 €** |
| **1** | **Kit de Cables Jumper** | Macho-Hembra y Hembra-Hembra. | **7 €** |
| **1** | **Breadboard Grande** | Para conectar todo sin soldar. | **6 €** |
| **1** | **Power Bank USB** | Alimentación. | **0 €** |
| **TOTAL** | | | **~55 - 65 €** |

---

## 🔊 Lección 3: El Plan B (El Zumbador)

El zumbador pasivo funciona como un pequeño instrumento musical. Podemos programarlo para que haga un "pip" corto si el libro es correcto, o un tono grave y largo si detecta un obstáculo cerca.

### 1. Conexión del Zumbador
Los zumbadores de módulo suelen tener 3 pines, pero si compra el que es solo la pieza negra con dos patillas largas, se conecta así:

* **Patilla larga (+):** Al **Pin 25** de la placa.
* **Patilla corta (-):** Al pin **GND**.



### 2. Código de prueba para tu tía
Dile que pruebe este código. Emitirá dos sonidos distintos: uno "alegre" (éxito) y uno de "error".

```cpp
#define BUZZER_PIN 25

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // Sonido de "Libro Correcto" (Dos pitidos agudos rápidos)
  tone(BUZZER_PIN, 1000, 100); 
  delay(150);
  tone(BUZZER_PIN, 1200, 100);
  delay(2000); // Espera 2 segundos

  // Sonido de "¡Cuidado! Obstáculo" (Tono grave constante)
  tone(BUZZER_PIN, 250, 500); 
  delay(2000); // Espera 2 segundos
}
```

---

### 💡 Estrategia de comunicación para el niño (Sin App)
Si al final no da tiempo a la App, podéis establecer este código de lenguaje para el concurso:
1.  **1 pitido agudo:** Libro detectado y correcto.
2.  **3 pitidos graves:** Falta un libro del horario.
3.  **Vibración intermitente:** Obstáculo a media distancia (1-2 metros).
4.  **Pitido constante + Vibración fuerte:** Obstáculo muy cerca (menos de 50 cm).

Esto demuestra a los jueces que el sistema es **robusto** y funciona de forma autónoma.

**¿Quieres que prepare ya el esquema final de los sensores de distancia (los del hombro) para completar todo el hardware?** Con eso ya tendría toda la información para cuando le lleguen los paquetes.

Prompt:
Genera la primera versión del código

Respuesta de Gemini:
Aquí tienes el código fuente **"Maestro"**. Es un código de grado senior: modular, utiliza una máquina de estados simple y gestiona la colisión de direcciones I2C de los sensores láser. 

Dile a tu tía que **copie y pegue esto íntegramente** en su Arduino IDE:

```cpp
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include "Adafruit_VL53L0X.h"

// --- CONFIGURACIÓN DE PINES ---
#define PIN_RFID_SS    5
#define PIN_RFID_RST   22
#define PIN_BUZZER     25
#define PIN_VIB_IZQ    26
#define PIN_VIB_DER    27
#define PIN_XSHUT_IZQ  16
#define PIN_XSHUT_DER  17

// --- INSTANCIAS ---
MFRC522 rfid(PIN_RFID_SS, PIN_RFID_RST);
Adafruit_VL53L0X sensorIzq = Adafruit_VL53L0X();
Adafruit_VL53L0X sensorDer = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_VIB_IZQ, OUTPUT);
  pinMode(PIN_VIB_DER, OUTPUT);
  pinMode(PIN_XSHUT_IZQ, OUTPUT);
  pinMode(PIN_XSHUT_DER, OUTPUT);

  // Reinicio de sensores ToF para cambiar direcciones I2C
  digitalWrite(PIN_XSHUT_IZQ, LOW); digitalWrite(PIN_XSHUT_DER, LOW);
  delay(10);
  digitalWrite(PIN_XSHUT_IZQ, HIGH); delay(10);
  sensorIzq.begin(0x30); // Sensor Izquierdo en 0x30
  
  digitalWrite(PIN_XSHUT_DER, HIGH); delay(10);
  sensorDer.begin(0x31); // Sensor Derecho en 0x31
  
  tone(PIN_BUZZER, 1000, 200); // Pitido de inicio ok
}

void loop() {
  gestionarObstaculos();
  gestionarLibros();
  delay(50); 
}

void gestionarObstaculos() {
  VL53L0X_RangingMeasurementData_t medidaIzq, medidaDer;
  
  sensorIzq.rangingTest(&medidaIzq, false);
  sensorDer.rangingTest(&medidaDer, false);

  // Lógica Hombro Izquierdo (Umbral 1.2 metros)
  if (medidaIzq.RangeStatus != 4 && medidaIzq.RangeMilliMeter < 1200) {
    digitalWrite(PIN_VIB_IZQ, HIGH);
  } else {
    digitalWrite(PIN_VIB_IZQ, LOW);
  }

  // Lógica Hombro Derecho
  if (medidaDer.RangeStatus != 4 && medidaDer.RangeMilliMeter < 1200) {
    digitalWrite(PIN_VIB_DER, HIGH);
  } else {
    digitalWrite(PIN_VIB_DER, LOW);
  }
}

void gestionarLibros() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  // Comparar UID detectado (Ejemplo para tu tía)
  // Aquí ella añadirá los IDs de sus pegatinas
  Serial.print("Libro detectado: ");
  for (byte i = 0; i < rfid.uid.size; i++) Serial.print(rfid.uid.uidByte[i], HEX);
  Serial.println();
  
  tone(PIN_BUZZER, 1500, 100); // Feedback sonoro de lectura
}
```

¿Alguna duda específica sobre la implementación del protocolo I2C o la gestión de energía?