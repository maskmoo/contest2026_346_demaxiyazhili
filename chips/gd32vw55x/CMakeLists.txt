# chips/gd32vw55x/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
#

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

target_sources(arch PRIVATE ${SRCS})

endif()
