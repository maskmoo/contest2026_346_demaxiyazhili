# GD32VW553H-EVAL 开发板

## 概述

GD32VW553H-EVAL 是基于 GD32VW553H 微控制器的评估板，集成了 WiFi 和 BLE 功能，适用于物联网和无线通信应用开发。

## 硬件资源

### 核心配置
- **MCU**: GD32VW553H (RISC-V, 32-bit)
- **RAM**: 294400 bytes (起始地址: 0x20000200)
- **Flash**: 支持 LittleFS 文件系统

### 通信接口

#### USART0
| 接口 | 引脚 | 功能 | 说明 |
|------|------|------|------|
| USART0_TX | PB15 | 发送 | 串口控制台 |
| USART0_RX | PA8 | 接收 | 串口控制台 |

#### I2C
| 接口 | 引脚 | 功能 | 说明 |
|------|------|------|------|
| I2C0_SCL | PA2 | 时钟线 | 始终可用，无冲突 |
| I2C0_SDA | PA3 | 数据线 | 始终可用，无冲突 |
| I2C1_SCL | PB12 | 时钟线 | 与 LCD_RESET 冲突 |
| I2C1_SDA | PB13 | 数据线 | 与 LCD_D/C 冲突 |

### 模拟接口

#### ADC
| 通道 | 引脚 | 说明 |
|------|------|------|
| ADC_IN0 | PA1 | 模拟输入通道 0 |

### 人机交互

#### LED
| LED | 引脚 | 说明 |
|-----|------|------|
| LED1 | PA4 | 指示灯1 |
| LED2 | PA5 | 指示灯2 |
| LED3 | PA6 | 指示灯3 |

#### 按键
| 按键 | 引脚 | 说明 |
|------|------|------|
| K1 | RESET | 复位按键 |
| K2 | PA0 | 唤醒按键 (Wakeup) |

### 存储

#### QSPI Flash
| 信号 | 引脚 | 说明 |
|------|------|------|
| QSPI_SCK | PA4 | 时钟 |
| QSPI_NSS | PA5 | 片选 |
| QSPI_IO0 | PA6 | 数据线0 |
| QSPI_IO1 | PA7 | 数据线1 |
| QSPI_IO2 | PB3 | 数据线2 |
| QSPI_IO3 | PB4 | 数据线3 |

### 显示

#### SPI LCD
| 信号 | 引脚 | 说明 |
|------|------|------|
| SPI_NSS | PA12 | 片选 |
| SPI_MOSI | PA9 | 主出从入 |
| SPI_SCK | PA11 | 时钟 |
| SPI_MISO | PA10 | 主入从出 |
| LCD_RESET | PB12 | LCD复位 |
| LCD_D/C | PB13 | 数据/命令选择 |

### 红外

#### IRFP
| 信号 | 引脚 | 说明 |
|------|------|------|
| IR_OUT | PB15 | 红外输出 (与USART0_TX共用) |
| TIMER_CH2 | PB11 | 定时器通道 |

## 引脚复用说明

### 冲突引脚

| 引脚 | 功能1 | 功能2 | Kconfig 配置 |
|------|-------|-------|--------------|
| PA4 | LED1 | QSPI_SCK | BOARD_LED_ENABLE / GD32VW55X_QSPI |
| PA5 | LED2 | QSPI_NSS | BOARD_LED_ENABLE / GD32VW55X_QSPI |
| PA6 | LED3 | QSPI_IO0 | BOARD_LED_ENABLE / GD32VW55X_QSPI |
| PB15 | USART0_TX | IR_OUT | BOARD_USART0_CONSOLE / BOARD_IR_OUTPUT_ENABLE |
| PB12 | LCD_RESET | I2C1_SCL | BOARD_LCD_ENABLE / BOARD_I2C1_ENABLE |
| PB13 | LCD_D/C | I2C1_SDA | BOARD_LCD_ENABLE / BOARD_I2C1_ENABLE |

**无冲突接口**：
- I2C0: PA2 (SCL) / PA3 (SDA) - 始终可用

### Kconfig 配置选项

