/*!
 * @file readAllSingleEndedChannels.ino
 * @brief Read AIN0 through AIN7 raw codes and terminal voltages.
 * @n Keep I2C_COMMUNICATION for I2C. Comment that line out for UART.
 * @n V = code / 8388607 * 2.5 * (100 + 24.9) / 24.9. Raw codes are printed in hex.
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

// Use I2C. Comment out the next line to use UART.
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
}

void loop() {
  float data[8];
  if (adc.readAllData(data, 8) == 0) {
    for (uint8_t i = 0; i < 8; ++i) {
      Serial.print(F("AIN"));
      Serial.print(i);
      Serial.print(F(" raw=0x"));
      Serial.print((int32_t)data[i], HEX);
      Serial.print(F(" V="));
      Serial.println(DFRobot_ADS122C14::rawCodeToVoltage((int32_t)data[i]), 3);
    }
  }
  Serial.println(F("---"));
  delay(1000);
}
