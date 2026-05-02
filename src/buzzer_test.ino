// Buzzer test

// Pins aligned with mochila_once.ino
#define PIN_BUZZER 25

void setup() {
  pinMode(PIN_BUZZER, OUTPUT);
}

void loop() {
  tone(PIN_BUZZER, 1200, 150);
  delay(300);
  tone(PIN_BUZZER, 1800, 150);
  delay(500);
}
