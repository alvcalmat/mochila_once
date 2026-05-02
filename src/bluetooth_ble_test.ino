#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

// Simple BLE UART-like service
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLECharacteristic *txChar;

void setup() {
  Serial.begin(115200);

  BLEDevice::init("Mochila_ONCE");
  BLEServer *server = BLEDevice::createServer();
  BLEService *service = server->createService(SERVICE_UUID);

  txChar = service->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );

  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(false);
  advertising->setMinPreferred(0x06);
  advertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE listo. Busca 'Mochila_ONCE' en el movil.");
}

void loop() {
  static uint32_t lastMs = 0;
  if (millis() - lastMs >= 1000) {
    lastMs = millis();
    txChar->setValue("ping");
    txChar->notify();
    Serial.println("BLE -> ping");
  }
}
