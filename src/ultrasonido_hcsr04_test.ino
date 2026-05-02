// HC-SR04 test (2 sensores: izquierdo y derecho)

// Pins aligned with src/mochila_once.ino
#define PIN_US_TRIG_IZQ 4
#define PIN_US_ECHO_IZQ 2
#define PIN_US_TRIG_DER 12
#define PIN_US_ECHO_DER 13

long leerUltrasonidoCm(int pinTrig, int pinEcho) {
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);

  long duracion = pulseIn(pinEcho, HIGH, 30000); // 30 ms timeout
  if (duracion == 0) {
    return -1;
  }
  return (duracion * 0.034) / 2;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_US_TRIG_IZQ, OUTPUT);
  pinMode(PIN_US_ECHO_IZQ, INPUT);
  pinMode(PIN_US_TRIG_DER, OUTPUT);
  pinMode(PIN_US_ECHO_DER, INPUT);
  Serial.println("HC-SR04 listo (IZQ y DER)." );
}

void loop() {
  long distanciaIzq = leerUltrasonidoCm(PIN_US_TRIG_IZQ, PIN_US_ECHO_IZQ);
  long distanciaDer = leerUltrasonidoCm(PIN_US_TRIG_DER, PIN_US_ECHO_DER);

  Serial.print("Izq: ");
  if (distanciaIzq < 0) {
    Serial.print("Sin lectura");
  } else {
    Serial.print(distanciaIzq);
    Serial.print(" cm");
  }

  Serial.print(" | Der: ");
  if (distanciaDer < 0) {
    Serial.print("Sin lectura");
  } else {
    Serial.print(distanciaDer);
    Serial.print(" cm");
  }

  Serial.println();
  delay(200);
}
