/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_gpio.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#ifndef __CHIPS_GD32VW55X_GD32VW55X_GPIO_H
#define __CHIPS_GD32VW55X_GD32VW55X_GPIO_H

#include <nuttx/config.h>
#include <stdbool.h>
#include <stdint.h>

/****************************************************************************
 * Pin encoding: [31:0] = port_base | (pin << 8) | mode
 ****************************************************************************/

int  gd32_gpio_config(uint32_t pinset);
void gd32_gpio_write(uint32_t pinset, bool value);
bool gd32_gpio_read(uint32_t pinset);

#endif
