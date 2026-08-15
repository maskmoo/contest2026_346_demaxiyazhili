# chips/gd32vw55x/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
#

if(CONFIG_ARCH_CHIP_GD32VW55X)

set(SRCS
    gd32vw55x_start.c
    gd32vw55x_irq.c
    gd32vw55x_lowputc.c
    gd32vw55x_serial.c
    gd32vw55x_gpio.c
    gd32vw55x_clockconfig.c
    gd32vw55x_allocateheap.c
    gd32vw55x_timerisr.c
)

if(CONFIG_GD32VW55X_SPI)
    list(APPEND SRCS gd32vw55x_spi.c)
endif()

if(CONFIG_GD32VW55X_I2C)
    list(APPEND SRCS gd32vw55x_i2c.c)
endif()

target_sources(arch PRIVATE ${SRCS})

endif()
