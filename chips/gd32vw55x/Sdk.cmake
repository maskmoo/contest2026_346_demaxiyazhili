############################################################################
# chips/gd32vw55x/GD32VW55X_SDK.cmake
#
# SPDX-License-Identifier: Apache-2.0
#
# Locates the GigaDevice GD32VW55x SDK and exposes it under chip/sdk.
# CMake equivalent of Sdk.mk
#
# By default the SDK is cloned at "context" time, pinned to a validated
# commit, with the port patches applied on top -- that is what lets CI
# (and anyone with only the NuttX tree) build with nothing preinstalled.
# CONFIG_GD32VW55X_WIFI_SDK_PATH overrides it with a local checkout.
#
# The Wi-Fi and BLE support need the SDK for the prebuilt radio libraries;
# PROGMEM needs it for the mask ROM API headers (the SiP flash is not
# programmed through the FMC registers -- see gd32vw55x_progmem.c).
#
# PINNED VERSION: SDK V1.0.3g -- see gigadevice_port/SDK_VERSION.md.  The
# prebuilt libraries are binary, and the port depends on their ABI: do not
# move this SHA without revalidating.
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to you under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
############################################################################

# SDK 版本配置 (对标 Makefile: GDWIFI_SDK_REPO, GDWIFI_SDK_URL, GDWIFI_SDK_VERSION)
set(GDWIFI_SDK_REPO "GD32VW55x_WiFi_BLE_SDK")
set(GDWIFI_SDK_URL "https://github.com/GigaDeviceSemiconductor/GD32VW55x_WiFi_BLE_SDK.git")
set(GDWIFI_SDK_VERSION "945c6e28754f1bbdefb8bcd3049593fae8873bd5")

# 补丁文件路径 (对标 Makefile: GDWIFI_PATCHES)
set(GDWIFI_PATCHES "${CMAKE_CURRENT_SOURCE_DIR}/gdwifi/patches/0001-nuttx-port.patch")

# 从配置中获取 SDK 路径并移除引号 (对标 Makefile: GDWIFI_SDK := $(patsubst "%",%,$(CONFIG_GD32VW55X_WIFI_SDK_PATH)))
if(CONFIG_GD32VW55X_WIFI_SDK_PATH)
    string(REGEX REPLACE "^\"(.*)\"$" "\\1" GDWIFI_SDK "${CONFIG_GD32VW55X_WIFI_SDK_PATH}")
else()
    set(GDWIFI_SDK "")
endif()

# 如果路径为空，设置默认克隆路径 (对标 Makefile: ifeq ($(GDWIFI_SDK),))
if("${GDWIFI_SDK}" STREQUAL "")
    set(GDWIFI_SDK "${CMAKE_CURRENT_SOURCE_DIR}/${GDWIFI_SDK_REPO}")
    set(GDWIFI_SDK_AUTO_CLONE TRUE)
else()
    set(GDWIFI_SDK_AUTO_CLONE FALSE)
endif()

# 创建符号链接 (对标 Makefile: GDWIFI_LINK := $(shell ln -sfn ...))
# Makefile 中: GDWIFI_LINK = $(ARCH_SRCDIR)/chip/sdk -> GDWIFI_SDK
# 在 CMake 中，ARCH_SRCDIR = CMAKE_CURRENT_SOURCE_DIR = CHIP_DIR (即 Makefile 中的 $(ARCH_SRCDIR)/chip)
# 所以符号链接路径为 ${CMAKE_CURRENT_SOURCE_DIR}/sdk
set(GDWIFI_LINK "${CMAKE_CURRENT_SOURCE_DIR}/sdk")
if(EXISTS "${GDWIFI_LINK}")
    file(REMOVE "${GDWIFI_LINK}")
endif()
file(CREATE_LINK "${GDWIFI_SDK}" "${GDWIFI_LINK}" SYMBOLIC)

