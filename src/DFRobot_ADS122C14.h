/*!
 * @file DFRobot_ADS122C14.h
 * @brief DFR1322 Gravity ADS122C14 8-channel ADC module host library (I2C / UART RTU).
 * @details Declares the host class, channel enum, and register access used by examples.
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (https://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-08
 * @url https://github.com/DFRobot/DFRobot_ADS122C14
 */
#ifndef __DFROBOT_ADS122C14_H__
#define __DFROBOT_ADS122C14_H__

#include <Arduino.h>
#include <Wire.h>
#include <DFRobot_RTU.h>

class DFRobot_ADS122C14 : public DFRobot_RTU {
public:
  static const uint8_t SINGLE_ENDED = 0;
  static const uint8_t DIFFERENTIAL = 1;

  static const uint16_t DEVICE_PID = 0x1322;
  static const uint16_t DEVICE_VID = 0x3343;
  static const uint8_t I2C_ADDRESS_DEFAULT = 0x3B;
  static const uint8_t RTU_ADDRESS_DEFAULT = 0x32;

  /**
   * @enum eChannel_t
   * @brief Channel argument for readSingleData.
   * @n eAIN0 through eAIN7 match the silkscreen, AIN0 through AIN7.
   * @n eDIFF0 through eDIFF3 reuse values 0 through 3. eDIFF0 is AIN0-AIN1, eDIFF1 is AIN2-AIN3, eDIFF2 is AIN4-AIN5, eDIFF3 is AIN6-AIN7. Use them only in differential mode.
   */
  typedef enum {
    eAIN0 = 0,
    eAIN1,
    eAIN2,
    eAIN3,
    eAIN4,
    eAIN5,
    eAIN6,
    eAIN7,
    eDIFF0 = 0,
    eDIFF1,
    eDIFF2,
    eDIFF3
  } eChannel_t;

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
   * @return Input register 0x0003. 0 when the read fails.
   * @n Each hex digit is one field. 0x0920 means V0.9.2.0. 0x1000 means V1.0.0.0.
   */
  uint16_t getFirmwareVersion(void);

  /**
   * @fn rawCodeToVoltage
   * @brief Convert a raw code to terminal voltage.
   * @param code Sign-extended 24-bit code from readSingleData or readAllData.
   * @return Volts at the module terminal.
   * @n V = code / 8388607 * 2.5 * (100 + 24.9) / 24.9. The same scale applies to a differential code.
   * @n The module ports return the raw code only. A code of 0x0032F623 is about 4.992 V.
   */
  static float rawCodeToVoltage(int32_t code);

private:
  static const uint16_t REG_VID = 0x0000;
  static const uint16_t REG_PID = 0x0001;
  static const uint16_t REG_HW_VERSION = 0x0002;
  static const uint16_t REG_VERSION = 0x0003;
  static const uint16_t REG_INPUT_MODE = 0x0006;
  static const uint16_t REG_CHANNEL_COUNT = 0x0007;
  static const uint16_t REG_AIN0_HIGH = 0x0008;

  TwoWire *_pWire;
  uint8_t _addr;
  bool _useI2c;

  bool i2cReadReg(uint16_t reg, uint8_t *data, size_t len);
  bool i2cWriteHolding(uint16_t reg, uint16_t value);
  bool readInputU16(uint16_t reg, uint16_t &value);
  bool readInputBlock(uint16_t startReg, uint16_t count, uint16_t *out);
  bool writeHoldingU16(uint16_t reg, uint16_t value);
  static int32_t combineAdcWords(uint16_t hi, uint16_t lo);
};

#endif
