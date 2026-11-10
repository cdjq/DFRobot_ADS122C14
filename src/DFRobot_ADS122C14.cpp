/*!
 * @file DFRobot_ADS122C14.cpp
 * @brief DFR1322 host library implementation.
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (https://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-08
 * @url https://github.com/DFRobot/DFRobot_ADS122C14
 */
#include "DFRobot_ADS122C14.h"

DFRobot_ADS122C14::DFRobot_ADS122C14(uint8_t i2cAddr, TwoWire *pWire)
  : _pWire(pWire), _addr(i2cAddr), _useI2c(true) {}

DFRobot_ADS122C14::DFRobot_ADS122C14(uint8_t deviceAddr, Stream *s)
  : DFRobot_RTU(s), _pWire(nullptr), _addr(deviceAddr), _useI2c(false) {}

bool DFRobot_ADS122C14::i2cReadReg(uint16_t reg, uint8_t *data, size_t len) {
  if (_pWire == nullptr || data == nullptr) {
    return false;
  }
  if (reg > 0x7F) {
    return false;
  }
  _pWire->beginTransmission(_addr);
  _pWire->write((uint8_t)reg);
  if (_pWire->endTransmission(false) != 0) {
    return false;
  }
  if (_pWire->requestFrom((int)_addr, (int)len) != (int)len) {
    return false;
  }
  for (size_t i = 0; i < len; ++i) {
    data[i] = (uint8_t)_pWire->read();
  }
  return true;
}

bool DFRobot_ADS122C14::i2cWriteHolding(uint16_t reg, uint16_t value) {
  if (_pWire == nullptr) {
    return false;
  }
  if (reg > 0x7F) {
    return false;
  }
  _pWire->beginTransmission(_addr);
  _pWire->write((uint8_t)(0x80u + reg));
  _pWire->write((uint8_t)(value >> 8));
  _pWire->write((uint8_t)(value & 0xFF));
  return _pWire->endTransmission() == 0;
}

bool DFRobot_ADS122C14::readInputU16(uint16_t reg, uint16_t &value) {
  uint8_t buf[2];
  if (_useI2c) {
    if (!i2cReadReg(reg, buf, 2)) {
      return false;
    }
  } else {
    value = readInputRegister(_addr, reg);
    return true;
  }
  value = ((uint16_t)buf[0] << 8) | buf[1];
  return true;
}

bool DFRobot_ADS122C14::readInputBlock(uint16_t startReg, uint16_t count, uint16_t *out) {
  if (out == nullptr || count == 0) {
    return false;
  }
  if (_useI2c) {
    uint8_t buf[32];
    if (count > 16) {
      return false;
    }
    if (!i2cReadReg(startReg, buf, (size_t)count * 2)) {
      return false;
    }
    for (uint16_t i = 0; i < count; ++i) {
      out[i] = ((uint16_t)buf[i * 2] << 8) | buf[i * 2 + 1];
    }
    return true;
  }
  return readInputRegister(_addr, startReg, out, count) == 0;
}

bool DFRobot_ADS122C14::writeHoldingU16(uint16_t reg, uint16_t value) {
  if (_useI2c) {
    return i2cWriteHolding(reg, value);
  }
  return writeHoldingRegister(_addr, reg, value) == 0;
}

int32_t DFRobot_ADS122C14::combineAdcWords(uint16_t hi, uint16_t lo) {
  const uint32_t u = ((uint32_t)hi << 16) | (uint32_t)lo;
  return (int32_t)u;
}

int8_t DFRobot_ADS122C14::begin(void) {
  delay(500);
  setTimeoutTimeMs(1000);
  uint16_t pid = 0;
  uint16_t vid = 0;
  if (!readInputU16(REG_PID, pid) || !readInputU16(REG_VID, vid)) {
    return -1;
  }
  if (pid != DEVICE_PID || vid != DEVICE_VID) {
    return -1;
  }
  return 0;
}

int8_t DFRobot_ADS122C14::setInputMode(uint8_t mode) {
  if (mode > DIFFERENTIAL) {
    return -1;
  }
  return writeHoldingU16(REG_INPUT_MODE, mode) ? 0 : -1;
}

int32_t DFRobot_ADS122C14::readSingleData(uint8_t channel) {
  const uint16_t reg = (uint16_t)(REG_AIN0_HIGH + (uint16_t)channel * 2);
  uint16_t words[2];
  if (!readInputBlock(reg, 2, words)) {
    return 0;
  }
  return combineAdcWords(words[0], words[1]);
}

int8_t DFRobot_ADS122C14::readAllData(float *data, uint8_t length) {
  if (data == nullptr) {
    return -1;
  }
  uint16_t countReg = 0;
  if (!readInputU16(REG_CHANNEL_COUNT, countReg)) {
    return -1;
  }
  const uint8_t active = (countReg > 4) ? 8 : 4;
  if (length < active) {
    return -1;
  }
  uint16_t regs[16];
  if (!readInputBlock(REG_AIN0_HIGH, (uint16_t)active * 2, regs)) {
    return -1;
  }
  for (uint8_t i = 0; i < active; ++i) {
    data[i] = (float)combineAdcWords(regs[i * 2], regs[i * 2 + 1]);
  }
  for (uint8_t i = active; i < length; ++i) {
    data[i] = 0.0f;
  }
  return 0;
}

uint16_t DFRobot_ADS122C14::getFirmwareVersion(void) {
  uint16_t ver = 0;
  readInputU16(REG_VERSION, ver);
  return ver;
}

float DFRobot_ADS122C14::rawCodeToVoltage(int32_t code) {
  return (float)code / 8388607.0f * (2.5f * (100.0f + 24.9f) / 24.9f);
}
