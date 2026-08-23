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

## NSH 测试方式

编译指定配置：

```bash
cd <openvela-workspace>
./build.sh contest2026_346_demaxiyazhili/boards/gd32vw553x/gd32vw553h-eval/configs/<config> -j8
```

烧录后在串口 NSH 终端中执行以下命令验证各功能。

### nsh — 最小 NSH 基线

无额外外设驱动，仅控制台 + procfs。

```nsh
nsh> hello              # 运行 hello 示例
nsh> ps                 # 查看任务列表
nsh> free               # 查看内存使用
nsh> ls /proc           # 查看 procfs
nsh> cat /proc/uptime   # 系统运行时间
nsh> cat /proc/version  # NuttX 版本
nsh> reset              # 复位板子
```

### adc — ADC 模拟采样

外设：ADC0（PA1 引脚），设备节点 `/dev/adc0`。

```nsh
nsh> adc                # 运行 ADC 示例，连续采样 20 次并打印原始值
```

预期输出：20 行 ADC 原始计数值（软件触发，通道 0）。

### pwm — PWM 脉冲输出

外设：TIMER1 PWM，设备节点 `/dev/pwm0`。

```nsh
nsh> pwm -p 50 -f 1000 -t 5   # 50% 占空比，1kHz 频率，持续 5 秒
```

预期行为：PWM 引脚输出 1kHz 方波，可用示波器或 LED 观察。

### e2prom — I2C EEPROM 读写

外设：I2C0（PA2/PA3），设备节点 `/dev/i2c0`。

```nsh
nsh> i2c dev 0x50 0x50         # 探测 I2C 总线上的设备（EEPROM 通常在 0x50）
nsh> i2c get -a 0x50 -r 0x00 -n 16   # 从地址 0x50 的寄存器 0x00 读取 16 字节
nsh> i2c set -a 0x50 -r 0x00 -d 0xAB # 向寄存器 0x00 写入 0xAB
```

预期输出：`i2c dev` 列出 ACK 的地址；`i2c get` 打印读取到的字节。

### lcd — LCD 显示（ILI9341）

外设：SPI0 驱动 ILI9341（320×240，RGB565 横屏），设备节点 `/dev/lcd0`、`/dev/fb0`。

```nsh
nsh> fb                 # 运行 framebuffer 示例，在 LCD 上绘制测试图案
```

预期行为：LCD 屏幕显示彩色条纹/渐变测试图案。

### lvgl — LVGL 图形界面

外设：同 lcd（ILI9341 + framebuffer），额外集成 LVGL 图形库。

```nsh
nsh> lvgl_demo          # 运行 LVGL 控件演示
```

预期行为：LCD 上渲染交互式 UI 控件（按钮、滑块、复选框、图表等），左上角显示 FPS 性能计数器。

### littlefs — LittleFS 文件系统 + 用户 LED

外设：内部 Flash MTD（`/dev/gd32flash`），挂载 LittleFS 到 `/data`；3 个用户 LED（PA4/PA5/PA6），设备节点 `/dev/userleds`。

```nsh
nsh> mount              # 查看挂载点，应包含 /data (littlefs)
nsh> ls /data/          # 列出 LittleFS 分区内容
nsh> echo "hello" > /data/test.txt   # 写入文件
nsh> cat /data/test.txt # 读回验证
nsh> leds               # 运行 LED 示例，依次闪烁三个板载 LED
nsh> led 0 on           # 点亮 LED 0
nsh> led 1 off          # 熄灭 LED 1
```

预期行为：`/data` 首次启动自动格式化为 LittleFS，文件跨重启持久化；`leds` 命令循环闪烁 LED。

### periph — 全外设综合测试

外设：I2C0、SPI0、ADC0、PWM、Input Capture、内部 Flash MTD、用户 LED、FWDGT 看门狗、WWDGT 看门狗。

```nsh
nsh> ls /dev            # 查看所有设备节点
nsh> i2c dev 0x50 0x50  # 探测 I2C
nsh> leds               # LED 示例
nsh> led 0 on           # 单独控制 LED
nsh> adc                # ADC 采样（需 adc 示例未编译时可用 cat /dev/adc0）
nsh> cat /dev/watchdog0 # 打开独立看门狗（开始倒计时，需及时喂狗）
```

设备节点一览：`/dev/i2c0`、`/dev/spi0`、`/dev/adc0`、`/dev/pwm0`、`/dev/capture0`、`/dev/gd32flash`、`/dev/userleds`、`/dev/watchdog0`、`/dev/watchdog1`。

### wapi — WiFi Station 模式

外设：WiFi（wlan0 网络接口），WAPI 命令行工具。

