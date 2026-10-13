# DFRobot_ADS122C14

- [中文版](./README_CN.md)

Arduino host library for the Gravity ADS122C14 8-channel 24-bit ADC module, SKU DFR1322. The host reads firmware registers over Gravity I2C or UART RTU. It does not access the ADS122 chip directly.

## Product Link

[DFR1322](https://www.dfrobot.com/)

## Table of Contents

- [Summary](#summary)
- [Installation](#installation)
- [Methods](#methods)
- [Compatibility](#compatibility)
- [History](#history)
- [Credits](#credits)

## Summary

* Reads 8 single-ended channels or 4 differential pairs from the module firmware cache. The module converts continuously at 500 SPS. A full single-ended update takes a few tens of milliseconds.
* Gravity I2C address 0x3B-0x3E when GPIO2 is low. UART RTU address 0x32 at 115200 8N1 when GPIO2 is high.
* Converts a raw code to terminal voltage with the on-board 100 kΩ / 24.9 kΩ divider and the 2.5 V reference.

## Installation

1. In the Arduino IDE Library Manager, search for `DFRobot_ADS122C14` and install it. This library also needs `DFRobot_RTU`.
2. Or download this repository, unzip it into the Arduino `libraries` folder, then open an example from File > Examples > DFRobot_ADS122C14.

## Methods

```C++
  /**
   * @fn DFRobot_ADS122C14
   * @brief I2C constructor.
   * @param i2cAddr 7-bit address 0x3B-0x3E from the ADDR dial. Default 0x3B.
   * @param pWire TwoWire instance. Call Wire.begin() before begin().
   * @n Use this constructor when module GPIO2 is low.
   */
  DFRobot_ADS122C14(uint8_t i2cAddr = I2C_ADDRESS_DEFAULT, TwoWire *pWire = &Wire);

  /**
   * @fn DFRobot_ADS122C14
   * @brief UART RTU constructor.
   * @param deviceAddr Modbus unit ID. Default 0x32.
   * @param s Stream connected to the module Gravity port. On UNO this is Serial (D0/D1).
   * @n Use this constructor when module GPIO2 is high. The module stays at 115200 8N1.
   */
  DFRobot_ADS122C14(uint8_t deviceAddr, Stream *s);

  /**
   * @fn begin
   * @brief Verify PID and VID on the module firmware registers.
   * @return 0 on success, -1 on failure.
   * @n Use the I2C constructor when module GPIO2 is low, and the UART constructor when GPIO2 is high.
   * @n I2C address is 0x3B-0x3E from the ADDR dial. UART is 115200 8N1, unit ID 0x32.
   * @n Call Wire.begin() before begin() on the I2C object. A non-zero return means the bus, address, or mode strap does not match.
   */
  int8_t begin(void);

  /**
   * @fn setInputMode
   * @brief Set single-ended or differential input mode.
   * @param mode SINGLE_ENDED (0) or DIFFERENTIAL (1).
   * @return 0 on success, -1 on a transport error or a rejected value.
   * @n SINGLE_ENDED and DIFFERENTIAL are mutually exclusive.
   * @n After a successful write, wait for one conversion round before trusting every channel. A single-ended round takes a few tens of milliseconds. Power-off restores single-ended mode.
   * @n SINGLE_ENDED reads 8 channels. DIFFERENTIAL reads 4 pairs and the last 4 channels read as 0.
   */
  int8_t setInputMode(uint8_t mode);

  /**
   * @fn readSingleData
   * @brief Read one channel raw code from the firmware cache.
   * @param channel eAIN0-eAIN7 in single-ended mode, or eDIFF0-eDIFF3 in differential mode.
   * @return Sign-extended int32 code. 0 when the read fails.
   * @n eAIN0 through eAIN7 match the silkscreen. eDIFF0 is AIN0-AIN1.
   * @n This reads the module cache. The module converts continuously at 500 SPS and refreshes one channel at a time. A full single-ended round takes a few tens of milliseconds.
   */
  int32_t readSingleData(uint8_t channel);

  /**
   * @fn readAllData
   * @brief Read every active channel raw code into data.
   * @param data Destination buffer. Values are raw codes stored as float, not volts.
   * @param length Buffer length. Use 8 for single-ended or 4 for differential.
   * @return 0 on success, -1 when the buffer is short or the read fails.
   * @n In differential mode only the first 4 elements are valid. Extra elements are set to 0.
   * @n Element 0 is eAIN0 in single-ended mode, or eDIFF0 (AIN0-AIN1) in differential mode.
   */
  int8_t readAllData(float *data, uint8_t length);

  /**
   * @fn getFirmwareVersion
   * @brief Read the firmware version register.
   * @return Input register 0x0005. 0x0100 means V1.0.0. 0 when the read fails.
   * @n The high byte is the major version and the low byte is the minor version.
   */
  uint16_t getFirmwareVersion(void);

  /**
   * @fn rawCodeToVoltage
   * @brief Convert a raw code to terminal voltage.
   * @param code Sign-extended 24-bit code from readSingleData or readAllData.
   * @return Volts at the module terminal.
   * @n V = code / 8388607 * 2.5 * (100 + 24.9) / 24.9. The same scale applies to a differential code.
   * @n USB debug lines use this formula and show 3 decimal places. A code of 3339555 is about 4.992 V.
   */
  static float rawCodeToVoltage(int32_t code);
```

## Compatibility

| MCU            | Work Well | Work Wrong | Not Tested | Remarks |
| -------------- | :-------: | :--------: | :--------: | ------- |
| Arduino UNO    |           |            |     √      |         |
| Arduino Mega   |           |            |     √      |         |
| Arduino Leonardo |         |            |     √      |         |
| ESP32          |           |            |     √      |         |
| ESP8266        |           |            |     √      |         |
| RP2040         |           |            |     √      |         |

## History

- 2026/10/08 - V1.0.0 version

## Credits

Written by wxzed(xiao.wu@dfrobot.com), 2026. (Welcome to our [website](https://www.dfrobot.com/))
