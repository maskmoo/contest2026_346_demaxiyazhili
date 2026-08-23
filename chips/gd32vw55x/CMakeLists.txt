# chips/gd32vw55x/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
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

# 延迟调用的函数：在 arch/risc-v/src 处理完后添加 SDK 头文件路径
# 这确保 SDK 路径在 chip 路径（由 arch/risc-v/src 添加）之后被添加
# 使用 BEFORE 使 SDK 路径被 prepend 到 chip 路径之前
function(_gd32vw55x_add_sdk_includes)
    get_property(_includes GLOBAL PROPERTY GD32VW55X_SDK_INCLUDES)
    if(_includes)
        target_include_directories(arch BEFORE PRIVATE ${_includes})
    endif()
    # board 目标继承了 arch 的 INCLUDE_DIRECTORIES，所以 SDK 路径对 board 也可见
    # 但 board 不继承 COMPILE_DEFINITIONS，需要单独添加 SDK 相关定义
    # 以避免 SDK 的 gd32vw55x.h 中 ErrStatus 与 NuttX sys/types.h 中 ERROR 冲突
    get_property(_defs GLOBAL PROPERTY GD32VW55X_SDK_DEFINES)
    if(TARGET board)
        if(_defs)
            target_compile_definitions(board PRIVATE ${_defs})
        endif()
        # board 也需要 GDWIFI_NUTTX 等定义（来自 Wireless.cmake / Bluetooth.cmake）
        get_property(_wifi_defs GLOBAL PROPERTY GD32VW55X_WIFI_DEFINES)
        if(_wifi_defs)
            target_compile_definitions(board PRIVATE ${_wifi_defs})
        endif()
    endif()
endfunction()

if(CONFIG_ARCH_CHIP_GD32VW55X)

# Head assembly file - must be linked first, directly to nuttx target
# (not in the arch library) to ensure it appears at address 0x08000000
target_sources(nuttx PRIVATE gd32vw55x_head.S)

# Core sources (unconditional)
set(SRCS
    gd32vw55x_start.c
    gd32vw55x_irq.c
    gd32vw55x_irq_dispatch.c
    gd32vw55x_lowputc.c
    gd32vw55x_serial.c
    gd32vw55x_gpio.c
    gd32vw55x_clockconfig.c
    gd32vw55x_allocateheap.c
    gd32vw55x_timerisr.c
)

# Optional peripheral drivers
if(CONFIG_RTC)
    list(APPEND SRCS gd32vw55x_rtc.c)
endif()

if(CONFIG_GD32VW55X_DMA)
    list(APPEND SRCS gd32vw55x_dma.c)
endif()

if(CONFIG_GD32VW55X_GPIO_IRQ)
    list(APPEND SRCS gd32vw55x_exti.c)
endif()

if(CONFIG_GD32VW55X_SPI)
    list(APPEND SRCS gd32vw55x_spi.c)
endif()

if(CONFIG_GD32VW55X_I2C)
    list(APPEND SRCS gd32vw55x_i2c.c)
endif()

if(CONFIG_GD32VW55X_ADC)
    list(APPEND SRCS gd32vw55x_adc.c)
endif()

if(CONFIG_GD32VW55X_PWM)
    list(APPEND SRCS gd32vw55x_pwm.c)
endif()

if(CONFIG_GD32VW55X_CAPTURE)
    list(APPEND SRCS gd32vw55x_capture.c)
endif()

if(CONFIG_GD32VW55X_FWDGT)
    list(APPEND SRCS gd32vw55x_fwdgt.c)
endif()

if(CONFIG_GD32VW55X_WWDGT)
    list(APPEND SRCS gd32vw55x_wwdgt.c)
endif()

if(CONFIG_GD32VW55X_PROGMEM)
    list(APPEND SRCS gd32vw55x_progmem.c)
endif()

if(CONFIG_GD32VW55X_RNG)
    list(APPEND SRCS gd32vw55x_rng.c)
endif()

if(CONFIG_GD32VW55X_CRC)
    list(APPEND SRCS gd32vw55x_crc.c)
endif()

# ===========================================================================
# SDK support (对标 Makefile: ifneq ($(CONFIG_GD32VW55X_WIFI)$(CONFIG_GD32VW55X_BLE)$(CONFIG_GD32VW55X_PROGMEM),))
# ===========================================================================
# 收集所有 SDK 头文件路径
# Makefile 中 SDK 路径在 chip 路径之前（因为 INCLUDES 先添加 SDK 后添加 chip）
# CMake 中 arch/risc-v/src/CMakeLists.txt 在 chip 子目录处理后才添加 chip 路径（BEFORE PUBLIC）
# 所以需要用 cmake_language(DEFER) 在 arch/risc-v/src 处理完后添加 SDK 路径
set(GD32VW55X_ALL_SDK_INCLUDES "")

