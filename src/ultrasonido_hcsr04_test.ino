// HC-SR04 test

// Pins aligned with mochila_once.ino
#define PIN_US_TRIG 4
#define PIN_US_ECHO 2

long leerUltrasonidoCm() {
  digitalWrite(PIN_US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_US_TRIG, LOW);

  long duracion = pulseIn(PIN_US_ECHO, HIGH, 30000); // 30 ms timeout
  if (duracion == 0) {
    return -1;
  }
  return (duracion * 0.034) / 2;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_US_TRIG, OUTPUT);
  pinMode(PIN_US_ECHO, INPUT);
  Serial.println("HC-SR04 listo.");
}

void loop() {
  long distancia = leerUltrasonidoCm();
  if (distancia < 0) {
    Serial.println("Sin lectura");
  } else {
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");
  }
  delay(200);
}
