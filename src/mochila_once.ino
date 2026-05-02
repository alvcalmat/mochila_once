// Incluimos los "diccionarios" o librerías que enseñan a la placa a usar los sensores
#include <Wire.h>             // Para hablar con los sensores láser (comunicación I2C)
#include <SPI.h>              // Para hablar con el lector de etiquetas (comunicación SPI)
#include <MFRC522.h>          // Librería específica para el lector de libros (RFID)
#include "Adafruit_VL53L0X.h" // Librería específica para los medidores de distancia láser

// --- PINES: Dónde va conectado cada cable en la placa ---
#define PIN_LECTOR_RFID_SS 5      // Pin selector del lector de libros
#define PIN_LECTOR_RFID_RST 22    // Pin para reiniciar el lector de libros
#define PIN_BOCINA 25             // Pin para el zumbador/altavoz (emite pitidos)
#define PIN_VIBRADOR_IZQUIERDO 26 // Pin del motor que vibra en el hombro izquierdo
#define PIN_VIBRADOR_DERECHO 27   // Pin del motor que vibra en el hombro derecho
#define PIN_LASER_IZQUIERDO 16    // Pin para encender/apagar el láser izquierdo
#define PIN_LASER_DERECHO 17      // Pin para encender/apagar el láser derecho
#define PIN_ULTRASONIDOS_TRIG 4   // Pin que dispara el sonido (Trigger)
#define PIN_ULTRASONIDOS_ECHO 2   // Pin que escucha el rebote (Echo)
#define PIN_BOTON_MATERIAL 32     // Botón para entrar en el modo "Meter Libros"
#define PIN_BOTON_NAVEGACION 33   // Botón para entrar en el modo "Camino al Colegio"

// --- OBJETOS: Creamos a los "trabajadores" que manejarán los sensores ---
MFRC522 lectorRFID(PIN_LECTOR_RFID_SS, PIN_LECTOR_RFID_RST); // Trabajador del lector
Adafruit_VL53L0X laserIzquierdo = Adafruit_VL53L0X();        // Trabajador del láser izquierdo
Adafruit_VL53L0X laserDerecho = Adafruit_VL53L0X();          // Trabajador del láser derecho

// --- ESTADOS: Las tareas que puede estar haciendo la mochila ---
enum EstadoDemo
{
  ESTADO_ESPERA,    // No hace nada, esperando a que pulsemos un botón
  ESTADO_MATERIAL,  // Revisando si metemos los libros de hoy
  ESTADO_NAVEGACION // Guiando al niño al colegio (simulación)
};

// Empezamos siempre en estado de espera
EstadoDemo estado = ESTADO_ESPERA;
uint32_t temporizadorEstado = 0; // Reloj para contar los segundos en un estado
uint32_t temporizadorRuta = 0;   // Reloj para saber cuándo dar la siguiente instrucción gps

// --- LISTA DE LIBROS DE HOY (Códigos secretos de las pegatinas) ---
// Cámbialos luego por los que te salgan en el test del RFID
const byte UID_MATEMATICAS[4] = {0x12, 0x34, 0x56, 0x78};
const byte UID_LENGUA[4] = {0xBA, 0x98, 0x76, 0x54};
const byte UID_ESTUCHE[4] = {0xAA, 0xBB, 0xCC, 0xDD};

// Lista de casillas para tachar cuando encontremos los 3 libros (Falso = No lo tengo aún, Verdadero = ¡Ya lo tengo!)
bool librosEnMochila[3] = {false, false, false};

// Memorias para los botones, para no leer pulsaciones repetidas súper rápido
bool botonMaterialAntes = true;
bool botonNavPrev = true;
uint32_t tiempoUltimoBoton = 0; // Para lo que llamamos "debounce" (evitar rebotes del muelle del botón)