# 设置 SDK 相关变量 (对标 Makefile: SDKDIR = chip/sdk, MSDK = chip/sdk/MSDK)
# Makefile 中 SDKDIR 是相对于 ARCH_SRCDIR 的路径
# 在 CMake 中 ARCH_SRCDIR = CMAKE_CURRENT_SOURCE_DIR = Makefile 中的 $(ARCH_SRCDIR)/chip
# 所以 SDKDIR 应该是 "sdk" (相对于 CMAKE_CURRENT_SOURCE_DIR，等价于 Makefile 中的 "chip/sdk"相对于 $(ARCH_SRCDIR))
set(SDKDIR "sdk")
set(MSDK "${SDKDIR}/MSDK")

# 添加头文件搜索路径 (对标 Makefile: INCLUDES += ...)
# 注意：这些路径需要在 CMakeLists.txt 中通过 target_include_directories 添加到 arch 目标
set(GDWIFI_SDK_INCLUDES
    "${GDWIFI_SDK}/ROM-EXPORT/bootloader"
    "${GDWIFI_SDK}/ROM-EXPORT/halcomm"
)

# 自动克隆函数 (对标 Makefile: $(GDWIFI_SDK): 规则)
function(gd32vw55x_clone_sdk)
    if(GDWIFI_SDK_AUTO_CLONE)
        # 检查是否已经克隆
        if(EXISTS "${GDWIFI_SDK}/.git")
            message(STATUS "GD32VW55x SDK already cloned: ${GDWIFI_SDK}")
            return()
        endif()

        message(STATUS "Cloning GigaDevice GD32VW55x Wi-Fi/BLE SDK")

        # 克隆仓库 (对标 Makefile: $(call CLONE, ...))
        execute_process(
            COMMAND git clone ${GDWIFI_SDK_URL} ${GDWIFI_SDK}
            RESULT_VARIABLE CLONE_RESULT
        )

        if(NOT CLONE_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to clone GD32VW55x SDK")
        endif()

        # 切换到指定版本 (对标 Makefile: git -C ... checkout --quiet ...)
        message(STATUS "GD32VW55x SDK: ${GDWIFI_SDK_VERSION}")
        execute_process(
            COMMAND git -C ${GDWIFI_SDK} checkout --quiet ${GDWIFI_SDK_VERSION}
            RESULT_VARIABLE CHECKOUT_RESULT
        )

        if(NOT CHECKOUT_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to checkout SDK version ${GDWIFI_SDK_VERSION}")
        endif()

        # 应用补丁 (对标 Makefile: git -C ... apply ...)
        message(STATUS "Applying NuttX port patches")
        execute_process(
            COMMAND git -C ${GDWIFI_SDK} apply ${GDWIFI_PATCHES}
            RESULT_VARIABLE PATCH_RESULT
        )

        if(NOT PATCH_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to apply NuttX port patches")
        endif()

        message(STATUS "GD32VW55x SDK cloned and patched successfully")
    endif()
endfunction()

# 清理函数 (对标 Makefile: distclean:: 规则)
function(gd32vw55x_clean_sdk)
    if(GDWIFI_SDK_AUTO_CLONE AND EXISTS "${GDWIFI_SDK}")
        message(STATUS "Removing cloned SDK: ${GDWIFI_SDK}")
        file(REMOVE_RECURSE "${GDWIFI_SDK}")
    endif()

    # 清理符号链接
    if(EXISTS "${GDWIFI_LINK}")
        file(REMOVE "${GDWIFI_LINK}")
    endif()
endfunction()

# 导出变量到父作用域
set(GDWIFI_SDK "${GDWIFI_SDK}" PARENT_SCOPE)
set(GDWIFI_SDK_AUTO_CLONE "${GDWIFI_SDK_AUTO_CLONE}" PARENT_SCOPE)
set(SDKDIR "${SDKDIR}" PARENT_SCOPE)
set(MSDK "${MSDK}" PARENT_SCOPE)
set(GDWIFI_LINK "${GDWIFI_LINK}" PARENT_SCOPE)
set(GDWIFI_SDK_INCLUDES "${GDWIFI_SDK_INCLUDES}" PARENT_SCOPE)
