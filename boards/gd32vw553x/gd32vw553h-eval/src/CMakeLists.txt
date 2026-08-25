############################################################################
# boards/gd32vw553x/gd32vw553h-eval/src/CMakeLists.txt
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

set(SRCS
    gd32_boot.c
    gd32_bringup.c
)

if(CONFIG_ARCH_LEDS)
    list(APPEND SRCS gd32_autoleds.c)
elseif(CONFIG_USERLED)
    list(APPEND SRCS gd32_userleds.c)
endif()

if(CONFIG_BOARDCTL)
    list(APPEND SRCS gd32_appinit.c)
endif()

if(CONFIG_BOARDCTL_RESET)
    list(APPEND SRCS gd32_reset.c)
endif()

if(CONFIG_GD32VW55X_SPI)
    list(APPEND SRCS gd32_spi.c)
endif()

if(CONFIG_BOARD_LCD_ENABLE)
    list(APPEND SRCS gd32_lcd.c)
endif()

target_sources(board PRIVATE ${SRCS})
