# NuttX/openvela defconfig 配置管理技能

## 描述
管理 NuttX/openvela 的 defconfig 配置集，包括公共配置抽取、功能配置设计、资源冲突处理、配置验证。

## 适用场景
- 为新板卡设计 defconfig 配置集
- 从多个配置中抽取公共部分
- 处理板级资源冲突（引脚复用）
- 排查配置相关编译/运行问题

## 配置层级设计

### 目录结构
```
configs/
├── common/defconfig          # 公共基线配置
├── nsh/defconfig             # 最小 NSH Shell
├── periph/defconfig          # 外设综合测试
├── lcd/defconfig             # LCD 显示
├── ble/defconfig             # BLE 蓝牙
├── sta_softap/defconfig      # WiFi STA+SoftAP
├── wapi/defconfig            # WiFi API 测试
├── adc/defconfig             # ADC 采集
├── pwm/defconfig             # PWM 输出
├── e2prom/defconfig          # EEPROM 读写
├── littlefs/defconfig        # LittleFS 文件系统
└── ostest/defconfig          # OS 功能测试
```

### 继承机制
各功能配置通过 `#include` 继承公共配置：
```ini
# configs/lcd/defconfig
#include "../common/defconfig"

# LCD 特有配置
CONFIG_SPI_DRIVER=y
CONFIG_LCD=y
CONFIG_LCD_ILI9341=y
CONFIG_LCD_FRAMEBUFFER=y
CONFIG_BOARD_LCD_ENABLE=y
```

## 公共配置抽取流程

### 步骤 1：收集所有 defconfig
```bash
for f in configs/*/defconfig; do
    echo "=== $(basename $(dirname $f)) ==="
    grep "^CONFIG_" $f | sort
done
```

### 步骤 2：找出交集（公共配置）
```bash
# 提取所有配置的 CONFIG 项
for f in configs/*/defconfig; do
    grep "^CONFIG_" $f
done | sort | uniq -c | sort -rn | grep "^  *$(ls -d configs/*/defconfig | wc -l)"
# 全部配置都有的就是公共部分
```

### 步骤 3：抽取到 common/defconfig
公共配置通常包括：
```ini
# 架构和芯片
CONFIG_ARCH="risc-v"
CONFIG_ARCH_CHIP="gd32vw55x"
CONFIG_ARCH_CHIP_GD32VW553HM=y

# 串口和 NSH
CONFIG_USART0_CONSOLE=y
CONFIG_NSH_READLINE=y
CONFIG_NSH_ARCHINIT=y

# 基础文件系统
CONFIG_FS_PROCFS=y

# 调试
CONFIG_DEBUG_FEATURES=y
CONFIG_DEBUG_ERROR=y

# 板级初始化
CONFIG_BOARD_LATE_INITIALIZE=y
```

## 资源冲突管理

### 在 board Kconfig 中定义冲突关系
```kconfig
# LED 与 QSPI 冲突（共用 PA4/PA5/PA6）
config BOARD_LED_ENABLE
    bool "Enable board LEDs"
    default n
    depends on !CONFIG_GD32VW55X_QSPI

# LCD 与 I2C1 冲突（LCD 用 PB12/PB13，I2C1 也用）
config BOARD_I2C1_ENABLE
    bool "Enable I2C1"
    default n
    depends on !BOARD_LCD_ENABLE

# IR 与 USART0 冲突（都用 PB15）
config BOARD_IR_OUTPUT_ENABLE
    bool "Enable IR output"
    default n
    depends on !BOARD_USART_CONSOLE
```

### defconfig 中的互斥配置
```ini
# lcd/defconfig - 启用 LCD，禁用 I2C1
CONFIG_BOARD_LCD_ENABLE=y
# CONFIG_BOARD_I2C1_ENABLE is not set

# e2prom/defconfig - 启用 I2C1，禁用 LCD
CONFIG_BOARD_I2C1_ENABLE=y
# CONFIG_BOARD_LCD_ENABLE is not set
```

## 常见问题排查

### 1. 编译报错 "undeclared"（引脚/宏未定义）
**原因**：某个 defconfig 启用了外设但缺少对应的引脚定义
**解决**：
- 检查 board.h 中是否有 `#ifndef` 保护的默认定义
- 检查 Kconfig 中是否有对应的 `select` 依赖

### 2. 配置修改后 menuconfig 不生效
```bash
# 重新生成 .config
./build.sh <config> menuconfig
# 保存回 defconfig
./build.sh <config> savedefconfig
```

### 3. common 配置变更影响所有功能
**排查**：确认变更是否真的需要公共，还是只需要某个功能配置
**建议**：先在功能 defconfig 中验证，确认通用后再提升到 common

### 4. WiFi/BLE 配置依赖
WiFi 和 BLE 有复杂的依赖链：
```ini
# BLE 依赖 WiFi（共享射频平台）
CONFIG_GD32VW55X_BLE=y
CONFIG_GD32VW55X_WIFI=y          # BLE 需要 WiFi 先初始化
CONFIG_EXPERIMENTAL=y             # BLE 实验性功能
CONFIG_GD32VW55X_WIFI_SDK_PATH="/path/to/SDK"
```

## 验证命令
```bash
# 查看当前配置
nsh> cat /proc/config

# 检查配置一致性
diff <(grep "^CONFIG_" configs/nsh/defconfig | sort) \
     <(grep "^CONFIG_" nuttx/.config | sort)
```
