/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_lowputc.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#ifndef __CHIPS_GD32VW55X_GD32VW55X_LOWPUTC_H
#define __CHIPS_GD32VW55X_GD32VW55X_LOWPUTC_H

#include <nuttx/config.h>
#include <stdint.h>

void gd32vw55x_lowsetup(void);
void gd32vw55x_uart_configure(uint32_t uart_base, uint32_t uartclk,
                              uint32_t baud);
void gd32vw55x_gpio_config_af(uint32_t port_base, int pin, int af,
                              int pull);

#endif