// =========================================================================
// PREPARACIÓN (SETUP): Esto se ejecuta UNA sola vez al encender la mochila
// =========================================================================
void setup()
{
  Serial.begin(115200);  // Iniciamos la pantalla del ordenador para ver mensajes
  SPI.begin();           // Encendemos la comunicación con el lector
  lectorRFID.PCD_Init(); // Despertamos al lector de libros

  // Decimos a la placa quién es salida (enviar electricidad) y quién entrada (recibir)
  pinMode(PIN_BOCINA, OUTPUT);
  pinMode(PIN_VIBRADOR_IZQUIERDO, OUTPUT);
  pinMode(PIN_VIBRADOR_DERECHO, OUTPUT);
  pinMode(PIN_LASER_IZQUIERDO, OUTPUT);
  pinMode(PIN_LASER_DERECHO, OUTPUT);
  pinMode(PIN_ULTRASONIDOS_TRIG, OUTPUT);
  pinMode(PIN_ULTRASONIDOS_ECHO, INPUT);

  // Los botones tienen resistencia interna (PULLUP)
  pinMode(PIN_BOTON_MATERIAL, INPUT_PULLUP);
  pinMode(PIN_BOTON_NAVEGACION, INPUT_PULLUP);

  // Inicialización de los sensores Láser (Tienen que encenderse de uno en uno)
  digitalWrite(PIN_LASER_IZQUIERDO, LOW);
  digitalWrite(PIN_LASER_DERECHO, LOW); // Apagamos ambos
  delay(10);                            // Esperamos un poquito

  digitalWrite(PIN_LASER_IZQUIERDO, HIGH); // Encendemos el izquierdo
  delay(10);
  laserIzquierdo.begin(0x30); // Le asignamos su dirección secreta para que no se pise con el derecho

  digitalWrite(PIN_LASER_DERECHO, HIGH); // Encendemos el derecho
  delay(10);
  laserDerecho.begin(0x31); // Dirección secreta del derecho

  // Memorizamos el momento en el que hemos encendido
  temporizadorEstado = millis();
}

// Función que lee la distancia del ultrasonidos frontal
long leerUltrasonido()
{
  digitalWrite(PIN_ULTRASONIDOS_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_ULTRASONIDOS_TRIG, HIGH);
  delayMicroseconds(10); // Disparo de sonido de 10 microsegundos
  digitalWrite(PIN_ULTRASONIDOS_TRIG, LOW);

  // Contamos cuánto tarda en volver el eco y lo pasamos a centímetros
  return pulseIn(PIN_ULTRASONIDOS_ECHO, HIGH) * 0.034 / 2;
}

// Función que comprueba si dos códigos de pegatina son iguales
bool compararPegatina(const byte *pegatinaLeida, const byte *pegatinaGuardada, byte longitud)
{
  for (byte i = 0; i < longitud; i++)
  {
    if (pegatinaLeida[i] != pegatinaGuardada[i])
    {
      return false; // Son diferentes
    }
  }
  return true; // Son exactamente iguales
}

// Función para vaciar la lista al empezar de nuevo
void vaciarListaMochila()
{
  for (int i = 0; i < 3; i++)
  {
    librosEnMochila[i] = false; // "Desmarcamos" las 3 casillas
  }
}

// Función para emitir un pitidito rápido (ej: al pasar un libro correcto)
void pitidoRapido(uint16_t tono)
{
  tone(PIN_BOCINA, tono, 120); // Pitido de 'tono' hercios durante 120 milisegundos
}

// Función para la musiquita feliz de éxito
void tocarMelodiaExito()
{
  tone(PIN_BOCINA, 1200, 120);
  delay(140);
  tone(PIN_BOCINA, 1500, 120);
  delay(140);
  tone(PIN_BOCINA, 1800, 120);
  delay(140);
}

// Función para cambiar de estado de forma segura
void cambiarModo(EstadoDemo nuevoModo)
{
  estado = nuevoModo;            // Cambiamos la tarea actual
  temporizadorEstado = millis(); // Reiniciamos el reloj para empezar a contar de nuevo

  if (estado == ESTADO_MATERIAL)
  {
    vaciarListaMochila(); // Si empezamos a guardar, vaciamos las casillas mentales
  }
  if (estado == ESTADO_NAVEGACION)
  {
    temporizadorRuta = 0; // Si vamos a caminar, reseteamos la posición en las instrucciones
  }
}