if(CONFIG_GD32VW55X_WIFI OR CONFIG_GD32VW55X_BLE OR CONFIG_GD32VW55X_PROGMEM)
    # 包含 SDK 配置 (对标 Makefile: include Sdk.mk)
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sdk.cmake)

    # 克隆 SDK（如果需要）
    gd32vw55x_clone_sdk()

    # 设置 ARCH_SRCDIR 用于 Wireless.cmake / Bluetooth.cmake
    set(ARCH_SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}")

    # 收集 SDK ROM-EXPORT 头文件路径
    list(APPEND GD32VW55X_ALL_SDK_INCLUDES ${GDWIFI_SDK_INCLUDES})
endif()

# ===========================================================================
# Wi-Fi support (对标 Makefile: include Wireless.mk)
# ===========================================================================
if(CONFIG_GD32VW55X_WIFI)
    # 导出 Kconfig 定义到编译器 (Makefile 中 .config 自动传递 -D 标志，CMake 中需要显式添加)
    target_compile_definitions(arch PRIVATE CONFIG_GD32VW55X_WIFI)
    list(APPEND GD32VW55X_SDK_DEFINES CONFIG_GD32VW55X_WIFI)

    # 包含 Wi-Fi 配置
    include(${CMAKE_CURRENT_SOURCE_DIR}/gdwifi/Wireless.cmake)

    # 存储 Wi-Fi 编译定义供 board 目标使用
    set_property(GLOBAL PROPERTY GD32VW55X_WIFI_DEFINES
        CFG_RTOS EXEC_USING_STD_PRINTF GDWIFI_NUTTX GDWIFI_NUTTX_NET)

    # 添加 Wi-Fi 源文件
    list(APPEND SRCS ${GDWIFI_SRCS})

    # 收集 Wi-Fi 头文件路径
    list(APPEND GD32VW55X_ALL_SDK_INCLUDES ${GDWIFI_INCLUDES})

    # 添加 Wi-Fi 库路径 (PUBLIC 使路径传递到 nuttx 最终链接)
    target_link_directories(arch PUBLIC ${GDWIFI_LIBPATHS})

    # 添加 Wi-Fi 链接脚本
    if(EXISTS "${GDWIFI_LDSCRIPT}")
        target_link_options(arch PRIVATE -T${GDWIFI_LDSCRIPT})
    endif()

    # 添加 Wi-Fi 链接选项
    target_link_options(arch PRIVATE ${GDWIFI_LINK_FLAGS})

    message(STATUS "Wi-Fi SDK: ${GDWIFI_SDK}")
    message(STATUS "Wi-Fi MAC lib: ${GDWIFI_MACLIB}")
    message(STATUS "Wi-Fi WPA lib: ${GDWIFI_WPASLIB}")
endif()

# ===========================================================================
# BLE support (对标 Makefile: include Bluetooth.mk)
# ===========================================================================
if(CONFIG_GD32VW55X_BLE)
    # 导出 Kconfig 定义到编译器
    target_compile_definitions(arch PRIVATE CONFIG_GD32VW55X_BLE)
    list(APPEND GD32VW55X_SDK_DEFINES CONFIG_GD32VW55X_BLE)

    # 包含 BLE 配置
    include(${CMAKE_CURRENT_SOURCE_DIR}/gdble/Bluetooth.cmake)

    # 添加 BLE 源文件
    list(APPEND SRCS ${GDBLE_SRCS})

    # 收集 BLE 头文件路径
    list(APPEND GD32VW55X_ALL_SDK_INCLUDES ${GDBLE_INCLUDES})

    # BLE 库 (libble_max) 需要在 Wi-Fi 链接组内
    # Wireless.mk 负责将其放入 --start-group/--end-group
    list(APPEND GDWIFI_LIBS ${GDBLE_LIB})

    message(STATUS "BLE lib: ${GDBLE_LIB}")
endif()

# 添加源文件到 arch 目标
target_sources(arch PRIVATE ${SRCS})

# 延迟添加 SDK 头文件路径到 arch 目标
# 使用 cmake_language(DEFER) 在 arch/risc-v/src 处理完后添加 SDK 路径
# 这样 SDK 路径会通过 BEFORE 被添加到 chip 路径之前
# 与 Makefile 中 SDK 路径在 chip 路径之前的顺序一致
if(GD32VW55X_ALL_SDK_INCLUDES)
    set_property(GLOBAL PROPERTY GD32VW55X_SDK_INCLUDES "${GD32VW55X_ALL_SDK_INCLUDES}")
    if(GD32VW55X_SDK_DEFINES)
        set_property(GLOBAL PROPERTY GD32VW55X_SDK_DEFINES "${GD32VW55X_SDK_DEFINES}")
    endif()
    # 延迟到顶层目录处理完后执行，确保 board 目标已创建
    cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}"
        CALL _gd32vw55x_add_sdk_includes)
endif()

# 链接预编译库 (对标 Makefile: EXTRA_LIBS += --start-group ... --end-group)
# 使用 NUTTX_EXTRA_LIBRARIES 将预编译库加入与 libgcc 相同的链接组
# 这样预编译库可以解析 __riscv_save_N/__riscv_restore_N 等符号
if(CONFIG_GD32VW55X_WIFI)
    set_property(GLOBAL APPEND PROPERTY NUTTX_EXTRA_LIBRARIES
        -Wl,--start-group
        ${GDWIFI_LIBS}
        -Wl,--end-group
    )
endif()

endif()
