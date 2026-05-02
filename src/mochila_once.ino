#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include "Adafruit_VL53L0X.h"

// --- PINES ---
#define PIN_RFID_SS    5
#define PIN_RFID_RST   22
#define PIN_BUZZER     25
#define PIN_VIB_IZQ    26
#define PIN_VIB_DER    27
#define PIN_XSHUT_IZQ  16
#define PIN_XSHUT_DER  17
#define PIN_US_TRIG    4
#define PIN_US_ECHO    2
#define PIN_BTN_MATERIAL 32
#define PIN_BTN_NAV      33

// --- INSTANCIAS ---
MFRC522 rfid(PIN_RFID_SS, PIN_RFID_RST);
Adafruit_VL53L0X loxIzq = Adafruit_VL53L0X();
Adafruit_VL53L0X loxDer = Adafruit_VL53L0X();

enum EstadoDemo {
  ESTADO_ESPERA,
  ESTADO_MATERIAL,
  ESTADO_NAVEGACION
};

EstadoDemo estado = ESTADO_ESPERA;
uint32_t tInicioEstado = 0;
uint32_t tUltimoPulsoNav = 0;

// UIDs de ejemplo (4 bytes) para materiales del dia
const byte UID_MATEMATICAS[4] = {0x12, 0x34, 0x56, 0x78};
const byte UID_LENGUA[4]      = {0xBA, 0x98, 0x76, 0x54};
const byte UID_ESTUCHE[4]     = {0xAA, 0xBB, 0xCC, 0xDD};

bool materialVisto[3] = {false, false, false};

bool btnMaterialPrev = true;
bool btnNavPrev = true;
uint32_t tUltimoDebounce = 0;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_VIB_IZQ, OUTPUT);
  pinMode(PIN_VIB_DER, OUTPUT);
  pinMode(PIN_XSHUT_IZQ, OUTPUT);
  pinMode(PIN_XSHUT_DER, OUTPUT);
  pinMode(PIN_US_TRIG, OUTPUT);
  pinMode(PIN_US_ECHO, INPUT);
  pinMode(PIN_BTN_MATERIAL, INPUT_PULLUP);
  pinMode(PIN_BTN_NAV, INPUT_PULLUP);

  // Inicialización sensores ToF
  digitalWrite(PIN_XSHUT_IZQ, LOW); digitalWrite(PIN_XSHUT_DER, LOW);
  delay(10);
  digitalWrite(PIN_XSHUT_IZQ, HIGH); delay(10);
  loxIzq.begin(0x30);
  digitalWrite(PIN_XSHUT_DER, HIGH); delay(10);
  loxDer.begin(0x31);

  tInicioEstado = millis();
}

long leerUltrasonido() {
  digitalWrite(PIN_US_TRIG, LOW); delayMicroseconds(2);
  digitalWrite(PIN_US_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_US_TRIG, LOW);
  return pulseIn(PIN_US_ECHO, HIGH) * 0.034 / 2;
}

bool uidIgual(const byte *a, const byte *b, byte len) {
  for (byte i = 0; i < len; i++) {
    if (a[i] != b[i]) {
      return false;
    }
  }
  return true;
}

void resetMaterial() {
  for (int i = 0; i < 3; i++) {
    materialVisto[i] = false;
  }
}

void beepCorto(uint16_t freq) {
  tone(PIN_BUZZER, freq, 120);
}

void melodiaExito() {
  tone(PIN_BUZZER, 1200, 120); delay(140);
  tone(PIN_BUZZER, 1500, 120); delay(140);
  tone(PIN_BUZZER, 1800, 120); delay(140);
}

void setEstado(EstadoDemo nuevo) {
  estado = nuevo;
  tInicioEstado = millis();
  if (estado == ESTADO_MATERIAL) {
    resetMaterial();
  }
  if (estado == ESTADO_NAVEGACION) {
    tUltimoPulsoNav = 0;
  }
}

