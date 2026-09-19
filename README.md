# GD32VW553H-EVAL openvela 适配

## 一、作品简介

本作品将 **openvela 实时操作系统**移植到 **GD32VW553H-EVAL** 开发板上，基于兆易创新 GD32VW553X RISC-V 内核 MCU，实现了包括 WiFi（STA + SoftAP）、BLE 蓝牙、ILI9341 SPI LCD 显示屏、ADC、PWM、I2C、SPI、e2prom、LittleFS 文件系统、看门狗等外设驱动的完整适配。

**亮点：**

- **RISC-V 架构适配**：完成 GD32VW553X 芯片级 HAL 驱动移植，涵盖 GPIO、UART、SPI、I2C、ADC、PWM、DMA、RTC、CRC、看门狗等十余种外设
- **无线连接**：WiFi STA/SoftAP 双模式 + BLE 蓝牙，支持常见无线场景
- **图形显示**：ILI9341 SPI LCD 驱动
- **丰富配置集**：提供 12 个独立 defconfig 配置（nsh、periph、lcd、ble、sta_softap、wapi、adc、pwm、e2prom、littlefs、ostest 等），方便快速验证各子系统

## 二、选题方向

**新硬件适配**

GD32VW553H-EVAL 是一款集 RISC-V 内核、WiFi、BLE 于一体的高集成度开发板，此前 openvela 尚未支持该平台。本作品完成从芯片级到板级的完整 BSP 移植，使 openvela 生态新增一款可用的 RISC-V + 无线连接硬件平台。

## 三、目录结构

```
├── chips/gd32vw55x/                — 芯片级 HAL 驱动（GPIO/UART/SPI/I2C/ADC/PWM/DMA 等）
│   ├── gdwifi/                     — WiFi 底层驱动适配
│   └── gdble/                      — BLE 蓝牙底层驱动适配
├── boards/gd32vw553x/
│   └── gd32vw553h-eval/            — 板级支持包（BSP）
│       ├── src/                    — 板级初始化代码（boot/bringup/LCD/SPI/LED/按键等）
│       ├── include/                — 板级头文件
│       ├── scripts/                — 链接脚本
│       └── configs/                — 12 个 defconfig 配置集
│           ├── nsh/                — 最小 NSH Shell
│           ├── periph/             — 外设综合测试
│           ├── lcd/                — LCD 显示
│           ├── ble/                — BLE 蓝牙
│           ├── sta_softap/         — WiFi STA + SoftAP
│           ├── wapi/               — WiFi API 测试
│           ├── adc/                — ADC 采集
│           ├── pwm/                — PWM 输出
│           ├── e2prom/             — EEPROM 读写
│           ├── littlefs/           — LittleFS 文件系统
│           └── ostest/             — 操作系统功能测试
├── app/hello_app/                  — 示例应用（Hello World）
├── quickapp/                       — 快应用示例
├── doc/                            — 开发文档与移植计划
├── logs/                           — AI Coding 日志
├── contest2026_346_demaxiyazhili.xml — repo manifest（本仓映射）
└── openvela.xml                    — openvela 主 manifest
```

## 四、运行方式

### 1. 拉取完整工程

```bash
repo init -u https://github.com/maskmoo/contest2026_346_demaxiyazhili \
  -b dev-ai-contest-2026 -m contest2026_346_demaxiyazhili.xml
repo sync -c -j8
```

### 2. 编译

进入 openvela 工作区根目录（仓库上一级），使用 `build.sh` 编译：

```bash
cd ..

# 最小 NSH Shell
./build.sh vendor/openvela/boards/gd32vw553x/gd32vw553h-eval/configs/nsh -j8

# LCD 显示
./build.sh vendor/openvela/boards/gd32vw553x/gd32vw553h-eval/configs/lcd -j8

# WiFi STA + SoftAP
./build.sh vendor/openvela/boards/gd32vw553x/gd32vw553h-eval/configs/sta_softap -j8

# BLE 蓝牙
./build.sh vendor/openvela/boards/gd32vw553x/gd32vw553h-eval/configs/ble -j8

# 外设综合测试
./build.sh vendor/openvela/boards/gd32vw553x/gd32vw553h-eval/configs/periph -j8
```

其他配置（adc / pwm / e2prom / littlefs / ostest / wapi）替换 configs 路径即可。

### 3. 烧录与运行

编译产物通过 J-Link / OpenOCD 等调试器烧录到 GD32VW553H-EVAL 开发板，上电后通过串口（115200 baud）进入 NSH Shell 即可交互。

### 4. 如需 menuconfig

```bash
./build.sh vendor/openvela/boards/gd32vw553x/gd32vw553h-eval/configs/nsh menuconfig
```

## 五、AI Coding 使用说明

本作品在开发过程中借助 Claude Code（AI 辅助编程工具）完成多个环节：

- **方案设计**：通过 AI 分析 GD32VW553X 芯片手册与 openvela 现有 BSP 架构，确定移植方案与目录结构
- **编码实现**：AI 辅助生成芯片级 HAL 驱动框架、板级初始化代码、Kconfig/Makefile/CMakeLists.txt 构建文件
- **调试排错**：WiFi 配置构建错误（wapi config）、CMake 适配等问题通过 AI 快速定位与修复
- **文档输出**：README、移植文档等由 AI 协助编写

完整 AI Coding 对话日志见 `logs/` 目录。