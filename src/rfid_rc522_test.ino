#include <SPI.h>
#include <MFRC522.h>

// Pins aligned with mochila_once.ino
#define PIN_RFID_SS  5
#define PIN_RFID_RST 22

MFRC522 rfid(PIN_RFID_SS, PIN_RFID_RST);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID listo. Acerca una etiqueta...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID:");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(250);
}
