/****************************************************************************
 * boards/risc-v/gd32vw55x/gd32vw553h-eval/include/board.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __BOARDS_RISCV_GD32VW55X_GD32VW553H_EVAL_INCLUDE_BOARD_H
#define __BOARDS_RISCV_GD32VW55X_GD32VW553H_EVAL_INCLUDE_BOARD_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Clocking: 40 MHz HXTAL -> PLLDIG -> 160 MHz SYSCLK (see
 * gd32vw55x_clockconfig.c).  These are informational for board code.
 */

#define BOARD_HXTAL_FREQUENCY   40000000
#define BOARD_IRC32K_FREQUENCY  32000     /* Internal 32 kHz RC (FWDGT, RTC) */
#define BOARD_SYSCLK_FREQUENCY  160000000

/* USART pin muxing.  The GD32VW553H-EVAL wires USART (PB15/PA8) to the
 * on-board debug connector.  Pin/AF assignments follow the vendor SDK.
 */

#define BOARD_USART_TX_GPIO     GD32VW55X_GPIOB_BASE
#define BOARD_USART_TX_PIN      15
#define BOARD_USART_TX_AF       7
#define BOARD_USART_RX_GPIO     GD32VW55X_GPIOA_BASE
#define BOARD_USART_RX_PIN      8
#define BOARD_USART_RX_AF       3

/* Peripheral pin selection *************************************************/

/* The pin options come from arch/risc-v/src/gd32vw55x/hardware/
 * gd32vw55x_pinmap.h.
 */

#ifdef CONFIG_GD32VW55X_SPI
#  define GPIO_SPI_SCK   GPIO_SPI_SCK_2    /* PA11, AF5 */
#  define GPIO_SPI_MISO  GPIO_SPI_MISO_2   /* PA10, AF5 */
#  define GPIO_SPI_MOSI  GPIO_SPI_MOSI_2   /* PA9, AF5 */

/* SPI chip select for LCD: a plain GPIO output on PA12,
 * driven by gd32_spi0select().  Active low, idles high.
 */

#  define GPIO_SPI0_CSPIN (GPIO_CFG_MODE_OUTPUT | GPIO_CFG_PUPD_NONE | \
                           GPIO_CFG_PP | GPIO_CFG_SPEED_MAX | \
                           GPIO_CFG_OUTPUT_SET | GPIO_CFG_PORT_A | \
                           GPIO_CFG_PIN_12)
#endif

/* I2C0 uses PA2 (SCL) / PA3 (SDA) on AF4, so a sensor such as the SHT3x
 * can be wired there directly.
 */

#ifdef CONFIG_GD32VW55X_I2C0
#  define GPIO_I2C0_SCL  GPIO_I2C0_SCL_1   /* PA2, AF4 */
#  define GPIO_I2C0_SDA  GPIO_I2C0_SDA_1   /* PA3, AF4 */
#endif

/* Note: I2C1 (PB12/PB13) conflicts with LCD_RESET/LCD_D/C, so it is not
 * available on this board when LCD is used.
 */

/* PWM.  The board bringup registers /dev/pwm0 on TIMER1.  Route its channel
 * 2 (TIMER1_CH2) to PB11 for IR output (AF1; TIMER1 lives on AF1).
 */

#ifdef CONFIG_GD32VW55X_PWM
#  define GPIO_TIMER1_CH2OUT (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_NONE | \
                              GPIO_CFG_PP | GPIO_CFG_SPEED_MAX | \
                              GPIO_CFG_AF_1 | GPIO_CFG_PORT_B | GPIO_CFG_PIN_11)
#endif

/* ADC.  Route ADC channel 0 (ADC_IN0) to PA1 on the board header so an
 * analog signal can be applied there.
 */

#ifdef CONFIG_GD32VW55X_ADC
#  define GPIO_ADC_IN0 (GPIO_CFG_MODE_ANALOG | GPIO_CFG_PUPD_NONE | \
                        GPIO_CFG_PORT_A | GPIO_CFG_PIN_1)
#endif

/* LEDs *********************************************************************/

/* The GD32VW553H-EVAL has three LEDs on GPIOA, driven push-pull and
 * active HIGH.
 *
 *   LED1  PA4
 *   LED2  PA5
 *   LED3  PA6
 */

#define GPIO_LED1 (GPIO_CFG_MODE_OUTPUT | GPIO_CFG_PUPD_NONE | GPIO_CFG_PP | \
                   GPIO_CFG_SPEED_MAX | GPIO_CFG_OUTPUT_RESET | \
                   GPIO_CFG_PORT_A | GPIO_CFG_PIN_4)
#define GPIO_LED2 (GPIO_CFG_MODE_OUTPUT | GPIO_CFG_PUPD_NONE | GPIO_CFG_PP | \
                   GPIO_CFG_SPEED_MAX | GPIO_CFG_OUTPUT_RESET | \
                   GPIO_CFG_PORT_A | GPIO_CFG_PIN_5)
#define GPIO_LED3 (GPIO_CFG_MODE_OUTPUT | GPIO_CFG_PUPD_NONE | GPIO_CFG_PP | \
                   GPIO_CFG_SPEED_MAX | GPIO_CFG_OUTPUT_RESET | \
                   GPIO_CFG_PORT_A | GPIO_CFG_PIN_6)

/* LED index values for use with board_userled() */

#define BOARD_LED1        0
#define BOARD_LED2        1
#define BOARD_LED3        2
#define BOARD_NLEDS       3

/* LED bits for use with board_userled_all() */

#define BOARD_LED1_BIT    (1 << BOARD_LED1)
#define BOARD_LED2_BIT    (1 << BOARD_LED2)
#define BOARD_LED3_BIT    (1 << BOARD_LED3)

/* If CONFIG_ARCH_LEDS is defined, the LEDs are used by the OS to signal
 * its state, and are not available to the application.  Otherwise they are
 * fully under application control.
 *
 *   SYMBOL              MEANING                   LED1  LED2  LED3
 *   ------------------  ------------------------  ----  ----  ----
 */

#define LED_STARTED       0  /* NuttX has been started  ON   OFF   OFF */
#define LED_HEAPALLOCATE  1  /* Heap has been allocated ON   OFF   OFF */
#define LED_IRQSENABLED   2  /* Interrupts enabled      ON   OFF   OFF */
#define LED_STACKCREATED  3  /* Idle stack created      ON   ON    OFF */
#define LED_INIRQ         4  /* In an interrupt         N/C  N/C   N/C */
#define LED_SIGNAL        5  /* In a signal handler     N/C  N/C   N/C */
#define LED_ASSERTION     6  /* An assertion failed     N/C  N/C   ON  */
#define LED_PANIC         7  /* The system has crashed  N/C  N/C   BLINK */

#endif /* __BOARDS_RISCV_GD32VW55X_GD32VW553H_EVAL_INCLUDE_BOARD_H */
