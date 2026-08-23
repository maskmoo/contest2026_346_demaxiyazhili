# CONFIG_GD32VW55X_WIFI_SDK_PATH 配置指南

## 📋 概述

`CONFIG_GD32VW55X_WIFI_SDK_PATH` 是一个 Kconfig 字符串配置项，用于指定 GigaDevice GD32VW55x Wi-Fi/BLE SDK 的本地路径。该配置决定了构建系统如何获取 SDK：自动克隆或使用本地副本。

**配置类型**：字符串
**默认值**：`""` (空字符串)
**依赖项**：`CONFIG_GD32VW55X_WIFI=y`

---

## 🔗 依赖关系

### 前置依赖
```
CONFIG_GD32VW55X_WIFI=y
    │
    └── CONFIG_GD32VW55X_WIFI_SDK_PATH (可选)
```

### 功能依赖
启用 Wi-Fi 功能后，以下模块依赖此 SDK：
- **Wi-Fi 802.11ax**：主功能模块
- **BLE 5.3**：依赖 Wi-Fi，共享时钟/RF/NVDS
- **PROGMEM**：需要 ROM API 头文件
- **预编译库**：libwifi*.a, librf.a, libble_max.a
- **NVDS 存储**：RF 校准数据

---

## 🎯 配置方式

### 方式 1：使用 make menuconfig（推荐）

```bash
# 进入 NuttX 配置界面
cd nuttx
./tools/configure.sh <board>:<config>
make menuconfig

# 导航路径
System Type → GD32VW55x Configuration Options → Wi-Fi 802.11ax support
    → Path to a local GD32VW55x Wi-Fi/BLE SDK checkout (optional)
```

### 方式 2：直接编辑 .config 文件

```bash
# 编辑 nuttx/.config
CONFIG_GD32VW55X_WIFI_SDK_PATH="/path/to/your/sdk"
```

### 方式 3：使用 defconfig 文件

```bash
# 在 boards/<board>/configs/<config>/defconfig 中添加
CONFIG_GD32VW55X_WIFI_SDK_PATH="/path/to/your/sdk"
```

---

## 🔄 两种工作模式

### 模式 1：自动克隆（默认）

**配置**：
```makefile
CONFIG_GD32VW55X_WIFI_SDK_PATH=""
# 或不设置
```

**行为**：
1. 从 GitHub 自动克隆 SDK
2. 固定版本：commit `945c6e2` (SDK V1.0.3g, 2026-04-23)
3. 自动应用 NuttX 移植补丁
4. 克隆路径：`arch/risc-v/src/gd32vw55x/chip/GD32VW55x_WiFi_BLE_SDK`

**优点**：
- ✅ 无需手动操作
- ✅ 版本一致性保证
- ✅ 适用于 CI/CD 和全新环境
- ✅ 可重现的构建

**缺点**：
- ❌ 每次清理后需重新克隆
- ❌ 网络依赖

### 模式 2：本地路径

**配置**：
```makefile
CONFIG_GD32VW55X_WIFI_SDK_PATH="/home/developer/GD32VW55x_WiFi_BLE_SDK"
```

**行为**：
1. 跳过自动克隆
2. 跳过版本 checkout
3. 跳过补丁应用
4. 直接使用指定路径的 SDK

**前提条件**：
- ✅ SDK 已手动克隆
- ✅ 已切换到正确版本 (`945c6e2`)
- ✅ 已应用 NuttX 移植补丁

**优点**：
- ✅ 构建速度快（无需克隆）
- ✅ 适用于开发调试
- ✅ 可修改 SDK 源码

**缺点**：
- ❌ 需要手动维护
- ❌ 版本一致性需自行保证

---

## 📁 文件结构

### 配置文件位置

```
contest2026_346_demaxiyazhili/
├── chips/gd32vw55x/
│   ├── Kconfig                    # 配置定义（第396-411行）
│   └── Sdk.mk                    # 配置应用逻辑（第51-70行）
├── boards/gd32vw553x/gd32vw553h-eval/configs/
│   ├── ble/defconfig
│   ├── wapi/defconfig
│   └── sta_softap/defconfig
└── nuttx/
    ├── .config                    # 当前生效配置
    └── include/nuttx/config.h     # 编译时配置头文件
```

### SDK 目录结构

```
GD32VW55x_WiFi_BLE_SDK/
├── MSDK/                          # 主 SDK 目录
├── ROM-EXPORT/
│   ├── bootloader/                # ROM API 头文件
│   └── halcomm/                   # HAL 通用头文件
├── wifi_manager/                  # Wi-Fi 管理层
├── wpa_supplicant/                # WPA 认证
└── ...
```

---

## 🔧 Sdk.mk 应用逻辑

### 核心代码解析

```makefile
# 第1行：提取配置值并移除引号
GDWIFI_SDK := $(patsubst "%",%,$(CONFIG_GD32VW55X_WIFI_SDK_PATH))

# 第2行：判断是否为空
ifeq ($(GDWIFI_SDK),)

# 第3行：设置默认克隆路径
GDWIFI_SDK = $(ARCH_SRCDIR)$(DELIM)chip$(DELIM)$(GDWIFI_SDK_REPO)

# 自动克隆规则
$(GDWIFI_SDK):
	$(Q) echo "Cloning GigaDevice GD32VW55x Wi-Fi/BLE SDK"
	$(Q) $(call CLONE, $(GDWIFI_SDK_URL),$(GDWIFI_SDK))
	$(Q) git -C $(GDWIFI_SDK) checkout --quiet $(GDWIFI_SDK_VERSION)
	$(Q) git -C $(GDWIFI_SDK) apply $(GDWIFI_PATCHES)

endif

# 创建符号链接（两种模式共用）
GDWIFI_LINK := $(shell ln -sfn $(GDWIFI_SDK) $(ARCH_SRCDIR)$(DELIM)chip$(DELIM)sdk)
```

