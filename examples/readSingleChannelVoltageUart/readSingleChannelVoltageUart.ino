/*!
 * @file readSingleChannelVoltageUart.ino
 * @brief Read silkscreen AIN0 over UART RTU.
 * @n Module GPIO2 must be high (UART). Address is 0x32, 115200 8N1.
 * @n On UNO and Mega this sketch uses Serial, so the monitor and the module share D0/D1.
 * @n | Board  | MCU TX -> module RX | MCU RX <- module TX |
 * @n | UNO    | D1                  | D0                  |
 * @n | Mega   | D1                  | D0                  |
 * @n | ESP32  | GPIO17              | GPIO16              |
 * @n Connect GND to GND. VCC is 3.3 V or 5 V per the module.
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (https://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-08
 * @url https://github.com/DFRobot/DFRobot_ADS122C14
 */
#include <DFRobot_ADS122C14.h>

#if defined(ESP32)
DFRobot_ADS122C14 adc(DFRobot_ADS122C14::RTU_ADDRESS_DEFAULT, &Serial2);
#else
DFRobot_ADS122C14 adc(DFRobot_ADS122C14::RTU_ADDRESS_DEFAULT, &Serial);
#endif

void setup() {
  Serial.begin(115200);
#if defined(ESP32)
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
#else
  delay(3000);
#endif
  Serial.println(F("DFR1322 UART AIN0"));
  if (adc.begin() != 0) {
    Serial.println(F("begin failed: GPIO2 high, address 0x32, UART wiring"));
    while (1) {
      delay(1000);
    }
  }
  if (adc.setInputMode(DFRobot_ADS122C14::SINGLE_ENDED) != 0) {
    Serial.println(F("setInputMode failed"));
    while (1) {
      delay(1000);
    }
  }
  Serial.print(F("FW 0x"));
  Serial.println(adc.getFirmwareVersion(), HEX);
}

void loop() {
  const int32_t raw = adc.readSingleData(DFRobot_ADS122C14::eAIN0);
  const float volts = DFRobot_ADS122C14::rawCodeToVoltage(raw);
  Serial.print(F("AIN0 raw="));
  Serial.print(raw);
  Serial.print(F(" V="));
  Serial.println(volts, 3);
  delay(500);
}
