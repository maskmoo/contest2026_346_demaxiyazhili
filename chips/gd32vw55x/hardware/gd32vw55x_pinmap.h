/****************************************************************************
 * chips/gd32vw55x/hardware/gd32vw55x_pinmap.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * GD32VW553H-EVAL pin mapping for UART0 (console).
 * UART0: TX=PB15 (AF7), RX=PA8 (AF3)
 *
 ****************************************************************************/

#ifndef __CHIPS_GD32VW55X_HARDWARE_GD32VW55X_PINMAP_H
#define __CHIPS_GD32VW55X_HARDWARE_GD32VW55X_PINMAP_H

#include <nuttx/config.h>

/* UART0: TX=PB15 (AF7), RX=PA8 (AF3) */

#define BOARD_USART0_TX_GPIO   GD32VW55X_GPIOB_BASE
#define BOARD_USART0_TX_PIN    15
#define BOARD_USART0_TX_AF     7
#define BOARD_USART0_RX_GPIO   GD32VW55X_GPIOA_BASE
#define BOARD_USART0_RX_PIN    8
#define BOARD_USART0_RX_AF     3

/* UART1: TX=PB15 (AF7), RX=PA8 (AF3) — same pins as USART0 */

#define BOARD_UART1_TX_GPIO    GD32VW55X_GPIOB_BASE
#define BOARD_UART1_TX_PIN     15
#define BOARD_UART1_TX_AF      7
#define BOARD_UART1_RX_GPIO    GD32VW55X_GPIOA_BASE
#define BOARD_UART1_RX_PIN     8
#define BOARD_UART1_RX_AF      3

#endif
