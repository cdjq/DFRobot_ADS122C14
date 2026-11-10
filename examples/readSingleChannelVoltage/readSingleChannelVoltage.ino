/*!
 * @file readSingleChannelVoltage.ino
 * @brief Read silkscreen AIN0 as a raw code and a terminal voltage.
 * @n Keep I2C_COMMUNICATION for I2C. Comment that line out for UART.
 * @n V = code / 8388607 * 2.5 * (100 + 24.9) / 24.9. The raw code is printed in hex.
 * @n I2C: GPIO2 low, address 0x3B. UART: GPIO2 high, address 0x32, 115200 8N1.
 * @n Connect GND to GND. VCC is 3.3 V or 5 V per the module.
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (https://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-08
 * @url https://github.com/DFRobot/DFRobot_ADS122C14
 */
#include <DFRobot_ADS122C14.h>

// 使用 I2C。注释掉下一行则使用 UART。
#define I2C_COMMUNICATION

#ifdef I2C_COMMUNICATION
#include <Wire.h>
DFRobot_ADS122C14 adc(DFRobot_ADS122C14::I2C_ADDRESS_DEFAULT, &Wire);
#else
/*
 * board               | UNO/ESP8266 | Leonardo/Mega | ESP32
 * MCU TX -> module RX | 5           | Serial1 TX    | GPIO17
 * MCU RX <- module TX | 4           | Serial1 RX    | GPIO16
 */
#if defined(ARDUINO_AVR_UNO) || defined(ESP8266)
#include <SoftwareSerial.h>
SoftwareSerial DFRSerial(4, 5);
#else
#define DFRSerial Serial1
#endif
DFRobot_ADS122C14 adc(DFRobot_ADS122C14::RTU_ADDRESS_DEFAULT, &DFRSerial, 115200);
#endif

void setup() {
  Serial.begin(115200);
  Serial.println(F("DFR1322 readSingleChannelVoltage"));
  if (adc.begin() != 0) {
    Serial.println(F("begin failed: check wiring, GPIO2 mode and address"));
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
  Serial.print(F("AIN0 raw=0x"));
  Serial.print(raw, HEX);
  Serial.print(F(" V="));
  Serial.println(volts, 3);
  delay(500);
}
