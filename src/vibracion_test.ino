// Vibration motors test

// Pins aligned with mochila_once.ino
#define PIN_VIB_IZQ 26
#define PIN_VIB_DER 27

void setup() {
  pinMode(PIN_VIB_IZQ, OUTPUT);
  pinMode(PIN_VIB_DER, OUTPUT);
}

void loop() {
  digitalWrite(PIN_VIB_IZQ, HIGH);
  digitalWrite(PIN_VIB_DER, LOW);
  delay(400);

  digitalWrite(PIN_VIB_IZQ, LOW);
  digitalWrite(PIN_VIB_DER, HIGH);
  delay(400);

  digitalWrite(PIN_VIB_IZQ, HIGH);
  digitalWrite(PIN_VIB_DER, HIGH);
  delay(300);

  digitalWrite(PIN_VIB_IZQ, LOW);
  digitalWrite(PIN_VIB_DER, LOW);
  delay(500);
}
