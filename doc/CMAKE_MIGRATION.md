# CMake 适配说明

## 概述

对标 Makefile 构建逻辑，为 GD32VW55x 创建等效的 CMake 脚本。

## 文件结构

```
chips/gd32vw55x/
├── CMakeLists.txt              # 芯片级 CMake（串联各模块）
├── GD32VW55X_SDK.cmake         # SDK 路径处理（对标 Sdk.mk）
├── gdwifi/
│   ├── Wireless.mk             # 原 Makefile
│   └── Wireless.cmake          # 🆕 CMake 等效（对标 Wireless.mk）
└── gdble/
    ├── Bluetooth.mk            # 原 Makefile
    └── Bluetooth.cmake         # 🆕 CMake 等效（对标 Bluetooth.mk）
```

## 映射关系

| Makefile | CMake | 功能 |
|----------|-------|------|
| `Sdk.mk` | `GD32VW55X_SDK.cmake` | SDK 路径处理、自动克隆 |
| `Wireless.mk` | `gdwifi/Wireless.cmake` | Wi-Fi 源文件、头文件、库 |
| `Bluetooth.mk` | `gdble/Bluetooth.cmake` | BLE 源文件、头文件、库 |

## 核心逻辑

### 1. SDK 路径处理 (GD32VW55X_SDK.cmake)

```cmake
# 对标 Makefile:
# GDWIFI_SDK := $(patsubst "%",%,$(CONFIG_GD32VW55X_WIFI_SDK_PATH))
# ifeq ($(GDWIFI_SDK),)
#   GDWIFI_SDK = $(ARCH_SRCDIR)/chip/GD32VW55x_WiFi_BLE_SDK
# endif

if(CONFIG_GD32VW55X_WIFI_SDK_PATH)
    string(REGEX REPLACE "^\"(.*)\"$" "\\1" GDWIFI_SDK ...)
else()
    set(GDWIFI_SDK "${CMAKE_CURRENT_SOURCE_DIR}/chip/${GDWIFI_SDK_REPO}")
endif()
```

### 2. Wi-Fi 编译 (Wireless.cmake)

- 编译选项: `-DCFG_RTOS -DEXEC_USING_STD_PRINTF -DGDWIFI_NUTTX`
- 包含路径: SDK 各子目录
- 源文件: glue、wifi_manager、util、plf、mbedTLS、SPL
- 预编译库: `libwifi.a`、`libwpas.a`、`librf.a`

### 3. BLE 编译 (Bluetooth.cmake)

- 编译选项: `-DCFG_BLE_SUPPORT`
- 包含路径: `blesw/src/export`
- 源文件: `gd32_ble.c`
- 预编译库: `libble_max.a`

## 串联流程

```
CMakeLists.txt (芯片级)
    │
    ├── if(CONFIG_GD32VW55X_WIFI)
    │       │
    │       ├── include(GD32VW55X_SDK.cmake)  # SDK 路径
    │       ├── gd32vw55x_clone_sdk()         # 自动克隆
    │       ├── include(gdwifi/Wireless.cmake) # Wi-Fi 配置
    │       └── 添加源文件、头文件、库
    │
    └── if(CONFIG_GD32VW55X_BLE)
            │
            ├── include(gdble/Bluetooth.cmake) # BLE 配置
            └── 添加源文件、头文件、库
```

## CONFIG_GD32VW55X_WIFI_SDK_PATH 行为

| 配置值 | 行为 |
|--------|------|
| `""` 或未设置 | 自动克隆到 `chip/GD32VW55x_WiFi_BLE_SDK` |
| `"/path/to/sdk"` | 使用指定的本地 SDK 路径 |

## 注意事项

1. BLE 依赖 Wi-Fi，必须先启用 Wi-Fi
2. SDK 版本锁定为 V1.0.3g (commit 945c6e2)
3. 预编译库使用 `--start-group/--end-group` 处理循环依赖
4. `--wrap=setvbuf` 解决 SDK 与 NuttX stdio 冲突
