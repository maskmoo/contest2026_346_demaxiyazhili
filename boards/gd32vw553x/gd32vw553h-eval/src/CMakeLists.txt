# boards/risc-v/gd32vw55x/gd32vw553h-eval/src/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
#

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

target_sources(board PRIVATE ${SRCS})
