/****************************************************************************
 * chips/gd32vw55x/hardware/gd32vw55x_uart.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#ifndef __CHIPS_GD32VW55X_HARDWARE_GD32VW55X_UART_H
#define __CHIPS_GD32VW55X_HARDWARE_GD32VW55X_UART_H

#include "gd32vw55x_memorymap.h"

#define GD32VW55X_UART_CTL0_OFFSET    0x0000
#define GD32VW55X_UART_CTL1_OFFSET    0x0004
#define GD32VW55X_UART_CTL2_OFFSET    0x0008
#define GD32VW55X_UART_BAUD_OFFSET    0x000c
#define GD32VW55X_UART_GP_OFFSET      0x0010
#define GD32VW55X_UART_RT_OFFSET      0x0014
#define GD32VW55X_UART_CMD_OFFSET     0x0018
#define GD32VW55X_UART_STAT_OFFSET    0x001c
#define GD32VW55X_UART_INTC_OFFSET    0x0020
#define GD32VW55X_UART_RDATA_OFFSET   0x0024
#define GD32VW55X_UART_TDATA_OFFSET   0x0028
#define GD32VW55X_UART_CHC_OFFSET     0x00c0

#define UART_CTL0_UEN                 (1 << 0)
#define UART_CTL0_REN                 (1 << 2)
#define UART_CTL0_TEN                 (1 << 3)
#define UART_CTL0_IDLEIE              (1 << 4)
#define UART_CTL0_RBNEIE              (1 << 5)
#define UART_CTL0_TCIE                (1 << 6)
#define UART_CTL0_TBEIE               (1 << 7)
#define UART_CTL0_PERRIE              (1 << 8)
#define UART_CTL0_PM                  (1 << 9)
#define UART_CTL0_PCEN                (1 << 10)
#define UART_CTL0_WL0                 (1 << 12)
#define UART_CTL0_WL1                 (1 << 28)

#define UART_CTL1_STB_SHIFT           12
#define UART_CTL1_STB_MASK            (3 << UART_CTL1_STB_SHIFT)
#define UART_CTL1_STB_1               (0 << UART_CTL1_STB_SHIFT)
#define UART_CTL1_STB_2               (2 << UART_CTL1_STB_SHIFT)

#define UART_CTL2_ERRIE               (1 << 0)
#define UART_CTL2_RTSEN               (1 << 8)
#define UART_CTL2_CTSEN               (1 << 9)
#define UART_CTL2_DENR                (1 << 6)
#define UART_CTL2_DENT                (1 << 7)

#define UART_STAT_PERR                (1 << 0)
#define UART_STAT_FERR                (1 << 1)
#define UART_STAT_NERR                (1 << 2)
#define UART_STAT_ORERR               (1 << 3)
#define UART_STAT_IDLEF               (1 << 4)
#define UART_STAT_RBNE                (1 << 5)
#define UART_STAT_TC                  (1 << 6)
#define UART_STAT_TBE                 (1 << 7)

#define UART_INTC_PEC                 (1 << 0)
#define UART_INTC_FEC                 (1 << 1)
#define UART_INTC_NEC                 (1 << 2)
#define UART_INTC_OREC                (1 << 3)
#define UART_INTC_IDLEC               (1 << 4)
#define UART_INTC_TCC                 (1 << 6)

#endif
