# GD32VW553H-EVAL 开发板

## 概述

GD32VW553H-EVAL 是基于 GD32VW553H 微控制器的评估板，集成了 WiFi 和 BLE 功能，适用于物联网和无线通信应用开发。

## 硬件资源

### 核心配置
- **MCU**: GD32VW553H (RISC-V, 32-bit)
- **RAM**: 294400 bytes (起始地址: 0x20000200)
- **Flash**: 支持 LittleFS 文件系统

### 通信接口

#### UART
| 接口 | 引脚 | 功能 | 说明 |
|------|------|------|------|
| UART2_TX | PA6 | 发送 | 默认串口控制台 (GD-Link虚拟串口) |
| UART2_RX | PA7 | 接收 | 默认串口控制台 |
| UART1_TX | PB15 | 发送 | 扩展串口 |
| UART1_RX | PA8 | 接收 | 扩展串口 |
| USART0_TX | PA0 | 发送 | 扩展串口 |
| USART0_RX | PA1 | 接收 | 扩展串口 |

#### I2C
| 接口 | 引脚 | 功能 | 说明 |
|------|------|------|------|
| I2C0_SCL | PA2 | 时钟线 | 默认配置 (无SPI时) |
| I2C0_SDA | PA3 | 数据线 | 默认配置 (无SPI时) |
| I2C0_SCL | PB0 | 时钟线 | SPI启用时的备用配置 |
| I2C0_SDA | PB1 | 数据线 | SPI启用时的备用配置 |
| I2C1_SCL | PB12 | 时钟线 | 扩展I2C |
| I2C1_SDA | PB13 | 数据线 | 扩展I2C |

#### SPI
| 接口 | 引脚 | 功能 | 说明 |
|------|------|------|------|
| SPI_SCK | PA2 | 时钟 | SPI0 |
| SPI_MISO | PA1 | 主入从出 | SPI0 |
| SPI_MOSI | PA0 | 主出从入 | SPI0 |
| SPI0_CS | PA4 | 片选 | SD卡片选 (GPIO输出) |

### 模拟接口

#### ADC
| 通道 | 引脚 | 说明 |
|------|------|------|
| ADC_IN8 | PB0 | 模拟输入通道 8 (无SPI时可用) |

### 人机交互

#### LED
| LED | 引脚 | 说明 |
|-----|------|------|
| LED1 | PC0 | 运行指示灯 |
| LED2 | PC1 | 睡眠指示灯 |
| LED3 | PC2 | 接收指示灯 |

**注意**: LED 为高电平有效，使用推挽输出驱动。

#### 按键
| 按键 | 引脚 | 说明 |
|------|------|------|
| KEY1 | PA0 | 用户按键 (与USART0_TX共用) |

### 存储

#### SPI Flash (QSPI)
| 信号 | 引脚 | 说明 |
|------|------|------|
| QSPI_SCK | PA4 | 时钟 (与SPI_CS共用) |
| QSPI_NSS | PA5 | 片选 |
| QSPI_IO0 | PA6 | 数据线0 (与UART2_TX共用) |
| QSPI_IO1 | PA7 | 数据线1 (与UART2_RX共用) |
| QSPI_IO2 | PB3 | 数据线2 |
| QSPI_IO3 | PB4 | 数据线3 |

### 调试接口

#### JTAG/SWD
| 信号 | 引脚 | 说明 |
|------|------|------|
| JNTRST | PB4 | JTAG复位 (与QSPI_IO3共用) |
| JTDO | PB3 | JTAG数据输出 (与QSPI_IO2共用) |

### 定时器

#### PWM
| 通道 | 引脚 | 说明 |
|------|------|------|
| TIMER1_CH0 | PA0 | PWM输出通道0 (AF1) |

## 配置说明

### 配置目录结构

```
configs/
├── common/          # 公共配置
├── nsh/             # 基础 NSH Shell
├── adc/             # ADC 示例
├── ble/             # BLE 蓝牙
├── littlefs/        # LittleFS 文件系统
├── ostest/          # 操作系统测试
├── periph/          # 外设测试
├── pwm/             # PWM 示例
├── sdcard/          # SD 卡
├── sht3x/           # SHT3x 温湿度传感器
├── sta_softap/      # WiFi STA/SoftAP
└── wapi/            # WAPI 无线
```

### 配置继承机制

所有配置文件通过 `#include "../common/defconfig"` 继承公共配置，避免重复定义。

公共配置包含：
- 架构配置 (RISC-V)
- 芯片型号 (GD32VW553HM)
- 板级配置 (gd32vw553h-eval)

### 配置详情

#### nsh - 基础配置
- **功能**: 提供 NSH Shell 命令行接口
- **特性**: 
  - UART2 串口控制台 (PA6/PA7)
  - Hello World 示例
  - 基础文件系统支持 (procfs)

