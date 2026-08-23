############################################################################
# chips/gd32vw55x/gdwifi/Wireless.cmake
#
# SPDX-License-Identifier: Apache-2.0
#
# Compiles the open-source layers of the GD32VW55x SDK Wi-Fi stack
# (wifi_manager, lwIP 2.2.0 + port, util, platform bsp, SPL subset) into
# libarch and links the prebuilt BSD-3 MAC/RF/supplicant libraries.
# CMake equivalent of Wireless.mk
#
# The OS binding is gdwifi/wrapper_nuttx.c (sys_* facade).
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

# 注意：当此文件被 include() 包含时，CMAKE_CURRENT_SOURCE_DIR 是父 CMakeLists.txt 所在的目录
# 即 chips/gd32vw55x/，而不是 chips/gd32vw55x/gdwifi/
# 所以我们需要使用相对于父目录的路径
set(GDWIFI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gdwifi")

# 编译选项 (对标 Makefile: CFLAGS += ...)
# Defines: RTOS build + supplicant, never PLATFORM_OS_FREERTOS
# 注意：必须使用 target_compile_definitions 而非 add_compile_definitions
# 因为 arch 目标在父作用域创建，add_compile_definitions 只影响当前子目录
target_compile_definitions(arch PRIVATE
    CFG_RTOS
    EXEC_USING_STD_PRINTF
    GDWIFI_NUTTX
    GDWIFI_NUTTX_NET
)

# 抑制 SDK 源码警告 (对标 Makefile: CFLAGS += -Wno-...)
target_compile_options(arch PRIVATE
    -Wno-undef
    -Wno-format
    -Wno-shadow
    -Wno-address
    -Wno-unused-function
    -Wno-strict-prototypes
    -Wno-array-parameter
)

# 包含路径 (对标 Makefile: INCLUDES += ...)
# 我们的 patched config 目录必须在最前面
set(GDWIFI_INCLUDES
    ${GDWIFI_DIR}/config
    ${GDWIFI_DIR}
    ${ARCH_SRCDIR}/${SDKDIR}/config
    ${ARCH_SRCDIR}/${MSDK}/plf/riscv/gd32vw55x
    ${ARCH_SRCDIR}/${MSDK}/plf/riscv/NMSIS/Core/Include
    ${ARCH_SRCDIR}/${MSDK}/plf/riscv/arch
    ${ARCH_SRCDIR}/${MSDK}/plf/riscv/arch/boot
    ${ARCH_SRCDIR}/${MSDK}/plf/riscv/arch/lib
    ${ARCH_SRCDIR}/${MSDK}/plf/riscv/arch/ll
    ${ARCH_SRCDIR}/${MSDK}/plf/riscv/arch/compiler
    ${ARCH_SRCDIR}/${MSDK}/plf/GD32VW55x_standard_peripheral
    ${ARCH_SRCDIR}/${MSDK}/plf/GD32VW55x_standard_peripheral/Include
    ${ARCH_SRCDIR}/${MSDK}/plf/src
    ${ARCH_SRCDIR}/${MSDK}/plf/src/dma
    ${ARCH_SRCDIR}/${MSDK}/plf/src/nvds
    ${ARCH_SRCDIR}/${MSDK}/plf/src/raw_flash
    ${ARCH_SRCDIR}/${MSDK}/plf/src/reg
    ${ARCH_SRCDIR}/${MSDK}/plf/src/rf
    ${ARCH_SRCDIR}/${MSDK}/plf/src/time
    ${ARCH_SRCDIR}/${MSDK}/plf/src/trng
    ${ARCH_SRCDIR}/${MSDK}/plf/src/uart
    ${ARCH_SRCDIR}/${MSDK}/plf/src/wdt
    ${ARCH_SRCDIR}/${MSDK}/macsw/export
    ${ARCH_SRCDIR}/${MSDK}/macsw/import
    ${ARCH_SRCDIR}/${MSDK}/util/include
    ${ARCH_SRCDIR}/${MSDK}/wifi_manager
    ${ARCH_SRCDIR}/${MSDK}/wifi_manager/wpas
    ${ARCH_SRCDIR}/${MSDK}/wpa_supplicant/src
    ${ARCH_SRCDIR}/${MSDK}/wpa_supplicant/src/utils
    ${ARCH_SRCDIR}/${MSDK}/wpa_supplicant/src/crypto
    ${ARCH_SRCDIR}/${MSDK}/rtos/rtos_wrapper
    ${ARCH_SRCDIR}/${MSDK}/plf/riscv/NMSIS/DSP/Include
    ${ARCH_SRCDIR}/${MSDK}/mbedtls/mbedtls/include
    ${ARCH_SRCDIR}/${MSDK}/mbedtls/mbedtls/library
    ${ARCH_SRCDIR}/${MSDK}/mbedtls/mbedtls/tests/include/spe
    # NuttX 网络兼容层
    ${GDWIFI_DIR}/net_compat
)

# 我们的 NuttX 侧 glue (对标 Makefile: CHIP_CSRCS += ...)
set(GDWIFI_SRCS
    ${GDWIFI_DIR}/wrapper_nuttx.c
    ${GDWIFI_DIR}/gdwifi_glue.c
    ${GDWIFI_DIR}/gdwifi_newlib_compat.c
    ${GDWIFI_DIR}/gdwifi_netdev.c
    ${GDWIFI_DIR}/gdwifi_netif_compat.c
)

# wifi_manager (STA) (对标 Makefile: GDWIFI_MGR = ...; CHIP_CSRCS += ...)
set(GDWIFI_MGR ${ARCH_SRCDIR}/${MSDK}/wifi_manager/)
list(APPEND GDWIFI_SRCS
    ${GDWIFI_MGR}/wifi_eloop.c
    ${GDWIFI_MGR}/wifi_init.c
    ${GDWIFI_MGR}/wifi_management.c
    ${GDWIFI_MGR}/wifi_net_ip.c
    ${GDWIFI_MGR}/wifi_netlink.c
    ${GDWIFI_MGR}/wifi_vif.c
    ${GDWIFI_MGR}/wifi_wpa.c
)

# util (对标 Makefile: GDWIFI_UTIL = ...; CHIP_CSRCS += ...)
set(GDWIFI_UTIL ${ARCH_SRCDIR}/${MSDK}/util/src/)
list(APPEND GDWIFI_SRCS
    ${GDWIFI_UTIL}/debug_print.c
    ${GDWIFI_UTIL}/crc.c
    ${GDWIFI_UTIL}/dlist.c
    ${GDWIFI_UTIL}/slist.c
    ${GDWIFI_UTIL}/trace_ext.c
    ${GDWIFI_UTIL}/util.c
    ${GDWIFI_UTIL}/cyclic_buffer.c
    ${GDWIFI_UTIL}/user_setting.c
    ${GDWIFI_UTIL}/aes_ecb.c
)

# plf/src platform pieces (对标 Makefile: GDWIFI_PLF = ...; CHIP_CSRCS += ...)
set(GDWIFI_PLF ${ARCH_SRCDIR}/${MSDK}/plf/src/)
list(APPEND GDWIFI_SRCS
    ${GDWIFI_PLF}/gd32vw55x_platform.c
    ${GDWIFI_PLF}/init_rom.c
    ${GDWIFI_PLF}/plf_assert.c
    ${GDWIFI_PLF}/wakelock.c
    ${GDWIFI_PLF}/dsp.c
    ${GDWIFI_PLF}/time/systime.c
    ${GDWIFI_PLF}/trng/trng.c
    ${GDWIFI_PLF}/nvds/nvds_flash.c
    ${GDWIFI_PLF}/raw_flash/raw_flash_api.c
    ${GDWIFI_PLF}/dma/dma.c
)

# mbedTLS 3.6 compiled from source (对标 Makefile: GDWIFI_MBEDTLS_SRCS)
file(GLOB GDWIFI_MBEDTLS_SRCS "${GDWIFI_SDK}/MSDK/mbedtls/mbedtls/library/*.c")
# 移除 net_sockets.c (与 NuttX socket headers 冲突)
list(FILTER GDWIFI_MBEDTLS_SRCS EXCLUDE REGEX "net_sockets\\.c$")
list(APPEND GDWIFI_SRCS ${GDWIFI_MBEDTLS_SRCS})

# SDK interrupt handler bodies (对标 Makefile: CHIP_CSRCS += ...)
list(APPEND GDWIFI_SRCS
    ${ARCH_SRCDIR}/${MSDK}/plf/riscv/gd32vw55x/gd32vw55x_it.c
)

# SPL subset (对标 Makefile: SPL_SRCS = ...; CHIP_CSRCS += ...)
# Note: the SPL eclic driver is NOT compiled; eclic_irq_enable/disable are
# provided by gdwifi_glue.c on top of the NuttX IRQ layer so the SDK cannot
# reprogram ECLIC levels behind the kernel's back.
set(SPL_SRCS
    rcu gpio fmc efuse trng crc dma pmu syscfg exti timer eclic
    rtc cau cau_aes hau hau_sha_md5 pkcau
)
foreach(SPL_SRC ${SPL_SRCS})
    list(APPEND GDWIFI_SRCS
        ${ARCH_SRCDIR}/${MSDK}/plf/GD32VW55x_standard_peripheral/Source/gd32vw55x_${SPL_SRC}.c
    )
endforeach()

# 链接脚本 (对标 Makefile: ARCHSCRIPT += ...)
set(GDWIFI_LDSCRIPT "${GDWIFI_SDK}/ROM-EXPORT/symbol/rom_symbol_m.gcc")

# 预编译库路径 (对标 Makefile: EXTRA_LIBPATHS += ...)
set(GDWIFI_LIBPATHS
    ${ARCH_SRCDIR}/${MSDK}/lib
    ${ARCH_SRCDIR}/${MSDK}/plf/riscv/NMSIS/Library/DSP/GCC
)

# MAC firmware variant per Kconfig (对标 Makefile: ifeq ... endif)
if(CONFIG_GD32VW55X_WIFI_LIB_MINSRAM)
    set(GDWIFI_MACLIB "wifi_minsram")
elseif(CONFIG_GD32VW55X_WIFI_LIB_MESH_SMART)
    set(GDWIFI_MACLIB "wifi_mesh_smart")
elseif(CONFIG_GD32VW55X_WIFI_LIB_MULTISTREAM)
    set(GDWIFI_MACLIB "wifi_multistream")
elseif(CONFIG_GD32VW55X_WIFI_LIB_SOFTAP_MANY)
    set(GDWIFI_MACLIB "wifi_softap_many_clients")
else()
    set(GDWIFI_MACLIB "wifi")
endif()

# WPA supplicant library (对标 Makefile: ifeq ... endif)
if(CONFIG_GD32VW55X_WIFI_WPS_EAP_TLS)
    set(GDWIFI_WPASLIB "wpas_wps_eap-tls")
else()
    set(GDWIFI_WPASLIB "wpas")
endif()

# 预编译库 (对标 Makefile: EXTRA_LIBS += ...)
set(GDWIFI_LIBS
    ${GDWIFI_MACLIB}
    ${GDWIFI_WPASLIB}
    rf
    nmsis_dsp_rv32imafc
)

# 链接选项 (对标 Makefile: LDFLAGS += --wrap=setvbuf)
set(GDWIFI_LINK_FLAGS
    -Wl,--wrap=setvbuf
)

# 导出变量到父作用域
set(GDWIFI_SRCS "${GDWIFI_SRCS}" PARENT_SCOPE)
set(GDWIFI_INCLUDES "${GDWIFI_INCLUDES}" PARENT_SCOPE)
set(GDWIFI_LIBPATHS "${GDWIFI_LIBPATHS}" PARENT_SCOPE)
set(GDWIFI_LIBS "${GDWIFI_LIBS}" PARENT_SCOPE)
set(GDWIFI_LDSCRIPT "${GDWIFI_LDSCRIPT}" PARENT_SCOPE)
set(GDWIFI_LINK_FLAGS "${GDWIFI_LINK_FLAGS}" PARENT_SCOPE)
set(GDWIFI_MACLIB "${GDWIFI_MACLIB}" PARENT_SCOPE)
set(GDWIFI_WPASLIB "${GDWIFI_WPASLIB}" PARENT_SCOPE)
set(GDWIFI_PLF "${GDWIFI_PLF}" PARENT_SCOPE)