### 执行流程

```
CONFIG_GD32VW55X_WIFI_SDK_PATH
         │
         ▼
┌─────────────────────────┐
│ 移除引号 (patsubst)     │
└───────────┬─────────────┘
            │
            ▼
    ┌───────────────┐
    │ 值是否为空？   │
    └───────┬───────┘
            │
    ┌───────┴───────┐
    │               │
   空字符串        非空字符串
    │               │
    ▼               ▼
┌────────────┐  ┌────────────────┐
│ 自动克隆   │  │ 使用本地路径    │
│ • git clone│  │ （跳过克隆）    │
│ • checkout │  └────────────────┘
│ • patch    │
└────────────┘
         │
         ▼
┌─────────────────────────┐
│ 创建符号链接 chip/sdk   │
└─────────────────────────┘
```

---

## 📊 影响范围

### 直接影响

| 文件 | 影响 |
|------|------|
| `nuttx/.config` | 配置存储位置 |
| `nuttx/include/nuttx/config.h` | 编译时宏定义 |
| `chips/gd32vw55x/Sdk.mk` | 构建逻辑 |

### 间接影响

| 模块 | 影响说明 |
|------|----------|
| Wi-Fi 驱动 | 使用 SDK 中的预编译库 |
| BLE 驱动 | 依赖 Wi-Fi 和 SDK |
| PROGMEM | 需要 ROM API 头文件 |
| 构建系统 | 决定 SDK 来源 |

---

## 🛠️ 使用示例

### 示例 1：首次构建（自动克隆）

```bash
# 1. 配置（使用默认值）
./tools/configure.sh gd32vw553h-eval:sta_softap

# 2. 构建
make context  # 自动克隆 SDK
make

# 输出：
# Cloning GigaDevice GD32VW55x Wi-Fi/BLE SDK
# GD32VW55x SDK: 945c6e28754f1bbdefb8bcd3049593fae8873bd5
# Applying NuttX port patches
```

### 示例 2：开发调试（本地路径）

```bash
# 1. 手动克隆并修补 SDK
git clone https://github.com/GigaDeviceSemiconductor/GD32VW55x_WiFi_BLE_SDK.git ~/my-sdk
cd ~/my-sdk
git checkout 945c6e2
git apply /path/to/contest2026_346_demaxiyazhili/chips/gd32vw55x/gdwifi/patches/0001-nuttx-port.patch

# 2. 配置
make menuconfig
# 设置：CONFIG_GD32VW55X_WIFI_SDK_PATH="/home/user/my-sdk"

# 3. 构建（直接使用本地 SDK）
make
```

### 示例 3：切换 SDK 版本

```bash
# 1. 复制 SDK 到新目录
cp -r ~/my-sdk ~/my-sdk-v2

# 2. 修改版本
cd ~/my-sdk-v2
git checkout <new-commit>
# 应用补丁（可能需要手动调整）

# 3. 更新配置
CONFIG_GD32VW55X_WIFI_SDK_PATH="/home/user/my-sdk-v2"

# 4. 验证
make clean
make
```

---

## ⚠️ 注意事项

### 1. 版本锁定
- 当前验证版本：SDK V1.0.3g (2026-04-23)
- 验证 commit：`945c6e28754f1bbdefb8bcd3049593fae8873bd5`
- **不要随意更改版本**，预编译库的 ABI 与移植层绑定

### 2. 补丁应用
- 自动模式会自动应用补丁
- 本地模式需手动应用：`git apply 0001-nuttx-port.patch`
- 补丁位置：`chips/gd32vw55x/gdwifi/patches/`

### 3. 路径格式
- 支持绝对路径：`/home/user/sdk`
- 支持相对路径：`../sdk`（相对于 nuttx 目录）
- 路径中不要包含空格或特殊字符

### 4. 符号链接
- 两种模式都会创建 `chip/sdk` 符号链接
- 确保有创建符号链接的权限
- 符号链接指向实际 SDK 目录

### 5. 清理操作
```bash
# 自动模式：清理会删除克隆的 SDK
make distclean

# 本地模式：清理不会影响本地 SDK
make distclean
# SDK 保持不变
```

---

## 🔍 故障排除

### 问题 1：克隆失败

**现象**：
```
fatal: repository 'https://github.com/GigaDeviceSemiconductor/GD32VW55x_WiFi_BLE_SDK.git' not found
```

**解决**：
1. 检查网络连接
2. 检查 GitHub 访问权限
3. 使用本地模式绕过

### 问题 2：补丁应用失败

**现象**：
```
error: patch failed: ...
error: ...
```

**解决**：
1. 确认 SDK 版本正确
2. 手动应用补丁并解决冲突
3. 使用本地模式

### 问题 3：路径无效

**现象**：
```
make: *** No rule to make target '...'
```

**解决**：
1. 检查路径是否正确
2. 确认 SDK 目录结构完整
3. 检查权限

---

## 📚 相关文档

- **Kconfig 定义**：`chips/gd32vw55x/Kconfig` (第396-411行)
- **构建逻辑**：`chips/gd32vw55x/Sdk.mk`
- **移植说明**：`chips/gd32vw55x/gdwifi/README.md`
- **版本信息**：`gigadevice_port/SDK_VERSION.md`

---

## 📝 版本历史

| 日期 | 版本 | 说明 |
|------|------|------|
| 2026-04-23 | V1.0.3g | 当前验证版本 |
| - | 945c6e2 | 验证 commit |

---

**文档维护**：GD32VW55x 移植团队
**最后更新**：2026-08-23
