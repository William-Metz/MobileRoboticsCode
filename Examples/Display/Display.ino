#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "../../include/wscommunicator.h" 

const char* SSID = "Pomona";
const uint16_t PORT = 8181;
const unsigned long HEARTBEAT_INTERVAL = 1000;
WSCommunicator wsCommunicator(SSID, PORT, HEARTBEAT_INTERVAL);

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);

  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.clearDisplay();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  wsCommunicator.setup();

  IPAddress ip = wsCommunicator.getIpAddress();
  uint16_t port = wsCommunicator.getPort();

  u8x8.drawString(0, 0, "IP Address:");
  u8x8.drawString(0, 1, ip.toString().c_str());

  u8x8.drawString(0, 3, "Port:");
  char portStr[6];
  snprintf(portStr, sizeof(portStr), "%u", port);
  u8x8.drawString(0, 4, portStr);
}

void loop() {
  wsCommunicator.loopStep();

}