void leerBotones() {
  bool btnMat = digitalRead(PIN_BTN_MATERIAL);
  bool btnNav = digitalRead(PIN_BTN_NAV);

  if (millis() - tUltimoDebounce > 30) {
    if (btnMaterialPrev && !btnMat) {
      setEstado(ESTADO_MATERIAL);
    }
    if (btnNavPrev && !btnNav) {
      setEstado(ESTADO_NAVEGACION);
    }
    tUltimoDebounce = millis();
  }

  btnMaterialPrev = btnMat;
  btnNavPrev = btnNav;
}

void procesarMaterial() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  bool encontrado = false;
  if (rfid.uid.size == 4) {
    if (uidIgual(rfid.uid.uidByte, UID_MATEMATICAS, 4)) {
      materialVisto[0] = true;
      encontrado = true;
    } else if (uidIgual(rfid.uid.uidByte, UID_LENGUA, 4)) {
      materialVisto[1] = true;
      encontrado = true;
    } else if (uidIgual(rfid.uid.uidByte, UID_ESTUCHE, 4)) {
      materialVisto[2] = true;
      encontrado = true;
    }
  }

  if (encontrado) {
    beepCorto(1200);
  } else {
    tone(PIN_BUZZER, 400, 250);
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  if (materialVisto[0] && materialVisto[1] && materialVisto[2]) {
    melodiaExito();
    setEstado(ESTADO_ESPERA);
  }
}

void navegarDemo() {
  uint32_t t = millis() - tInicioEstado;

  if (t >= 20000) {
    melodiaExito();
    setEstado(ESTADO_ESPERA);
    return;
  }

  // Pulsos de navegacion en instantes clave
  if (t >= 10000 && t < 10500 && tUltimoPulsoNav == 0) {
    tUltimoPulsoNav = t;
    digitalWrite(PIN_VIB_DER, HIGH);
    delay(120);
    digitalWrite(PIN_VIB_DER, LOW);
    delay(80);
    digitalWrite(PIN_VIB_DER, HIGH);
    delay(120);
    digitalWrite(PIN_VIB_DER, LOW);
    beepCorto(1400);
  }

  if (t >= 15000 && t < 15500 && tUltimoPulsoNav == 10000) {
    tUltimoPulsoNav = 15000;
    digitalWrite(PIN_VIB_IZQ, HIGH);
    delay(120);
    digitalWrite(PIN_VIB_IZQ, LOW);
    delay(80);
    digitalWrite(PIN_VIB_IZQ, HIGH);
    delay(120);
    digitalWrite(PIN_VIB_IZQ, LOW);
    tone(PIN_BUZZER, 700, 160);
  }
}

void loop() {
  leerBotones();

  // 1. GESTION SENSORES DE OBSTACULOS
  VL53L0X_RangingMeasurementData_t mIzq, mDer;
  loxIzq.rangingTest(&mIzq, false);
  loxDer.rangingTest(&mDer, false);

  bool alertaIzq = (mIzq.RangeStatus != 4 && mIzq.RangeMilliMeter < 1200);
  bool alertaDer = (mDer.RangeStatus != 4 && mDer.RangeMilliMeter < 1200);

  long distUS = leerUltrasonido();
  bool alertaCritica = (distUS > 0 && distUS < 30);

  if (alertaCritica) {
    digitalWrite(PIN_VIB_IZQ, HIGH);
    digitalWrite(PIN_VIB_DER, HIGH);
    tone(PIN_BUZZER, 2000, 60);
  } else {
    digitalWrite(PIN_VIB_IZQ, alertaIzq ? HIGH : LOW);
    digitalWrite(PIN_VIB_DER, alertaDer ? HIGH : LOW);
  }

  // 2. MAQUINA DE ESTADOS
  if (estado == ESTADO_MATERIAL) {
    procesarMaterial();
  } else if (estado == ESTADO_NAVEGACION) {
    navegarDemo();
  }

  delay(20);
}