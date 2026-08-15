/****************************************************************************
 * chips/gd32vw55x/hardware/gd32vw55x_rcu.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#ifndef __CHIPS_GD32VW55X_HARDWARE_GD32VW55X_RCU_H
#define __CHIPS_GD32VW55X_HARDWARE_GD32VW55X_RCU_H

#include "gd32vw55x_memorymap.h"

#define GD32VW55X_RCU_CTL           (GD32VW55X_RCU_BASE + 0x0000)
#define GD32VW55X_RCU_PLL           (GD32VW55X_RCU_BASE + 0x0004)
#define GD32VW55X_RCU_CFG0          (GD32VW55X_RCU_BASE + 0x0008)
#define GD32VW55X_RCU_INT           (GD32VW55X_RCU_BASE + 0x000c)
#define GD32VW55X_RCU_AHB1RST       (GD32VW55X_RCU_BASE + 0x0010)
#define GD32VW55X_RCU_AHB2RST       (GD32VW55X_RCU_BASE + 0x0014)
#define GD32VW55X_RCU_APB1RST       (GD32VW55X_RCU_BASE + 0x0020)
#define GD32VW55X_RCU_APB2RST       (GD32VW55X_RCU_BASE + 0x0024)
#define GD32VW55X_RCU_AHB1EN        (GD32VW55X_RCU_BASE + 0x0030)
#define GD32VW55X_RCU_AHB2EN        (GD32VW55X_RCU_BASE + 0x0034)
#define GD32VW55X_RCU_APB1EN        (GD32VW55X_RCU_BASE + 0x0040)
#define GD32VW55X_RCU_APB2EN        (GD32VW55X_RCU_BASE + 0x0044)
#define GD32VW55X_RCU_RSTSCK        (GD32VW55X_RCU_BASE + 0x0074)
#define GD32VW55X_RCU_PLLDIGCFG0    (GD32VW55X_RCU_BASE + 0x0084)
#define GD32VW55X_RCU_CFG1          (GD32VW55X_RCU_BASE + 0x008c)
#define GD32VW55X_RCU_PLLDIGCFG1    (GD32VW55X_RCU_BASE + 0x0094)

#define RCU_CTL_IRC16MEN            (1 << 0)
#define RCU_CTL_HXTALEN             (1 << 16)
#define RCU_CTL_HXTALSTB            (1 << 17)
#define RCU_CTL_PLLDIGPU            (1 << 20)
#define RCU_CTL_PLLDIGEN            (1 << 21)
#define RCU_CTL_RFCKMEN             (1 << 22)
#define RCU_CTL_PLLDIGSTB           (1 << 23)
#define RCU_CTL_HXTALPU             (1 << 28)
#define RCU_CTL_HXTALREADY          (1 << 31)

#define RCU_PLL_PLLDIGSEL           (1 << 15)

#define RCU_CFG0_SCS_MASK           (3 << 0)
#define RCU_CFG0_SCS_IRC16M         (0 << 0)
#define RCU_CFG0_SCS_HXTAL          (1 << 0)
#define RCU_CFG0_SCS_PLLDIG         (2 << 0)
#define RCU_CFG0_SCSS_MASK          (3 << 2)
#define RCU_CFG0_SCSS_PLLDIG        (2 << 2)
#define RCU_CFG0_AHBPSC_MASK        (15 << 4)
#define RCU_CFG0_AHBPSC_DIV1        (0 << 4)
#define RCU_CFG0_APB1PSC_MASK       (7 << 10)
#define RCU_CFG0_APB1PSC_DIV2       (4 << 10)
#define RCU_CFG0_APB2PSC_MASK       (7 << 13)
#define RCU_CFG0_APB2PSC_DIV1       (0 << 13)

#define RCU_CFG1_RFPLLCALEN         (1 << 14)
#define RCU_CFG1_RFPLLPU            (1 << 16)
#define RCU_CFG1_BGPU               (1 << 19)

#define RCU_PLLDIGCFG0_OSEL_MASK    (3 << 24)
#define RCU_PLLDIGCFG0_OSEL_480M    (3 << 24)
#define RCU_PLLDIGCFG0_SYSDIV_MASK  (0x3f << 26)
#define RCU_PLLDIGCFG0_SYSDIV(n)    (((n) & 0x3f) << 26)

#define RCU_AHB1EN_PAEN             (1 << 0)
#define RCU_AHB1EN_PBEN             (1 << 1)
#define RCU_AHB1EN_PCEN             (1 << 2)
#define RCU_AHB1EN_DMAEN            (1 << 21)

#define RCU_APB1EN_TIMER1EN         (1 << 0)
#define RCU_APB1EN_TIMER2EN         (1 << 1)
#define RCU_APB1EN_UART1EN          (1 << 17)
#define RCU_APB1EN_USART0EN         (1 << 18)
#define RCU_APB1EN_I2C0EN           (1 << 21)
#define RCU_APB1EN_I2C1EN           (1 << 22)

#define RCU_APB2EN_TIMER0EN         (1 << 0)
#define RCU_APB2EN_UART2EN          (1 << 4)
#define RCU_APB2EN_ADCEN            (1 << 8)
#define RCU_APB2EN_SPIEN            (1 << 12)

#endif