```nsh
nsh> wapi show wlan0                          # 查看接口当前状态
nsh> wapi scan wlan0                          # 扫描 WiFi 网络
nsh> wapi scan_results wlan0                  # 查看扫描结果
nsh> wapi mode wlan0 2                        # 设置为 Managed 模式 (Station)
nsh> wapi psk wlan0 <密码> 3 2                # 设置密码 (3=CCMP, 2=WPA2)
nsh> dhcpd_start wlan0                        # 启动 DHCP 服务（必须在连接前执行）
nsh> wapi essid wlan0 <SSID> 1                # 连接 WiFi（等待日志出现 4-way handshake 完成）
nsh> renew wlan0                              # 触发 NuttX DHCP 客户端，获取 IP/网关/DNS
nsh> ifconfig                                 # 确认 IP、网关、子网掩码
nsh> ping 8.8.8.8                             # 测试网络连通性
nsh> wapi disconnect wlan0                    # 断开连接
nsh> wapi power_save wlan0 off                # 关闭省电模式（可选，提升性能）
```

> **重要**：WiFi SDK 移植到 NuttX 时，SDK 内部的 DHCP 客户端已被禁用（`net_dhcp_start()` 为空操作）。WiFi 连接后需手动执行 `renew wlan0` 触发 NuttX 的 DHCP 客户端。但直接 `renew` 可能失败，需要先执行 `dhcpd_start wlan0` 初始化网络接口，之后 `renew` 才能正常从 AP 的 DHCP 服务器获取 IP。
>
> 注意：`wapi essid` 连接后 SDK 会报 `IPv4 addr got x.x.x.x`，这**不是**真正从 AP DHCP 获取的地址，需以 `renew` 后 `ifconfig` 显示的 IP 为准。

预期行为：`dhcpd_start` + `wapi essid` 连接 + `renew` 后，从 DHCP 服务器获取真实 IP（如小米热点的 `192.168.55.x` 网段），ping 可达外部地址。

**wapi psk 参数说明：**
- `<passphrase>` — WiFi 密码
- `<index/flag>` — 密码算法：`0`=NONE, `1`=WEP, `2`=TKIP, `3`=CCMP
- `[wpa]` — WPA 版本：`0`=NONE, `1`=WPA1, `2`=WPA2, `3`=WPA3

**wapi essid 参数说明：**
- `<essid>` — WiFi 网络名称
- `<index/flag>` — `0`=ESSID_OFF, `1`=ESSID_ON, `2`=ESSID_DELAY_ON

### sta_softap — WiFi Station + SoftAP 双模式

外设：WiFi（wlan0），含 DHCP 服务器（SoftAP 模式）。

**Station 模式连接外部 AP：**

```nsh
nsh> wapi show wlan0                          # 查看接口状态
nsh> wapi scan wlan0                          # 扫描网络
nsh> wapi scan_results wlan0                  # 查看扫描结果
nsh> wapi mode wlan0 2                        # Managed 模式 (Station)
nsh> wapi psk wlan0 <密码> 3 2                # 设置密码 (CCMP + WPA2)
nsh> dhcpd_start wlan0                        # 启动 DHCP 服务（必须在连接前执行）
nsh> wapi essid wlan0 <SSID> 1                # 连接 WiFi（等待 4-way handshake 完成）
nsh> renew wlan0                              # 触发 DHCP 获取 IP/网关/DNS
nsh> ifconfig                                 # 确认网络配置
nsh> ping 8.8.8.8                             # 测试连通性
```

**SoftAP 模式开启热点：**

```nsh
nsh> wapi mode wlan0 3                        # Master 模式 (SoftAP)
nsh> wapi essid wlan0 <热点名称> 1             # 设置热点 SSID
nsh> wapi psk wlan0 <密码> 3 2                # 设置热点密码
nsh> wapi ip wlan0 192.168.4.1                # 设置 AP 网关 IP
nsh> dhcpd_start wlan0                        # 启动 DHCP 服务器为客户端分配 IP
```

预期行为：Station 模式连接外部 AP 获取 IP；SoftAP 模式下作为热点为其他设备分配 IP。

### ble — 蓝牙低功耗（BLE）

外设：BLE 控制器 + WiFi（平台/RF 共享），BLE GATT 演示。

```nsh
nsh> wapi show wlan0                          # 查看 WiFi 接口状态（BLE 依赖 WiFi 平台）
nsh> wapi scan wlan0                          # 扫描 WiFi 网络
nsh> dhcpd_start wlan0                        # 如需联网，先启动 DHCP 服务
nsh> renew wlan0                              # 连接 WiFi 后触发 DHCP 获取 IP
nsh> date                                     # 查看/设置 RTC 时间
```

预期行为：BLE 控制器初始化后自动广播 GATT 服务，可用手机 BLE 扫描工具发现设备。WiFi 接口同时可用。

### ostest — OS 内核回归测试

无额外外设，运行 NuttX 标准测试套件。

```nsh
nsh> ostest             # 运行完整 OS 测试（线程、互斥锁、信号量、信号、消息队列、POSIX API 等）
```

预期输出：每个测试用例打印 PASS/FAIL，全部通过即表示 OS 内核功能正常。

---

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