// Comprobar si hemos pulsado algún botón
void vigilarBotones()
{
  bool pulsadoMaterial = digitalRead(PIN_BOTON_MATERIAL);
  bool pulsadoNavegacion = digitalRead(PIN_BOTON_NAVEGACION);

  // Si han pasado 30 milisegundos desde la ultima vigilancia (evitar vibraciones del muelle del botón)
  if (millis() - tiempoUltimoBoton > 30)
  {
    // Si estaba "arriba" (true) y ahora está abajo (false = pulsado)
    if (botonMaterialAntes && !pulsadoMaterial)
    {
      cambiarModo(ESTADO_MATERIAL);
    }
    if (botonNavPrev && !pulsadoNavegacion)
    {
      cambiarModo(ESTADO_NAVEGACION);
    }
    tiempoUltimoBoton = millis(); // Anotamos en qué momento lo hicimos
  }

  // Guardamos cómo están los botones para la próxima vez que vigilemos
  botonMaterialAntes = pulsadoMaterial;
  botonNavPrev = pulsadoNavegacion;
}

// --- TAREA 1: MODALIDAD METER LIBROS ---
void revisarLibros()
{
  // Si no hay tarjeta (libro) o da error al leer, no hacemos nada
  if (!lectorRFID.PICC_IsNewCardPresent() || !lectorRFID.PICC_ReadCardSerial())
  {
    return; // Salir de la función
  }

  bool libroCorrecto = false;

  // Si la pegatina es del tamaño adecuado (4 bytes)
  if (lectorRFID.uid.size == 4)
  {
    // Miramos si la pegatina es la de mates
    if (compararPegatina(lectorRFID.uid.uidByte, UID_MATEMATICAS, 4))
    {
      librosEnMochila[0] = true; // Tachamos la casilla 0 en nuestra cabeza
      libroCorrecto = true;
    }
    // Si no, miramos si es la de lengua
    else if (compararPegatina(lectorRFID.uid.uidByte, UID_LENGUA, 4))
    {
      librosEnMochila[1] = true; // Tachamos casilla 1
      libroCorrecto = true;
    }
    // Si no, miramos si es el estuche
    else if (compararPegatina(lectorRFID.uid.uidByte, UID_ESTUCHE, 4))
    {
      librosEnMochila[2] = true; // Tachamos casilla 2
      libroCorrecto = true;
    }
  }

  // Comportamiento según acierto o error
  if (libroCorrecto)
  {
    pitidoRapido(1200); // Pitido de acierto
  }
  else
  {
    tone(PIN_BOCINA, 400, 250); // Pitido grave y feo de "Este libro no toca hoy"
  }

  lectorRFID.PICC_HaltA(); // Le decimos a la pegatina "ya te he leído, duérmete"
  lectorRFID.PCD_StopCrypto1();

  // Si las tres casillas están tachadas, lo tenemos todo
  if (librosEnMochila[0] && librosEnMochila[1] && librosEnMochila[2])
  {
    tocarMelodiaExito();
    cambiarModo(ESTADO_ESPERA); // Ya hemos acabado la tarea, a descansar
  }
}

