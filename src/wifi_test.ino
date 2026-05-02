#include <WiFi.h>

// Rellena con tu red WiFi
const char *WIFI_SSID = "TU_SSID";
const char *WIFI_PASS = "TU_PASSWORD";

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Conectando a WiFi");
  uint32_t startMs = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - startMs > 15000) {
      Serial.println("\nTimeout de WiFi");
      return;
    }
  }

  Serial.println("\nWiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Nada que hacer
}
