/****************************************************************************
 * boards/risc-v/gd32vw55x/gd32vw553h-eval/include/board.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#ifndef __BOARDS_RISCV_GD32VW55X_GD32VW553H_EVAL_INCLUDE_BOARD_H
#define __BOARDS_RISCV_GD32VW55X_GD32VW553H_EVAL_INCLUDE_BOARD_H

#include "gd32vw55x_memorymap.h"

/****************************************************************************
 * Pin encoding for board GPIOs: port_base | (pin << 8) | mode
 * port_base: full 32-bit peripheral base address
 * pin: 0-15
 * mode: 0=input, 1=output
 ****************************************************************************/

/* LED definitions **********************************************************/

/*   LED1: PA4
 *   LED2: PA5
 *   LED3: PA5 (requires shorting JP3(3,5)(4,6) and JP5(2,3))
 */

#define BOARD_LED1  0
#define BOARD_LED2  1
#define BOARD_LED3  2
#define BOARD_NLEDS 3

#define GPIO_LED1  (GD32VW55X_GPIOA_BASE | (4 << 8) | 1)
#define GPIO_LED2  (GD32VW55X_GPIOA_BASE | (5 << 8) | 1)
#define GPIO_LED3  (GD32VW55X_GPIOA_BASE | (5 << 8) | 1)

/* Button definitions *******************************************************/

/*   KEY: PA0 (active low, with pull-up) */

#define GPIO_BTN_KEY  (GD32VW55X_GPIOA_BASE | (0 << 8) | 0)

/* UART0 (USART0): TX=PB15, RX=PA8 — used as NSH console */

#define BOARD_USART0_TX_GPIO   GD32VW55X_GPIOB_BASE
#define BOARD_USART0_TX_PIN    15
#define BOARD_USART0_TX_AF     7
#define BOARD_USART0_RX_GPIO   GD32VW55X_GPIOA_BASE
#define BOARD_USART0_RX_PIN    8
#define BOARD_USART0_RX_AF     3

/* UART1: TX=PB15, RX=PA8 — same pins */

#define BOARD_UART1_TX_GPIO    GD32VW55X_GPIOB_BASE
#define BOARD_UART1_TX_PIN     15
#define BOARD_UART1_TX_AF      7
#define BOARD_UART1_RX_GPIO    GD32VW55X_GPIOA_BASE
#define BOARD_UART1_RX_PIN     8
#define BOARD_UART1_RX_AF      3

/* SPI1 (LCD): SCK=PA11, MOSI=PA9, MISO=PA10, NSS=PA12 */

#define GPIO_LCD_DC     (GD32VW55X_GPIOB_BASE | (13 << 8) | 1)
#define GPIO_LCD_RESET  (GD32VW55X_GPIOB_BASE | (12 << 8) | 1)

#endif /* __BOARDS_RISCV_GD32VW55X_GD32VW553H_EVAL_INCLUDE_BOARD_H */