// --- TAREA 2: MODALIDAD RUTA AL COLEGIO ---
void simularRuta()
{
  // Calculamos el tiempo desde que entramos en modo navegacíon
  uint32_t segundosEnLaRuta = millis() - temporizadorEstado;

  // A los 20.000 milisegundos (20 segundos) hemos llegado al cole
  if (segundosEnLaRuta >= 20000)
  {
    tocarMelodiaExito();
    cambiarModo(ESTADO_ESPERA); // Desactivamos el gps, fin del modo ruta
    return;
  }

  // Gira a la DERECHA en el segundo 10
  // Si llevamos entre 10 y 10.5 segundos y no hemos avisado todavia
  if (segundosEnLaRuta >= 10000 && segundosEnLaRuta < 10500 && temporizadorRuta == 0)
  {
    temporizadorRuta = segundosEnLaRuta; // Marcamos que ya hemos avisado de esto
    // Vibrar dos veces en el hombro derecho
    digitalWrite(PIN_VIBRADOR_DERECHO, HIGH);
    delay(120);
    digitalWrite(PIN_VIBRADOR_DERECHO, LOW);
    delay(80);
    digitalWrite(PIN_VIBRADOR_DERECHO, HIGH);
    delay(120);
    digitalWrite(PIN_VIBRADOR_DERECHO, LOW);
    pitidoRapido(1400); // Y pitido alto tipo gps
  }

  // Gira a la IZQUIERDA en el segundo 15
  // Si llevamos entre 15 y 15.5 seg y nuestra ultima instruccion fue en el seg 10
  if (segundosEnLaRuta >= 15000 && segundosEnLaRuta < 15500 && temporizadorRuta > 9000 && temporizadorRuta < 11000)
  {
    temporizadorRuta = 15000;
    // Vibrar dos veces en el hombro izquierdo
    digitalWrite(PIN_VIBRADOR_IZQUIERDO, HIGH);
    delay(120);
    digitalWrite(PIN_VIBRADOR_IZQUIERDO, LOW);
    delay(80);
    digitalWrite(PIN_VIBRADOR_IZQUIERDO, HIGH);
    delay(120);
    digitalWrite(PIN_VIBRADOR_IZQUIERDO, LOW);
    tone(PIN_BOCINA, 700, 160); // Pitido más grave
  }
}

// =========================================================================
// BUCLE PRINCIPAL (LOOP): Esto se repite infinitamente muy rápido
// =========================================================================
void loop()
{
  vigilarBotones(); // Estar constantemente atentos a los botones

  // --- ESCUDO ANTICHOQUES: Los láseres leen distancias de seguridad ---
  VL53L0X_RangingMeasurementData_t medidaIzq, medidaDer;
  laserIzquierdo.rangingTest(&medidaIzq, false); // Leemos el laser izq
  laserDerecho.rangingTest(&medidaDer, false);   // Leemos el laser der

  // Hay obstáculo a menos de 1.2m (1200 milímetros) y el láser no da error (-4)
  bool cuidadoIzquierda = (medidaIzq.RangeStatus != 4 && medidaIzq.RangeMilliMeter < 1200);
  bool cuidadoDerecha = (medidaDer.RangeStatus != 4 && medidaDer.RangeMilliMeter < 1200);

  // Leemos distancia cortísima frontal
  long distanciaUltra = leerUltrasonido();
  // Hay coche/persona cruzada a menos de 30 cm de la cara
  bool peligroChoqueMuro = (distanciaUltra > 0 && distanciaUltra < 30);

  // Reacciones físicas a los choques
  if (peligroChoqueMuro)
  {
    // Frena! (Ambos vibran a tope a la vez y la bocina chilla como loca)
    digitalWrite(PIN_VIBRADOR_IZQUIERDO, HIGH);
    digitalWrite(PIN_VIBRADOR_DERECHO, HIGH);
    tone(PIN_BOCINA, 2000, 60);
  }
  else
  {
    // Si no hay emergencia, que vibre levemente el hombro del lado que tiene pared cerca
    digitalWrite(PIN_VIBRADOR_IZQUIERDO, cuidadoIzquierda ? HIGH : LOW);
    digitalWrite(PIN_VIBRADOR_DERECHO, cuidadoDerecha ? HIGH : LOW);
  }

  // --- LAS DOS TAREAS POSIBLES ---
  // Según el botón que pulsamos ejecutamos revisar etiqueta o simular la acera
  if (estado == ESTADO_MATERIAL)
  {
    revisarLibros();
  }
  else if (estado == ESTADO_NAVEGACION)
  {
    simularRuta();
  }

  delay(20); // Mini pausa para no bloquear la placa respirando
}
