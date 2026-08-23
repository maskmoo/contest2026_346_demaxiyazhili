############################################################################
# boards/gd32vw553x/gd32vw553h-eval/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
#
# Board-level CMakeLists.txt for GD32VW553H-EVAL board
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

# 板级源文件
set(BOARD_SRCS
    src/gd32vw553h-eval_board.c
)

# 板级头文件路径
set(BOARD_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 板级编译选项
set(BOARD_CFLAGS
    -DGD32VW553H_EVAL
)

# 板级链接脚本
set(BOARD_LDSCRIPT
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/gd32vw553h-eval.ld
)

# 根据配置添加功能模块
if(CONFIG_GD32VW55X_WIFI)
    # Wi-Fi 相关源文件
    list(APPEND BOARD_SRCS
        src/gd32vw55x_wifi_board.c
    )
endif()

if(CONFIG_GD32VW55X_BLE)
    # BLE 相关源文件
    list(APPEND BOARD_SRCS
        src/gd32vw55x_ble_board.c
    )
endif()

# 导出板级配置
set(BOARD_SRCS "${BOARD_SRCS}" PARENT_SCOPE)
set(BOARD_INCLUDES "${BOARD_INCLUDES}" PARENT_SCOPE)
set(BOARD_CFLAGS "${BOARD_CFLAGS}" PARENT_SCOPE)
set(BOARD_LDSCRIPT "${BOARD_LDSCRIPT}" PARENT_SCOPE)

# 包含 src 子目录 (对标 Makefile: include src/Makefile)
add_subdirectory(src)
