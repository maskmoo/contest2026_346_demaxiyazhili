############################################################################
# chips/gd32vw55x/gdble/Bluetooth.cmake
#
# SPDX-License-Identifier: Apache-2.0
#
# GD32VW55x BLE 5.3 through the vendor's own BLE host.
# CMake equivalent of Bluetooth.mk
#
# The prebuilt libble_max.a is an all-in-one controller + RivieraWaves host
# (GAP/GATT/SMP inside the blob).  The controller talks to that host
# internally, not over HCI -- the blob is built without any HCI transport
# layer (h4tl.o is empty).  So we drive the vendor host directly (ble_adp_*,
# ble_adv_*, ble_gap_*), the way every GigaDevice BLE example does; there is
# no virtual HCI and no NuttX Bluetooth host here.
#
# Everything else libble needs (sys_*, rf_*, nvds, util/, the
# gd32vw55x_platform.c) already comes from the Wi-Fi build -- that is why
# GD32VW55X_BLE depends on GD32VW55X_WIFI.
#
# Validated HAL: SDK V1.0.3g (2026-04-23, commit 945c6e2).
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
############################################################################

# 编译选项 (对标 Makefile: CFLAGS += -DCFG_BLE_SUPPORT)
# 注意：必须使用 target_compile_definitions 而非 add_compile_definitions
target_compile_definitions(arch PRIVATE CFG_BLE_SUPPORT)

# SDK 路径 (对标 Makefile: SDKDIR = ...; MSDK = ...)
# 注意：如果 Wireless.cmake 已经定义了这些变量，这里就不需要重复定义
if(NOT DEFINED SDKDIR)
    set(SDKDIR "chip/sdk")
endif()
if(NOT DEFINED MSDK)
    set(MSDK "${SDKDIR}/MSDK")
endif()

# BLE 头文件路径 (对标 Makefile: INCLUDES += ...)
# config_max 必须在 blesw/src/export 之前，避免 ble_config.h 被覆盖
set(GDBLE_INCLUDES
    ${ARCH_SRCDIR}/${MSDK}/blesw/src/export/config_max
    ${ARCH_SRCDIR}/${MSDK}/blesw/src/export
)

# BLE 源文件 (对标 Makefile: CHIP_CSRCS += chip/gdble/gd32_ble.c)
# 注意：当此文件被 include() 包含时，CMAKE_CURRENT_SOURCE_DIR 是父 CMakeLists.txt 所在的目录
# 即 chips/gd32vw55x/，而不是 chips/gd32vw55x/gdble/
# 所以我们需要使用相对于父目录的路径
set(GDBLE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gdble")
set(GDBLE_SRCS
    ${GDBLE_DIR}/gd32_ble.c
)

# BLE 库 (对标 Makefile: GDBLE_LIB = ble_max)
# libble 依赖 rf_* (librf)，必须在 --start-group/--end-group 内部
# Wireless.mk 负责将其放入链接组
set(GDBLE_LIB "ble_max")

# 导出变量到父作用域
set(GDBLE_SRCS "${GDBLE_SRCS}" PARENT_SCOPE)
set(GDBLE_INCLUDES "${GDBLE_INCLUDES}" PARENT_SCOPE)
set(GDBLE_LIB "${GDBLE_LIB}" PARENT_SCOPE)
