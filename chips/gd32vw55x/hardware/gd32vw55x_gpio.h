/****************************************************************************
 * chips/gd32vw55x/hardware/gd32vw55x_gpio.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#ifndef __CHIPS_GD32VW55X_HARDWARE_GD32VW55X_GPIO_H
#define __CHIPS_GD32VW55X_HARDWARE_GD32VW55X_GPIO_H

#include "gd32vw55x_memorymap.h"

#define GD32VW55X_NGPIO_PORTS          3

#define GD32VW55X_GPIOA_PINMASK        0xffff
#define GD32VW55X_GPIOB_PINMASK        0xb81f
#define GD32VW55X_GPIOC_PINMASK        0xe100

#define GD32VW55X_GPIO_CTL_OFFSET      0x0000
#define GD32VW55X_GPIO_OMODE_OFFSET    0x0004
#define GD32VW55X_GPIO_OSPD_OFFSET     0x0008
#define GD32VW55X_GPIO_PUD_OFFSET      0x000c
#define GD32VW55X_GPIO_ISTAT_OFFSET    0x0010
#define GD32VW55X_GPIO_OCTL_OFFSET     0x0014
#define GD32VW55X_GPIO_BOP_OFFSET      0x0018
#define GD32VW55X_GPIO_LOCK_OFFSET     0x001c
#define GD32VW55X_GPIO_AFSEL0_OFFSET   0x0020
#define GD32VW55X_GPIO_AFSEL1_OFFSET   0x0024
#define GD32VW55X_GPIO_BC_OFFSET       0x0028
#define GD32VW55X_GPIO_TG_OFFSET       0x002c

#define GD32VW55X_GPIOA                GD32VW55X_GPIOA_BASE
#define GD32VW55X_GPIOB                GD32VW55X_GPIOB_BASE
#define GD32VW55X_GPIOC                GD32VW55X_GPIOC_BASE

#define GPIO_MODE_INPUT                0
#define GPIO_MODE_OUTPUT               1
#define GPIO_MODE_AF                   2
#define GPIO_MODE_ANALOG               3

#define GPIO_CTL_INPUT                 GPIO_MODE_INPUT
#define GPIO_CTL_OUTPUT                GPIO_MODE_OUTPUT
#define GPIO_CTL_AF                    GPIO_MODE_AF
#define GPIO_CTL_ANALOG                GPIO_MODE_ANALOG

#define GPIO_OSPD_2MHZ                 0
#define GPIO_OSPD_10MHZ                1
#define GPIO_OSPD_25MHZ                2
#define GPIO_OSPD_MAX                  3

#define GPIO_PUD_NONE                  0
#define GPIO_PUD_PULLUP                1
#define GPIO_PUD_PULLDOWN              2

#define GPIO_PIN(n)                    (1 << (n))

#endif