#### LED 与 QSPI Flash 冲突
```kconfig
# 启用 LED（禁用 QSPI Flash）
CONFIG_BOARD_LED_ENABLE=y

# 启用 QSPI Flash（禁用 LED）
CONFIG_GD32VW55X_QSPI=y
```

#### USART 与 IR 输出冲突
```kconfig
# 启用 USART 控制台（禁用 IR 输出）
CONFIG_BOARD_USART0_CONSOLE=y

# 启用 IR 输出（禁用 USART 控制台）
CONFIG_BOARD_IR_OUTPUT_ENABLE=y
```

#### LCD 与 I2C1 冲突
```kconfig
# 启用 LCD（禁用 I2C1）
CONFIG_BOARD_LCD_ENABLE=y

# 启用 I2C1（禁用 LCD）
CONFIG_BOARD_I2C1_ENABLE=y

# I2C0 (PA2/PA3) 始终可用，无需配置
```

### 推荐配置

1. **基础串口调试**: 使用USART (PB15/PA8)，启用 `BOARD_USART0_CONSOLE`
2. **LED控制**: 使用PA4/PA5/PA6，启用 `BOARD_LED_ENABLE`，禁用 QSPI
3. **QSPI Flash**: 使用PA4-PA7/PB3/PB4，启用 `GD32VW55X_QSPI`，禁用 LED
4. **LCD显示**: 使用SPI (PA9-PA12) + LCD控制 (PB12/PB13)，启用 `BOARD_LCD_ENABLE`
5. **传感器开发**: 使用I2C0 (PA2/PA3)，始终可用，无冲突
6. **红外通信**: 使用PB15 (IR_OUT) + PB11 (TIMER_CH2)，启用 `BOARD_IR_OUTPUT_ENABLE`，禁用 USART
7. **I2C1扩展**: 使用PB12/PB13，启用 `BOARD_I2C1_ENABLE`，禁用 LCD

## 配置说明

### 配置目录结构

```
configs/
├── common/          # 公共配置
├── nsh/             # 基础 NSH Shell
├── adc/             # ADC 示例
├── ble/             # BLE 蓝牙
├── e2prom/          # E2PROM I2C 实验
├── littlefs/        # LittleFS 文件系统
├── ostest/          # 操作系统测试
├── periph/          # 外设测试
├── pwm/             # PWM 示例
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
  - USART 串口控制台 (PB15/PA8)
  - Hello World 示例
  - 基础文件系统支持 (procfs)

#### adc - ADC 示例
- **功能**: 演示 ADC 模数转换
- **硬件**: ADC_IN0 (PA1)
- **特性**:
  - 软件触发采样
  - 20 次采样示例

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
  - ADC, I2C, SPI, USART
  - PWM, 定时器
  - DMA, CRC, RNG
  - GPIO 中断, 看门狗

#### pwm - PWM 示例
- **功能**: PWM 脉宽调制输出
- **硬件**: 定时器通道

#### e2prom - E2PROM 实验
- **功能**: E2PROM 读写实验
- **硬件**: I2C0 (PA2/PA3)
- **特性**:
  - I2C 轮询模式
  - I2C 工具 (i2ctool)
  - 用于 AT24xx 等 E2PROM 芯片

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

## 开发建议

1. **首次使用**: 建议从 `nsh` 配置开始，验证基本功能
2. **外设测试**: 使用 `periph` 配置全面测试板载外设
3. **无线开发**: 使用 `wapi` 或 `sta_softap` 配置进行 WiFi 开发
4. **存储应用**: 使用 `littlefs` 配置进行文件系统开发
5. **E2PROM实验**: 使用 `e2prom` 配置进行 I2C E2PROM 读写

## 注意事项

1. 部分外设共用引脚，使用时需注意配置冲突
2. LED (PA4/PA5/PA6) 与 QSPI Flash 共用引脚，不能同时使用
3. USART0_TX (PB15) 与 IR_OUT 共用引脚，需根据功能选择
4. I2C0 (PA2/PA3) 可用于传感器连接
5. LCD 使用独立的 SPI 接口 (PA9/PA10/PA11/PA12)，不与其他外设冲突
