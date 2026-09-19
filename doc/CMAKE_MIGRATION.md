# GD32VW55x CMake 构建适配文档

本文档记录 GD32VW55x 芯片及 GD32VW553H-EVAL 开发板从 Make 构建迁移到 CMake 构建的完整适配过程，
包括 OpenVela CMake 构建框架分析、Make 与 CMake 的对应关系、已发现并修复的问题。

---

## 目录

1. [OpenVela CMake 构建框架概览](#1-openvela-cmake-构建框架概览)
2. [构建流程详解](#2-构建流程详解)
3. [核心 CMake 模块与函数](#3-核心-cmake-模块与函数)
4. [板级/芯片级 CMake 编写约定](#4-板级芯片级-cmake-编写约定)
5. [GD32VW55x 芯片级 CMake 适配](#5-gd32vw55x-芯片级-cmake-适配)
6. [GD32VW553H-EVAL 板级 CMake 适配](#6-gd32vw553h-eval-板级-cmake-适配)
7. [Wi-Fi/BLE SDK 集成](#7-wifi-ble-sdk-集成)
8. [已发现并修复的问题](#8-已发现并修复的问题)
9. [Make 与 CMake 对照表](#9-make-与-cmake-对照表)

---

## 1. OpenVela CMake 构建框架概览

OpenVela 基于 NuttX RTOS，其 CMake 构建系统位于 `nuttx/` 目录下，入口为
`nuttx/CMakeLists.txt`。构建通过标准 CMake 命令触发：

```bash
cmake -S nuttx -B build -DBOARD_CONFIG=<board>:<config>
# 例如:
cmake -S nuttx -B build \
  -DBOARD_CONFIG=contest2026_346_demaxiyazhili/boards/gd32vw553x/gd32vw553h-eval:nsh
```

### 目录布局

```
openvela/
├── nuttx/                          # NuttX 内核源码
│   ├── CMakeLists.txt              # 顶层构建入口
│   ├── cmake/                      # CMake 模块 (34 个 .cmake 文件)
│   ├── arch/<ARCH>/src/            # 架构/芯片代码
│   │   ├── CMakeLists.txt          # 创建 arch 内核库，添加芯片子目录
│   │   └── <chip>/CMakeLists.txt   # 芯片级源码列表
│   └── boards/CMakeLists.txt       # 创建 board 内核库，添加板级子目录
├── contest2026_346_demaxiyazhili/  # 自定义芯片/板级目录 (本项目)
│   ├── chips/gd32vw55x/            # 芯片级代码
│   │   ├── CMakeLists.txt
│   │   ├── Sdk.cmake
│   │   ├── gdwifi/Wireless.cmake
│   │   └── gdble/Bluetooth.cmake
│   └── boards/gd32vw553x/gd32vw553h-eval/  # 板级代码
│       ├── CMakeLists.txt
│       └── src/CMakeLists.txt
└── vendor/template/                # 官方模板参考
```

### 自定义路径机制

当芯片/板级代码不在 `nuttx/` 内核树中时，通过 Kconfig 变量指定自定义路径：

```
CONFIG_ARCH_CHIP_CUSTOM=y
CONFIG_ARCH_CHIP_CUSTOM_DIR="contest2026_346_demaxiyazhili/chips/gd32vw55x"

CONFIG_ARCH_BOARD_CUSTOM=y
CONFIG_ARCH_BOARD_CUSTOM_DIR="contest2026_346_demaxiyazhili/boards/gd32vw553x/gd32vw553h-eval"
```

构建系统据此解析出：

| 变量 | 含义 | 本项目值 |
|------|------|----------|
| `NUTTX_CHIP_ABS_DIR` | 芯片目录绝对路径 | `.../contest2026_346_demaxiyazhili/chips/gd32vw55x` |
| `NUTTX_BOARD_ABS_DIR` | 板级目录绝对路径 | `.../contest2026_346_demaxiyazhili/boards/gd32vw553x/gd32vw553h-eval` |
| `NUTTX_BOARD_DIR` | 同 `NUTTX_BOARD_ABS_DIR` | 同上 |

---

## 2. 构建流程详解

```
nuttx/CMakeLists.txt (顶层入口)
  │
  ├── [阶段 1] Kconfig 处理
  │     nuttx_process_config.cmake   → 预处理 defconfig 中的 #include
  │     nuttx_kconfig.cmake          → 运行 olddefconfig 生成 .config
  │     nuttx_export_kconfig(.config) → 所有 CONFIG_* 成为 CMake 变量
  │
  ├── [阶段 2] 目录解析
  │     解析 BOARD_CONFIG → NUTTX_BOARD_ABS_DIR
  │     解析 CONFIG_ARCH_CHIP_CUSTOM_DIR → NUTTX_CHIP_ABS_DIR
  │     创建符号链接: arch/<ARCH>/src/chip → 芯片目录
  │
  ├── [阶段 3] 工具链加载
  │     arch/<ARCH>/src/cmake/Toolchain.cmake
  │       → project(NuttX LANGUAGES C CXX ASM)
  │
  ├── [阶段 4] 库目标创建
  │     nuttx_add_kernel_library(arch)   ← 芯片代码目标
  │     nuttx_add_kernel_library(board)  ← 板级代码目标
  │
  ├── [阶段 5] 子目录递归
  │     add_subdirectory(arch)
  │       └── arch/<ARCH>/src/CMakeLists.txt
  │             └── add_subdirectory(${NUTTX_CHIP_ABS_DIR})  ← 芯片 CMake
  │     add_subdirectory(boards)
  │       └── boards/CMakeLists.txt
  │             └── add_subdirectory(${NUTTX_BOARD_ABS_DIR}) ← 板级 CMake
  │                   └── add_subdirectory(src)               ← 板级源码 CMake
  │
  └── [阶段 6] 链接
        收集所有库 → 链接 nuttx 可执行文件 → 生成 .bin/.hex/.asm
```

### 关键时序

- `arch` 目标在 `add_subdirectory(arch)` 时创建，芯片 CMake 在此阶段执行
- `board` 目标在 `add_subdirectory(boards)` 时创建，板级 CMake 在此阶段执行
- `board` 通过生成器表达式继承 `arch` 的 `INCLUDE_DIRECTORIES`：
  ```cmake
  target_include_directories(board PRIVATE
      $<TARGET_PROPERTY:arch,INCLUDE_DIRECTORIES>)
  ```
- 链接脚本通过全局属性 `LD_SCRIPT` 传递，在链接阶段读取

---

## 3. 核心 CMake 模块与函数

### 3.1 库管理 (`nuttx/cmake/nuttx_add_library.cmake`)

| 函数 | 用途 | 示例 |
|------|------|------|
| `nuttx_add_kernel_library(name [src])` | 创建内核库 (带 `__KERNEL__` 定义) | `arch`、`board` |
| `nuttx_add_system_library(name)` | 创建系统库 | `libcxx` |
| `nuttx_add_library(name)` | 创建用户库 | — |
| `nuttx_add_aux_library(name)` | 创建辅助 OBJECT 库 | ROMFS |
| `nuttx_add_extra_library()` | 添加外部预编译 `.a` | `libgcc` |

### 3.2 扩展 DSL (`nuttx/cmake/nuttx_extensions.cmake`)

```cmake
# 创建库 (在子目录 CMakeLists.txt 中使用)
nuttx_library(mylib [MODE "KERNEL"])

# 添加源文件
nuttx_sources(file1.c file2.c)
nuttx_sources_ifdef(CONFIG_VAR file3.c)

# 添加头文件路径
nuttx_include_directories(path/)
nuttx_include_directories_ifdef(CONFIG_VAR path/)

# 添加编译定义/选项
nuttx_compile_definitions(DEF1 DEF2)
nuttx_compile_options(-O2 -Wall)
```

### 3.3 ROMFS (`nuttx/cmake/nuttx_add_romfs.cmake`)

```cmake
nuttx_add_romfs(
    NAME etc
    MOUNTPOINT etc
    RCSRCS etc/init.d/rcS etc/init.d/rc.sysinit
    RCRAWS etc/group
    PATH ${CMAKE_CURRENT_LIST_DIR}/etc)
target_link_libraries(board PRIVATE romfs_etc)
```

### 3.4 应用注册 (`nuttx/cmake/nuttx_add_application.cmake`)

```cmake
nuttx_add_application(
    NAME myapp
    STACKSIZE 2048
    PRIORITY 100
    SRCS myapp.c)
```

### 3.5 Kconfig 集成 (`nuttx/cmake/nuttx_kconfig.cmake`)

- `nuttx_export_kconfig(.config)` — 将所有 `CONFIG_*` 导出为 CMake 变量
- `nuttx_olddefconfig()` — 运行 kconfiglib 的 `olddefconfig`
- `nuttx_generate_kconfig()` — 从 CMakeLists.txt 树自动生成 Kconfig 菜单

---

## 4. 板级/芯片级 CMake 编写约定

### 4.1 芯片级 (`chips/<chip>/CMakeLists.txt`)

**必须做的事：**

1. 定义核心源文件列表
2. 按 `CONFIG_*` 条件追加外设源文件
3. 调用 `target_sources(arch PRIVATE ${SRCS})` 添加到 `arch` 目标
4. 添加芯片头文件路径 `target_include_directories(arch PRIVATE ...)`

**模板参考** (`vendor/template/chips/chip_name/CMakeLists.txt`)：

```cmake
set(SRCS
    chip_start.c
    chip_irq.c
    chip_uart.c
    chip_allocateheap.c)

# 条件外设
if(CONFIG_CHIP_SPI)
    list(APPEND SRCS chip_spi.c)
endif()

target_sources(arch PRIVATE ${SRCS})
target_include_directories(arch PRIVATE ${NUTTX_CHIP_ABS_DIR}/include)
```

**可选操作：**

- `target_compile_definitions(arch PRIVATE ...)` — 添加编译宏
- `target_compile_options(arch PRIVATE ...)` — 添加编译选项
- `set_property(GLOBAL APPEND PROPERTY NUTTX_EXTRA_LIBRARIES ...)` — 注册预编译库
- `target_sources(nuttx PRIVATE head.S)` — 直接添加启动汇编到 nuttx 目标

### 4.2 板级顶层 (`boards/<arch>/<chip>/<board>/CMakeLists.txt`)

**必须做的事：**

1. 调用 `add_subdirectory(src)` 包含源码子目录
2. 设置链接脚本 `set_property(GLOBAL PROPERTY LD_SCRIPT ...)`

**模板参考** (`vendor/template/boards/chip_name/board_name/CMakeLists.txt`)：

```cmake
add_subdirectory(src)

# 可选：注册额外预编译库
file(GLOB EXTRA_LIBS
     ${NUTTX_BOARD_ABS_DIR}/libs/${CONFIG_ARCH_BOARD_CUSTOM_NAME}/*.a)
set_property(GLOBAL APPEND PROPERTY NUTTX_EXTRA_LIBRARIES ${EXTRA_LIBS})

set_property(GLOBAL PROPERTY LD_SCRIPT
             ${NUTTX_BOARD_ABS_DIR}/scripts/ld.script)
```

### 4.3 板级源码 (`boards/.../src/CMakeLists.txt`)

**必须做的事：**

1. 定义源文件列表
2. 按 `CONFIG_*` 条件追加源文件
3. 调用 `target_sources(board PRIVATE ${SRCS})`

**模板参考** (`vendor/template/boards/chip_name/board_name/src/CMakeLists.txt`)：

```cmake
set(SRCS board_boot.c board_bringup.c)

if(CONFIG_BOARDCTL)
    list(APPEND SRCS board_appinit.c)
endif()

if(CONFIG_BOARDCTL_RESET)
    list(APPEND SRCS board_reset.c)
endif()

target_sources(board PRIVATE ${SRCS})
```

**链接脚本位置选择：**

链接脚本可以在顶层或 src/ 中设置，但不要两处都设。推荐放在顶层（与模板一致）。

### 4.4 常见错误模式

| 错误 | 说明 |
|------|------|
| 使用 `PARENT_SCOPE` 导出自定义变量 | 框架不消费 `BOARD_SRCS` 等变量，它们是死代码 |
| 在顶层 CMakeLists.txt 中定义源文件列表 | 源文件应在 `src/CMakeLists.txt` 中通过 `target_sources` 添加 |
| 引用不存在的文件 | CMake 不会在 configure 阶段检查文件是否存在，错误推迟到 build 阶段 |
| 顶层和 src/ 重复设置 LD_SCRIPT | 只需一处设置，重复会导致后者覆盖前者 |
| 使用 `add_compile_definitions` 而非 `target_compile_definitions` | 前者影响全局，后者限定到特定目标 |

---

## 5. GD32VW55x 芯片级 CMake 适配

### 5.1 文件结构

```
chips/gd32vw55x/
├── CMakeLists.txt              # 主构建脚本 (224 行)
├── Kconfig                     # 芯片级配置 (580 行)
├── Make.defs                   # Make 构建入口
├── Sdk.cmake                   # SDK 定位与克隆 (CMake 版)
├── Sdk.mk                      # SDK 定位与克隆 (Make 版)
├── gd32vw55x_head.S            # 启动汇编
├── gd32vw55x_start.c           # 启动 C 代码
├── gd32vw55x_irq.c             # IRQ 处理
├── gd32vw55x_serial.c          # 串口驱动
├── ... (其他外设驱动)
├── gdwifi/
│   ├── Wireless.cmake          # Wi-Fi 配置 (CMake 版)
│   ├── Wireless.mk             # Wi-Fi 配置 (Make 版)
│   ├── wrapper_nuttx.c         # NuttX OS 适配层
│   ├── gdwifi_glue.c           # 平台胶水代码
│   ├── gdwifi_netdev.c         # 网络设备驱动
│   ├── net_compat/             # lwIP 兼容层
│   ├── config/                 # SDK 配置覆盖
│   └── patches/                # SDK 补丁
└── gdble/
    ├── Bluetooth.cmake         # BLE 配置 (CMake 版)
    ├── Bluetooth.mk            # BLE 配置 (Make 版)
    └── gd32_ble.c              # BLE 驱动
```

### 5.2 核心源文件 (无条件编译)

对照 `Make.defs` 中的 `CHIP_CSRCS`：

| 源文件 | 功能 | Make | CMake |
|--------|------|------|-------|
| `gd32vw55x_head.S` | 启动汇编 (地址 0x08000000) | `HEAD_ASRC` | `target_sources(nuttx PRIVATE ...)` |
| `gd32vw55x_start.c` | C 入口 | `CHIP_CSRCS` | `SRCS` |
| `gd32vw55x_irq.c` | IRQ 管理 | `CHIP_CSRCS` | `SRCS` |
| `gd32vw55x_irq_dispatch.c` | IRQ 分发 | `CHIP_CSRCS` | `SRCS` |
| `gd32vw55x_lowputc.c` | 底层串口输出 | `CHIP_CSRCS` | `SRCS` |
| `gd32vw55x_serial.c` | 串口驱动 | `CHIP_CSRCS` | `SRCS` |
| `gd32vw55x_gpio.c` | GPIO 驱动 | `CHIP_CSRCS` | `SRCS` |
| `gd32vw55x_clockconfig.c` | 时钟配置 | `CHIP_CSRCS` | `SRCS` |
| `gd32vw55x_allocateheap.c` | 堆分配 | `CHIP_CSRCS` | `SRCS` |
| `gd32vw55x_timerisr.c` | 定时器中断 | `CHIP_CSRCS` | `SRCS` |

### 5.3 条件外设源文件

| Kconfig 变量 | 源文件 | Make 条件 | CMake 条件 |
|-------------|--------|-----------|------------|
| `CONFIG_RTC` | `gd32vw55x_rtc.c` | `ifeq ($(CONFIG_RTC),y)` | `if(CONFIG_RTC)` |
| `CONFIG_GD32VW55X_DMA` | `gd32vw55x_dma.c` | `ifeq` | `if()` |
| `CONFIG_GD32VW55X_GPIO_IRQ` | `gd32vw55x_exti.c` | `ifeq` | `if()` |
| `CONFIG_GD32VW55X_SPI` | `gd32vw55x_spi.c` | `ifeq` | `if()` |
| `CONFIG_GD32VW55X_I2C` | `gd32vw55x_i2c.c` | `ifeq` | `if()` |
| `CONFIG_GD32VW55X_ADC` | `gd32vw55x_adc.c` | `ifeq` | `if()` |
| `CONFIG_GD32VW55X_PWM` | `gd32vw55x_pwm.c` | `ifeq` | `if()` |
| `CONFIG_GD32VW55X_CAPTURE` | `gd32vw55x_capture.c` | `ifeq` | `if()` |
| `CONFIG_GD32VW55X_FWDGT` | `gd32vw55x_fwdgt.c` | `ifeq` | `if()` |
| `CONFIG_GD32VW55X_WWDGT` | `gd32vw55x_wwdgt.c` | `ifeq` | `if()` |
| `CONFIG_GD32VW55X_PROGMEM` | `gd32vw55x_progmem.c` | `ifeq` | `if()` |
| `CONFIG_GD32VW55X_RNG` | `gd32vw55x_rng.c` | `ifeq` | `if()` |
| `CONFIG_GD32VW55X_CRC` | `gd32vw55x_crc.c` | `ifeq` | `if()` |

### 5.4 启动汇编处理差异

**Make** 使用专用变量 `HEAD_ASRC`，构建系统自动将其链接为第一个目标文件：

```makefile
HEAD_ASRC = gd32vw55x_head.S
```

**CMake** 直接添加到 `nuttx` 可执行目标（而非 `arch` 库），确保链接顺序：

```cmake
target_sources(nuttx PRIVATE gd32vw55x_head.S)
```

> **原因：** `HEAD_ASRC` 需要出现在地址 `0x08000000`。如果放入 `arch` 库，
> 链接器可能将其排在其他库的目标文件之后。直接添加到 `nuttx` 目标可保证其
> 在链接命令行中位于最前面。

### 5.5 SDK 头文件路径时序处理

**Make** 中 SDK 路径在芯片路径之前添加（因为 `INCLUDES` 先追加 SDK 后追加 chip）。

**CMake** 中 `arch/risc-v/src/CMakeLists.txt` 在芯片子目录处理完成后才添加
芯片路径（`BEFORE PUBLIC`）。为保持 SDK 路径在芯片路径之前的顺序，使用
`cmake_language(DEFER)` 延迟添加：

```cmake
# 在芯片 CMakeLists.txt 中
set_property(GLOBAL PROPERTY GD32VW55X_SDK_INCLUDES "${GD32VW55X_ALL_SDK_INCLUDES}")
cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}"
    CALL _gd32vw55x_add_sdk_includes)

# 延迟执行的函数
function(_gd32vw55x_add_sdk_includes)
    get_property(_includes GLOBAL PROPERTY GD32VW55X_SDK_INCLUDES)
    if(_includes)
        target_include_directories(arch BEFORE PRIVATE ${_includes})
    endif()
endfunction()
```

---

## 6. GD32VW553H-EVAL 板级 CMake 适配

### 6.1 文件结构

```
boards/gd32vw553x/gd32vw553h-eval/
├── CMakeLists.txt          # 板级顶层
├── Kconfig                 # 板级配置选项
├── configs/                # 构建配置 (13 个 defconfig)
│   ├── common/defconfig    # 基础配置
│   ├── nsh/defconfig       # 最小 NSH
│   ├── periph/defconfig    # 全外设
│   ├── ble/defconfig       # BLE
│   ├── wapi/defconfig      # Wi-Fi STA
│   └── ... (adc, lcd, lvgl, pwm 等)
├── include/
│   └── board.h             # 板级头文件
├── scripts/
│   ├── Make.defs           # Make 工具链配置
│   └── ld.script           # 链接脚本
└── src/
    ├── CMakeLists.txt      # 板级源码列表
    ├── Makefile            # Make 源码列表
    ├── gd32_boot.c         # 启动
    ├── gd32_bringup.c      # 板级初始化
    ├── gd32_appinit.c      # 应用初始化
    ├── gd32_autoleds.c     # LED 自动控制
    ├── gd32_userleds.c     # 用户 LED
    ├── gd32_buttons.c      # 按键
    ├── gd32_reset.c        # 复位
    ├── gd32_spi.c          # SPI 总线
    └── gd32_lcd.c          # LCD 显示
```

### 6.2 板级源文件条件编译

| Kconfig 变量 | 源文件 | 功能 |
|-------------|--------|------|
| (无条件) | `gd32_boot.c` | 启动引导 |
| (无条件) | `gd32_bringup.c` | 板级初始化 |
| `CONFIG_ARCH_LEDS` | `gd32_autoleds.c` | 自动 LED 控制 |
| `CONFIG_USERLED` | `gd32_userleds.c` | 用户 LED 控制 |
| `CONFIG_BOARDCTL` | `gd32_appinit.c` | 应用初始化 |
| `CONFIG_BOARDCTL_RESET` | `gd32_reset.c` | 系统复位 |
| `CONFIG_GD32VW55X_SPI` | `gd32_spi.c` | SPI 总线 |
| `CONFIG_BOARD_LCD_ENABLE` | `gd32_lcd.c` | LCD 显示 |

### 6.3 板级 Kconfig 资源冲突

板级 Kconfig 定义了以下引脚资源冲突：

| 冲突组 | 引脚 | 选项 A | 选项 B |
|--------|------|--------|--------|
| PA4/PA5/PA6 | LED | `BOARD_LED_ENABLE` | `BOARD_QSPI_FLASH_ENABLE` |
| PB15 | USART_TX | `BOARD_USART_CONSOLE` | `BOARD_IR_OUTPUT_ENABLE` |
| PB12/PB13 | LCD 控制 | `BOARD_LCD_ENABLE` | `BOARD_I2C1_ENABLE` |

---

## 7. Wi-Fi/BLE SDK 集成

### 7.1 SDK 获取 (`Sdk.cmake`)

```cmake
# SDK 仓库
set(GDWIFI_SDK_URL "https://github.com/GigaDeviceSemiconductor/GD32VW55x_WiFi_BLE_SDK.git")
set(GDWIFI_SDK_VERSION "945c6e28754f1bbdefb8bcd3049593fae8873bd5")  # V1.0.3g

# 自动克隆流程:
# 1. git clone → 指定路径
# 2. git checkout --quiet <SHA>
# 3. git apply patches/0001-nuttx-port.patch
```

可通过 `CONFIG_GD32VW55X_WIFI_SDK_PATH` 指定本地 SDK 路径跳过自动克隆。

### 7.2 Wi-Fi 配置 (`Wireless.cmake`)

**编译定义：**
```cmake
target_compile_definitions(arch PRIVATE
    CFG_RTOS EXEC_USING_STD_PRINTF GDWIFI_NUTTX GDWIFI_NUTTX_NET)
```

**源文件组成：**
- NuttX 适配层：`wrapper_nuttx.c`、`gdwifi_glue.c`、`gdwifi_netdev.c` 等 (5 个)
- wifi_manager：`wifi_eloop.c`、`wifi_init.c` 等 (7 个)
- util：`debug_print.c`、`crc.c` 等 (9 个)
- plf/src 平台层：`gd32vw55x_platform.c`、`init_rom.c` 等 (10 个)
- mbedTLS 3.6：从源码编译 (排除 `net_sockets.c`)
- SPL 子集：rcu、gpio、fmc 等外设驱动 (17 个)

**预编译库：**
- MAC 固件 (按 Kconfig 选择变体)：`wifi` / `wifi_minsram` / `wifi_mesh_smart` 等
- WPA supplicant：`wpas` / `wpas_wps_eap-tls`
- RF 库：`rf`
- NMSIS DSP：`nmsis_dsp_rv32imafc`

**链接脚本：** `ROM-EXPORT/symbol/rom_symbol_m.gcc` (ROM API 符号表)

**链接选项：** `-Wl,--wrap=setvbuf`

### 7.3 BLE 配置 (`Bluetooth.cmake`)

```cmake
target_compile_definitions(arch PRIVATE CFG_BLE_SUPPORT)
```

- BLE 头文件路径：`blesw/src/export/config_max`（必须在 `blesw/src/export` 之前）
- BLE 源文件：`gdble/gd32_ble.c`
- BLE 预编译库：`ble_max`（必须在 Wi-Fi 的 `--start-group/--end-group` 内）

> **依赖关系：** `CONFIG_GD32VW55X_BLE` 依赖 `CONFIG_GD32VW55X_WIFI`，
> 因为 BLE 需要 Wi-Fi 构建中的平台层代码（`sys_*`、`rf_*`、`nvds`、`util/`）。

### 7.4 预编译库链接方式

```cmake
# 在芯片 CMakeLists.txt 中
set_property(GLOBAL APPEND PROPERTY NUTTX_EXTRA_LIBRARIES
    -Wl,--start-group
    ${GDWIFI_LIBS}       # wifi, wpas, rf, nmsis_dsp
    ${GDBLE_LIB}         # ble_max (如果启用)
    -Wl,--end-group)
```

预编译库通过 `NUTTX_EXTRA_LIBRARIES` 全局属性加入链接，与 `libgcc` 等系统库
使用相同的链接组，确保可以互相解析符号。

---

## 8. 已发现并修复的问题

### 8.1 🔴 板级 CMakeLists.txt 引用不存在的文件 (已修复)

**问题：** 板级顶层 `CMakeLists.txt` 引用了多个不存在的文件：

```cmake
# 以下文件均不存在于 src/ 目录中
set(BOARD_SRCS src/gd32vw553h-eval_board.c)     # ✗ 不存在
list(APPEND BOARD_SRCS src/gd32vw55x_wifi_board.c)  # ✗ 不存在
list(APPEND BOARD_SRCS src/gd32vw55x_ble_board.c)   # ✗ 不存在

# 链接脚本扩展名错误
set(BOARD_LDSCRIPT scripts/gd32vw553h-eval.ld)   # ✗ 实际是 ld.script
```

**修复：** 重写为模板模式：

```cmake
add_subdirectory(src)
set_property(GLOBAL PROPERTY LD_SCRIPT
             ${NUTTX_BOARD_DIR}/scripts/ld.script)
```

### 8.2 🔴 板级 CMakeLists.txt 架构错误 (已修复)

**问题：** 使用 `PARENT_SCOPE` 导出自定义变量，但 OpenVela 框架不消费这些变量：

```cmake
# 这些变量不会被框架使用 — 是死代码
set(BOARD_SRCS "${BOARD_SRCS}" PARENT_SCOPE)
set(BOARD_INCLUDES "${BOARD_INCLUDES}" PARENT_SCOPE)
set(BOARD_CFLAGS "${BOARD_CFLAGS}" PARENT_SCOPE)
set(BOARD_LDSCRIPT "${BOARD_LDSCRIPT}" PARENT_SCOPE)
```

框架的实际工作方式是通过 `target_sources(board PRIVATE ...)` 和
`set_property(GLOBAL PROPERTY LD_SCRIPT ...)`。

**修复：** 删除所有自定义变量导出，遵循模板模式。

### 8.3 🟡 LD_SCRIPT 重复设置 (已修复)

**问题：** 顶层和 `src/CMakeLists.txt` 都设置了 `LD_SCRIPT`，后者覆盖前者。

**修复：** 统一在顶层设置，`src/CMakeLists.txt` 中移除。

### 8.4 🟡 Makefile 与 CMake 条件编译不一致

**问题：** `src/Makefile` 未处理 `CONFIG_BOARD_LCD_ENABLE`，但 `src/CMakeLists.txt` 中有：

```cmake
if(CONFIG_BOARD_LCD_ENABLE)
    list(APPEND SRCS gd32_lcd.c)
endif()
```

**影响：** Make 构建不会编译 `gd32_lcd.c`，CMake 构建会。

**建议：** 在 `src/Makefile` 中补充 LCD 条件编译，保持一致。

### 8.5 🟢 注释路径错误 (已修复)

**问题：** `src/CMakeLists.txt` 注释路径为 `boards/risc-v/gd32vw55x/...`，
实际应为 `boards/gd32vw553x/...`。

**修复：** 修正注释路径。

---

## 9. Make 与 CMake 对照表

### 9.1 构建入口

| 功能 | Make | CMake |
|------|------|-------|
| 配置 | `make distclean && ./tools/configure.sh <board>:<config>` | `cmake -S nuttx -B build -DBOARD_CONFIG=<board>:<config>` |
| 编译 | `make -j$(nproc)` | `cmake --build build -j$(nproc)` |
| 清理 | `make clean` | `cmake --build build --target clean` |
| 全清理 | `make distclean` | `rm -rf build` |

### 9.2 芯片级构建

| 功能 | Make (`Make.defs`) | CMake (`CMakeLists.txt`) |
|------|---------------------|--------------------------|
| 核心源文件 | `CHIP_CSRCS = file1.c file2.c` | `set(SRCS file1.c file2.c)` |
| 条件源文件 | `ifeq ($(CONFIG_X),y) CSRCS += ... endif` | `if(CONFIG_X) list(APPEND SRCS ...) endif()` |
| 启动汇编 | `HEAD_ASRC = head.S` | `target_sources(nuttx PRIVATE head.S)` |
| 添加源文件 | 自动 (CHIP_CSRCS 变量) | `target_sources(arch PRIVATE ${SRCS})` |
| 头文件路径 | `INCLUDES += -I path` | `target_include_directories(arch PRIVATE path)` |
| 编译定义 | `CFLAGS += -DDEF` | `target_compile_definitions(arch PRIVATE DEF)` |
| 编译选项 | `CFLAGS += -Wno-xxx` | `target_compile_options(arch PRIVATE -Wno-xxx)` |
| 预编译库 | `EXTRA_LIBS += -Wl,--start-group ...` | `set_property(GLOBAL APPEND PROPERTY NUTTX_EXTRA_LIBRARIES ...)` |

### 9.3 板级构建

| 功能 | Make (`Makefile`) | CMake (`CMakeLists.txt`) |
|------|-------------------|--------------------------|
| 核心源文件 | `CSRCS = boot.c bringup.c` | `set(SRCS boot.c bringup.c)` |
| 条件源文件 | `ifeq ... endif` | `if() ... endif()` |
| 添加源文件 | 自动 (CSRCS 变量) | `target_sources(board PRIVATE ${SRCS})` |
| 链接脚本 | `ARCHSCRIPT += $(BOARD_DIR)/scripts/ld.script` | `set_property(GLOBAL PROPERTY LD_SCRIPT ...)` |
| 工具链配置 | `scripts/Make.defs` | 由框架自动加载 `Toolchain.cmake` |
| ROMFS | `include $(TOPDIR)/boards/Board.mk` | `nuttx_add_romfs(...)` |

### 9.4 SDK 集成

| 功能 | Make | CMake |
|------|------|-------|
| SDK 克隆 | Makefile 规则 + `git clone` | `gd32vw55x_clone_sdk()` 函数 |
| SDK 路径包含 | `INCLUDES += -I$(SDK)/path` | `target_include_directories(arch PRIVATE ...)` |
| SDK 路径时序 | Make 变量追加顺序自然保证 | `cmake_language(DEFER)` 延迟注入 |
| SDK 头文件定义 | `.config` 自动传递 `-D` 标志 | `target_compile_definitions(arch PRIVATE ...)` |
| Wi-Fi 配置 | `include chip/gdwifi/Wireless.mk` | `include(${CMAKE_CURRENT_SOURCE_DIR}/gdwifi/Wireless.cmake)` |
| BLE 配置 | `include chip/gdble/Bluetooth.mk` | `include(${CMAKE_CURRENT_SOURCE_DIR}/gdble/Bluetooth.cmake)` |

---

## 附录 A: `scripts/Make.defs` 参数逐项 CMake 追踪

板级 `scripts/Make.defs` 是 Make 构建中板级定制的核心文件。以下逐项追踪其在
CMake 构建系统中的对应位置。

### Make.defs 原文

```makefile
include $(TOPDIR)/.config
include $(TOPDIR)/tools/Config.mk
include $(TOPDIR)/arch/risc-v/src/common/Toolchain.defs

ARCH_SRCDIR = $(TOPDIR)$(DELIM)arch$(DELIM)$(CONFIG_ARCH)$(DELIM)src
ARCH_INCDIR = $(TOPDIR)$(DELIM)arch$(DELIM)$(CONFIG_ARCH)$(DELIM)include

LDSCRIPT = ld.script
ARCHSCRIPT += $(BOARD_DIR)$(DELIM)scripts$(DELIM)$(LDSCRIPT)

ARCHCPUFLAGS += -mno-relax

ifeq ($(CONFIG_STACK_OVERFLOW_CHECK),y)
  ARCHCFLAGS += -finstrument-functions -ffixed-s11
endif

ARCHDEFINES += -DARCH_RISCV

CFLAGS = $(ARCHCFLAGS) $(ARCHOPTIMIZATION) $(ARCHCPUFLAGS) \
         $(ARCHINCLUDES) $(ARCHDEFINES) $(EXTRAFLAGS)
CPICFLAGS = $(ARCHPICFLAGS) $(CFLAGS)
CXXFLAGS := $(ARCHCXXFLAGS) $(ARCHOPTIMIZATION) $(ARCHCPUFLAGS) \
            $(ARCHXXINCLUDES) $(ARCHDEFINES) $(EXTRAFLAGS)
CXXPICFLAGS = $(ARCHPICFLAGS) $(CXXFLAGS)
CPPFLAGS = $(ARCHINCLUDES) $(ARCHDEFINES) $(EXTRAFLAGS)
AFLAGS += $(CFLAGS) -D__ASSEMBLY__

LDELFFLAGS += -melf32lriscv
```

### 逐项映射

#### 1. `LDSCRIPT` / `ARCHSCRIPT` — 链接脚本

| 构建系统 | 位置 | 代码 |
|----------|------|------|
| **Make** | `scripts/Make.defs:30-31` | `LDSCRIPT = ld.script` → `ARCHSCRIPT += $(BOARD_DIR)/scripts/$(LDSCRIPT)` |
| **CMake** | 板级 `CMakeLists.txt:19-20` | `set_property(GLOBAL PROPERTY LD_SCRIPT ${NUTTX_BOARD_DIR}/scripts/ld.script)` |
| **CMake 消费** | `nuttx/CMakeLists.txt:702` | `get_property(ldscript GLOBAL PROPERTY LD_SCRIPT)` |
| **CMake 预处理** | `nuttx/CMakeLists.txt:704-715` | 链接脚本经过 C 预处理器处理，生成 `.tmp` 文件 |
| **CMake 链接** | `nuttx/CMakeLists.txt:759-760` | `target_link_options(nuttx PRIVATE -T${ldscript})` |

> **差异：** Make 中 `ARCHSCRIPT` 直接传给链接器；CMake 中链接脚本先经过 C 预处理器
> 处理（展开 `#include`、`#define` 等），再传给链接器。

#### 2. `ARCH_SRCDIR` / `ARCH_INCDIR` — 架构源码/头文件目录

| 构建系统 | 位置 | 代码 |
|----------|------|------|
| **Make** | `scripts/Make.defs:27-28` | `ARCH_SRCDIR = $(TOPDIR)/arch/$(CONFIG_ARCH)/src` |
| **CMake** | `nuttx/CMakeLists.txt` 自动设置 | `NUTTX_CHIP_ABS_DIR` 指向芯片目录 |
| **CMake 头文件** | `nuttx/arch/risc-v/src/CMakeLists.txt:31` | `target_include_directories(arch BEFORE PUBLIC ${NUTTX_CHIP_ABS_DIR} common)` |
| **CMake 全局头文件** | `nuttx/cmake/nuttx_add_library.cmake:61-63` | `target_include_directories(... SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include)` |

> **差异：** Make 中通过 `ARCHINCLUDES` 变量收集所有 `-I` 路径后一次性加入 `CFLAGS`；
> CMake 中通过 `target_include_directories()` 分层添加到各目标，`board` 目标通过
> 生成器表达式继承 `arch` 的头文件路径。

#### 3. `-mno-relax` — 禁用链接器松弛优化

| 构建系统 | 位置 | 代码 |
|----------|------|------|
| **Make** | `scripts/Make.defs:37` | `ARCHCPUFLAGS += -mno-relax` |
| **CMake** | ⚠️ **缺失** | RISC-V CMake 工具链中无此标志 |

> **影响：** RISC-V 链接器松弛优化（relaxation）会将 `call` 指令替换为更短的
> `jal`，将 `lui/addi` 序列替换为更短的 `gp` 相对寻址。禁用此优化对
> Wi-Fi/BLE 预编译库的链接兼容性至关重要。
>
> **修复建议：** 在板级或芯片级 CMakeLists.txt 中添加：
> ```cmake
> target_compile_options(arch PRIVATE -mno-relax)
> ```

#### 4. `-finstrument-functions -ffixed-s11` — 栈溢出检测

| 构建系统 | 位置 | 代码 |
|----------|------|------|
| **Make** | `scripts/Make.defs:39-41` | `ifeq ($(CONFIG_STACK_OVERFLOW_CHECK),y)` → `-finstrument-functions -ffixed-s11` |
| **CMake** | ⚠️ **缺失** | RISC-V `Toolchain.cmake` 无 `CONFIG_STACK_OVERFLOW_CHECK` 处理 |

> **参考：** 其他架构的 CMake 已有此模式：
> - ARM (`arch/arm/src/cmake/armv7-m_gcc.cmake:90-94`): `-finstrument-functions -ffixed-r10`
> - Xtensa (`arch/xtensa/src/cmake/Toolchain.cmake:159`): `-finstrument-functions`
>
> **修复建议：** 在板级 CMakeLists.txt 中添加：
> ```cmake
> if(CONFIG_STACK_OVERFLOW_CHECK)
>     target_compile_options(arch PRIVATE -finstrument-functions -ffixed-s11)
> endif()
> ```

#### 5. `-DARCH_RISCV` — RISC-V 架构标识宏

| 构建系统 | 位置 | 代码 |
|----------|------|------|
| **Make** | `scripts/Make.defs:43` | `ARCHDEFINES += -DARCH_RISCV` |
| **CMake** | ⚠️ **缺失** | CMake 不添加此定义 |

> **说明：** RISC-V GCC/Clang 编译器自动提供内置宏 `__riscv` 和 `__riscv_xlen=32/64`，
> 功能上可替代 `ARCH_RISCV`。但如果代码中有 `#ifdef ARCH_RISCV` 判断，则会编译失败。
>
> **修复建议：** 在芯片级 CMakeLists.txt 中添加：
> ```cmake
> target_compile_definitions(arch PRIVATE ARCH_RISCV)
> ```

#### 6. `ARCHCPUFLAGS` — CPU 架构编译标志 (`-march=`, `-mabi=`)

| 构建系统 | 位置 | 代码 |
|----------|------|------|
| **Make** | `arch/risc-v/src/common/Toolchain.defs:242-273` | 从 `CONFIG_ARCH_RV32` 等 Kconfig 构建 `-march=rv32imafc -mabi=ilp32f` |
| **CMake** | `nuttx/arch/risc-v/src/cmake/Toolchain.cmake:229-359` | 同样逻辑，从 Kconfig 构建 `ARCHCPUFLAGS` |
| **CMake 应用** | `Toolchain.cmake:357-359` | `list(APPEND PLATFORM_FLAGS ${ARCHCPUFLAGS})` → `add_compile_options(${PLATFORM_FLAGS})` |

> **完整映射：**
> - Lines 229-296: 从 ISA 扩展 Kconfig 构建 `ARCHCPUEXTFLAGS` 字符串
> - Lines 300-312: 设置 `ARCHTYPE` (rv32/rv64) 和 `ARCHABITYPE` (ilp32/lp64)
> - Line 316: `set(ARCHCPUFLAGS -march=${ARCHTYPE}${ARCHCPUEXTFLAGS})`
> - Lines 320-329: 追加 `-mabi=${ARCHABITYPE}[d|f]`
> - Lines 341-355: 非 GNU 工具链映射到 LLVM CPU 模型

#### 7. `ARCHCFLAGS` — 架构编译选项

| Make 来源 | CMake 位置 | 代码 |
|-----------|-----------|------|
| `Toolchain.defs:84` `-fno-common` | `Toolchain.cmake:164` | `add_compile_options(-fno-common)` |
| `Toolchain.defs:87` `-Wall` | `Toolchain.cmake:165` | `add_compile_options(-Wall)` |
| `Toolchain.defs:87` `-Wstrict-prototypes` | `Toolchain.cmake:170` | `$<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>` (仅 C) |
| `Toolchain.defs:87` `-Wshadow -Wundef` | `Toolchain.cmake:166-167` | `add_compile_options(-Wshadow -Wundef)` |
| `Toolchain.defs:87` `-Wno-attributes` | `Toolchain.cmake:168` | `add_compile_options(-Wno-attributes)` |
| `Toolchain.defs:87` `-Wno-unknown-pragmas` | `Toolchain.cmake:169` | `add_compile_options(-Wno-unknown-pragmas)` |

#### 8. `ARCHOPTIMIZATION` — 优化选项

| Make 来源 | CMake 位置 | 代码 |
|-----------|-----------|------|
| `Toolchain.defs:52-60` 优化级别 | `Toolchain.cmake:125-133` | `-Oz` / `-Os` / 自定义优化级别 |
| `Toolchain.defs:62` `-fno-strict-aliasing` | `Toolchain.cmake:136` | `add_compile_options(-fno-strict-aliasing)` |
| `Toolchain.defs:64-70` 栈保护 | `Toolchain.cmake:146-148` | `-fstack-protector-all` (if `CONFIG_STACK_CANARIES`) |
| `Toolchain.defs:72-77` 栈使用 | `Toolchain.cmake:150-157` | `-fstack-usage` / `-Wstack-usage=` |
| `Toolchain.defs:79-82` 覆盖率 | `Toolchain.cmake:159-161` | `-fprofile-arcs -ftest-coverage` |
| `Toolchain.defs:128-131` 未使用段 | `Toolchain.cmake:204-207` | `-ffunction-sections -fdata-sections` |
| `Toolchain.defs:148-150` 调试符号 | `Toolchain.cmake:222-224` | 调试级别设置 |

#### 9. `LDELFFLAGS` — 共享库链接标志

| 构建系统 | 位置 | 代码 |
|----------|------|------|
| **Make** | `scripts/Make.defs:52` | `LDELFFLAGS += -melf32lriscv` |
| **Make** | `Toolchain.defs:117-118` | `LDFLAGS += -melf32lriscv` |
| **CMake** | `Toolchain.cmake:193-202` | `if(CONFIG_ARCH_RV32) add_link_options(-Wl,-melf32lriscv)` |

> **差异：** Make 直接传 `-melf32lriscv` 给 `ld`；CMake 通过 `-Wl,` 前缀经编译器驱动
> 传递，效果相同。

#### 10. `CFLAGS` / `CXXFLAGS` / `AFLAGS` — 最终标志组合

| 变量 | CMake 对应 |
|------|-----------|
| `CFLAGS` = ARCHCFLAGS + ARCHOPTIMIZATION + ARCHCPUFLAGS + ARCHINCLUDES + ARCHDEFINES + EXTRAFLAGS | CMake 中通过 `add_compile_options()` + `target_include_directories()` + `target_compile_definitions()` 分层组合，无需手动拼接 |
| `CXXFLAGS` = ARCHCXXFLAGS + ... | 同上，通过 `$<$<COMPILE_LANGUAGE:CXX>:...>` 生成器表达式区分 CXX 专属选项 |
| `AFLAGS` = CFLAGS + `-D__ASSEMBLY__` | `nuttx/CMakeLists.txt:633`: `add_compile_definitions($<$<COMPILE_LANGUAGE:ASM>:__ASSEMBLY__>)` |

> **核心差异：** Make 中所有标志最终合并到 `CFLAGS` 一个变量；CMake 中标志分散在
> `add_compile_options()` (全局)、`target_compile_options()` (目标级)、
> `target_include_directories()` (头文件)、`target_compile_definitions()` (宏定义)
> 等多个层面，由 CMake 自动组合传递给编译器。

### 缺失标志汇总

| 标志 | Make.defs 行号 | 功能 | 修复位置建议 |
|------|---------------|------|-------------|
| `-mno-relax` | L37 | 禁用链接器松弛优化 | 芯片 `CMakeLists.txt` |
| `-finstrument-functions` | L40 | 函数插桩 (栈溢出检测) | 板级 `CMakeLists.txt` |
| `-ffixed-s11` | L40 | 固定 s11 寄存器 (栈溢出检测) | 板级 `CMakeLists.txt` |
| `-DARCH_RISCV` | L43 | RISC-V 架构标识宏 | 芯片 `CMakeLists.txt` |
