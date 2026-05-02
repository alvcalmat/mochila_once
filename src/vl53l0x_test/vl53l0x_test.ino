#include <Wire.h>
#include "Adafruit_VL53L0X.h"

// Pins aligned with mochila_once.ino
#define PIN_XSHUT_IZQ 16
#define PIN_XSHUT_DER 17

Adafruit_VL53L0X loxIzq;
Adafruit_VL53L0X loxDer;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(PIN_XSHUT_IZQ, OUTPUT);
  pinMode(PIN_XSHUT_DER, OUTPUT);

  // Reset both sensors
  digitalWrite(PIN_XSHUT_IZQ, LOW);
  digitalWrite(PIN_XSHUT_DER, LOW);
  delay(10);

  // Left sensor at 0x30
  digitalWrite(PIN_XSHUT_IZQ, HIGH);
  delay(10);
  if (!loxIzq.begin(0x30)) {
    Serial.println("Error: VL53L0X izq no encontrado.");
  }

  // Right sensor at 0x31
  digitalWrite(PIN_XSHUT_DER, HIGH);
  delay(10);
  if (!loxDer.begin(0x31)) {
    Serial.println("Error: VL53L0X der no encontrado.");
  }

  Serial.println("VL53L0X listo.");
}

void loop() {
  VL53L0X_RangingMeasurementData_t mIzq;
  VL53L0X_RangingMeasurementData_t mDer;

  loxIzq.rangingTest(&mIzq, false);
  loxDer.rangingTest(&mDer, false);

  Serial.print("Izq: ");
  if (mIzq.RangeStatus != 4) {
    Serial.print(mIzq.RangeMilliMeter);
    Serial.print(" mm");
  } else {
    Serial.print("fuera de rango");
  }

  Serial.print(" | Der: ");
  if (mDer.RangeStatus != 4) {
    Serial.print(mDer.RangeMilliMeter);
    Serial.print(" mm");
  } else {
    Serial.print("fuera de rango");
  }

  Serial.println();
  delay(200);
}