#### adc - ADC 示例
- **功能**: 演示 ADC 模数转换
- **硬件**: ADC_IN8 (PB0)
- **特性**:
  - 软件触发采样
  - 20 次采样示例
  - **注意**: 需确保SPI未启用，否则PB0被I2C0占用

#### ble - 蓝牙配置
- **功能**: BLE 蓝牙通信
- **特性**:
  - BLE GATT 示例
  - WiFi 支持
  - 网络协议栈

#### littlefs - 文件系统
- **功能**: LittleFS 嵌入式文件系统
- **硬件**: 内部 Flash (PROGMEM)
- **特性**:
  - 程序存储区挂载
  - LED 控制示例

#### ostest - 系统测试
- **功能**: NuttX 操作系统测试
- **特性**:
  - 内核功能测试
  - 任务调度测试

#### periph - 外设测试
- **功能**: 全面测试板载外设
- **支持外设**:
  - ADC, I2C, SPI, UART
  - PWM, 定时器
  - DMA, CRC, RNG
  - GPIO 中断, 看门狗
- **注意**: 启用SPI时，I2C0会切换到备用引脚 (PB0/PB1)

#### pwm - PWM 示例
- **功能**: PWM 脉宽调制输出
- **硬件**: TIMER1_CH0 (PA0)
- **注意**: PA0与USART0_TX和KEY1共用

#### sdcard - SD 卡
- **功能**: SD 卡读写
- **硬件**: SPI0 (PA0/PA1/PA2) + CS (PA4)
- **特性**:
  - FAT 文件系统
  - MMC/SD 支持

#### sht3x - 温湿度传感器
- **功能**: SHT3x 传感器读取
- **硬件**: I2C0 (PA2/PA3)
- **特性**:
  - I2C 轮询模式
  - 传感器数据读取
  - **注意**: 与SPI共用引脚，不能同时使用

#### sta_softap - WiFi 热点
- **功能**: WiFi STA 和 SoftAP 模式
- **特性**:
  - DHCP 服务
  - DNS 客户端
  - 网络管理工具

#### wapi - 无线 API
- **功能**: WiFi 无线配置
- **特性**:
  - WAPI 命令行工具
  - 网络连接管理

## 引脚复用说明

### 冲突引脚

| 引脚 | 功能1 | 功能2 | 功能3 | 说明 |
|------|-------|-------|-------|------|
| PA0 | USART0_TX | KEY1 | PWM(TIMER1_CH0) | 三选一 |
| PA1 | USART0_RX | SPI_MISO | ADC_IN1 | 三选一 |
| PA2 | I2C0_SCL | SPI_SCK | - | 二选一 |
| PA3 | I2C0_SDA | SPI_MOSI | - | 二选一 |
| PA4 | SPI_CS | QSPI_SCK | - | 二选一 |
| PA5 | QSPI_NSS | - | - | 专用 |
| PA6 | UART2_TX | QSPI_IO0 | - | 二选一 |
| PA7 | UART2_RX | QSPI_IO1 | - | 二选一 |
| PB0 | I2C0_SCL(SPI时) | ADC_IN8 | - | 二选一 |
| PB1 | I2C0_SDA(SPI时) | - | - | 专用 |
| PB3 | QSPI_IO2 | JTDO | - | 二选一 |
| PB4 | QSPI_IO3 | JNTRST | - | 二选一 |
| PB12 | I2C1_SCL | - | - | 专用 |
| PB13 | I2C1_SDA | - | - | 专用 |
| PB15 | UART1_TX | - | - | 专用 |
| PA8 | UART1_RX | - | - | 专用 |

### 推荐配置

1. **基础串口调试**: 使用UART2 (PA6/PA7)，无需跳线
2. **传感器开发**: 使用I2C0 (PA2/PA3)，禁用SPI
3. **SD卡存储**: 使用SPI0 (PA0/PA1/PA2)，CS=PA4
4. **无线通信**: 使用WiFi/BLE，无需额外引脚

## 开发建议

1. **首次使用**: 建议从 `nsh` 配置开始，验证基本功能
2. **外设测试**: 使用 `periph` 配置全面测试板载外设
3. **无线开发**: 使用 `wapi` 或 `sta_softap` 配置进行 WiFi 开发
4. **存储应用**: 使用 `littlefs` 或 `sdcard` 配置进行文件系统开发

## 注意事项

1. 部分外设共用引脚，使用时需注意配置冲突
2. UART2 (PA6/PA7) 是默认串口控制台，连接GD-Link虚拟串口
3. SPI和I2C0共用PA2/PA3引脚，不能同时使用
4. 启用SPI时，I2C0会自动切换到备用引脚PB0/PB1
5. LED使用PC0/PC1/PC2，与其他外设无冲突
