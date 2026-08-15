/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_config.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#ifndef __CHIPS_GD32VW55X_GD32VW55X_CONFIG_H
#define __CHIPS_GD32VW55X_GD32VW55X_CONFIG_H

#include <nuttx/config.h>
#include <arch/chip/chip.h>
#include <arch/board/board.h>

#undef HAVE_UART_DEVICE
#if defined(CONFIG_GD32VW55X_USART0) || defined(CONFIG_GD32VW55X_UART1) || \
    defined(CONFIG_GD32VW55X_UART2)
#  define HAVE_UART_DEVICE 1
#endif

#undef HAVE_SERIAL_CONSOLE
#if defined(CONFIG_USART0_SERIAL_CONSOLE) && defined(CONFIG_GD32VW55X_USART0)
#  undef CONFIG_UART1_SERIAL_CONSOLE
#  undef CONFIG_UART2_SERIAL_CONSOLE
#  define HAVE_SERIAL_CONSOLE 1
#elif defined(CONFIG_UART1_SERIAL_CONSOLE) && defined(CONFIG_GD32VW55X_UART1)
#  undef CONFIG_USART0_SERIAL_CONSOLE
#  undef CONFIG_UART2_SERIAL_CONSOLE
#  define HAVE_SERIAL_CONSOLE 1
#elif defined(CONFIG_UART2_SERIAL_CONSOLE) && defined(CONFIG_GD32VW55X_UART2)
#  undef CONFIG_USART0_SERIAL_CONSOLE
#  undef CONFIG_UART1_SERIAL_CONSOLE
#  define HAVE_SERIAL_CONSOLE 1
#else
#  undef CONFIG_USART0_SERIAL_CONSOLE
#  undef CONFIG_UART1_SERIAL_CONSOLE
#  undef CONFIG_UART2_SERIAL_CONSOLE
#endif

#endif
