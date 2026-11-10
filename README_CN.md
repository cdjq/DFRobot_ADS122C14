# DFRobot_ADS122C14

- [English Version](./README.md)

Gravity ADS122C14 八通道 24 位 ADC 模块（SKU DFR1322）的 Arduino 主机库。主机通过 Gravity I2C 或 UART RTU 读取固件寄存器，不直接访问 ADS122 芯片。

## 产品链接

[DFR1322](https://www.dfrobot.com/)

## 目录

- [概述](#概述)
- [库安装](#库安装)
- [方法](#方法)
- [兼容性](#兼容性)
- [历史](#历史)
- [创作者](#创作者)

## 概述

* 从模块固件缓存读取 8 路单端或 4 路差分。模块以 500 SPS 连续转换，单端一轮大约几十毫秒。
* GPIO2 为低时走 Gravity I2C，地址 0x3B–0x3E。GPIO2 为高时走 UART RTU，地址 0x32，115200 8N1。
* 用板上 100 kΩ / 24.9 kΩ 分压和 2.5 V 基准，把原始码换算成端子电压。

## 库安装

1. 在 Arduino IDE 的库管理器中搜索 `DFRobot_ADS122C14` 并安装。本库还依赖 `DFRobot_RTU`。
2. 或者下载本仓库，解压到 Arduino 的 `libraries` 目录，再从 文件 > 示例 > DFRobot_ADS122C14 打开示例。

## 方法

```C++
  /**
   * @fn DFRobot_ADS122C14
   * @brief I2C 构造。
   * @param i2cAddr 地址拨码决定的 7 位地址 0x3B-0x3E，默认 0x3B。
   * @param pWire TwoWire 实例。调用 begin() 之前先 Wire.begin()。
   * @n 模块 GPIO2 为低时使用这个构造。
   */
  DFRobot_ADS122C14(uint8_t i2cAddr = I2C_ADDRESS_DEFAULT, TwoWire *pWire = &Wire);

  /**
   * @fn DFRobot_ADS122C14
   * @brief UART RTU 构造。
   * @param deviceAddr Modbus 从站地址，默认 0x32。
   * @param s 接到模块 Gravity 口的 Stream。UNO 上是 Serial（D0/D1）。
   * @n 模块 GPIO2 为高时使用这个构造。模块保持 115200 8N1。
   */
  DFRobot_ADS122C14(uint8_t deviceAddr, Stream *s);

  /**
   * @fn begin
   * @brief 核对模块固件寄存器中的 PID 和 VID。
   * @return 成功返回 0，失败返回 -1。
   * @n GPIO2 为低时用 I2C 构造，为高时用 UART 构造。
   * @n I2C 地址由拨码决定，范围 0x3B-0x3E。UART 为 115200 8N1，从站 0x32。
   * @n I2C 对象要先 Wire.begin() 再 begin()。返回非 0 表示总线、地址或模式拨码不一致。
   */
  int8_t begin(void);

  /**
   * @fn setInputMode
   * @brief 设置单端或差分输入模式。
   * @param mode SINGLE_ENDED (0) 或 DIFFERENTIAL (1)。
   * @return 成功返回 0。传输失败或数值被拒绝时返回 -1。
   * @n SINGLE_ENDED 与 DIFFERENTIAL 互斥。
   * @n 写成功后，等一轮转换再相信全部通道。单端一轮大约几十毫秒。掉电恢复为单端。
   * @n 单端读 8 路。差分读 4 对，后 4 路为 0。
   */
  int8_t setInputMode(uint8_t mode);

  /**
   * @fn readSingleData
   * @brief 从固件缓存读一路原始码。
   * @param channel 单端时为 eAIN0-eAIN7，差分时为 eDIFF0-eDIFF3。
   * @return 符号扩展的 int32 码值。读取失败时为 0。
   * @n eAIN0 到 eAIN7 与丝印 AIN0 到 AIN7 一致。eDIFF0 是 AIN0-AIN1。
   * @n 这里读的是模块缓存。模块以 500 SPS 连续转换，每次更新一路。单端转完一轮大约几十毫秒。
   */
  int32_t readSingleData(uint8_t channel);

  /**
   * @fn readAllData
   * @brief 把当前有效通道的原始码读入 data。
   * @param data 目标缓冲区。元素是用 float 存放的原始码，不是电压。
   * @param length 缓冲区长度。单端用 8，差分用 4。
   * @return 成功返回 0。缓冲区太短或读取失败时返回 -1。
   * @n 差分模式下只有前 4 个元素有效，多出来的元素写成 0。
   * @n 单端时元素 0 是 eAIN0。差分时元素 0 是 eDIFF0（AIN0-AIN1）。
   */
  int8_t readAllData(float *data, uint8_t length);

  /**
   * @fn getFirmwareVersion
   * @brief 读取固件版本寄存器。
   * @return 输入寄存器 0x0003。0x0920 表示 V0.9.2.0。读取失败时为 0。
   * @n 每个十六进制位是一段。0x1000 表示 V1.0.0.0。
   */
  uint16_t getFirmwareVersion(void);

  /**
   * @fn rawCodeToVoltage
   * @brief 把原始码换算成端子电压。
   * @param code readSingleData 或 readAllData 得到的符号扩展 24 位码。
   * @return 模块端子上的电压，单位 V。
   * @n V = code / 8388607 * 2.5 * (100 + 24.9) / 24.9。差分码用同一比例。
   * @n USB 调试行使用这个公式，并显示 3 位小数。码值 3339555 约为 4.992 V。
   */
  static float rawCodeToVoltage(int32_t code);
```

## 兼容性

| 主板           | 通过 | 未通过 | 未测试 | 备注 |
| -------------- | :--: | :----: | :----: | ---- |
| Arduino UNO    |      |        |   √    |      |
| Arduino Mega   |      |        |   √    |      |
| Arduino Leonardo |    |        |   √    |      |
| ESP32          |      |        |   √    |      |
| ESP8266        |      |        |   √    |      |
| RP2040         |      |        |   √    |      |

## 历史

- 2026/10/08 - V1.0.0 版本

## 创作者

Written by wxzed(xiao.wu@dfrobot.com), 2026. (欢迎访问我们的[网站](https://www.dfrobot.com/))
