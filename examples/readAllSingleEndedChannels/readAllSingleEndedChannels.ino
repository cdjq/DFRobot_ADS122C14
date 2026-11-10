/*!
 * @file readAllSingleEndedChannels.ino
 * @brief Read AIN0 through AIN7 raw codes and terminal voltages.
 * @n V = code / 8388607 * 2.5 * (100 + 24.9) / 24.9. Raw codes are printed in hex.
 * @n Module GPIO2 must be low (I2C). Default address is 0x3B.
 * @n This sketch uses the board's default Wire pins.
 * @n | Board            | SDA | SCL |
 * @n | Arduino UNO      | A4  | A5  |
 * @n | Arduino Mega     | 20  | 21  |
 * @n | Arduino Leonardo | 2   | 3   |
 * @n | ESP32            | 21  | 22  |
 * @n | ESP8266          | D2  | D1  |
 * @n Connect GND to GND. VCC is 3.3 V or 5 V per the module.
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (https://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-08
 * @url https://github.com/DFRobot/DFRobot_ADS122C14
 */
#include <Wire.h>
#include <DFRobot_ADS122C14.h>

DFRobot_ADS122C14 adc(DFRobot_ADS122C14::I2C_ADDRESS_DEFAULT, &Wire);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  if (adc.begin() != 0) {
    Serial.println(F("begin failed: I2C wiring, address dial, GPIO2 low"));
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
