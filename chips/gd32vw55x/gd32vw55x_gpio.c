/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_gpio.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>
#include "gd32vw55x_gpio.h"
#include "hardware/gd32vw55x_gpio.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int gd32_gpio_config(uint32_t pinset)
{
  uint32_t port_base = pinset & 0xffff0000;
  int pin            = (pinset >> 8) & 0xff;
  int mode           = pinset & 0x1;
  uint32_t regaddr;
  uint32_t regval;
  int shift;

  shift = pin << 1;

  regaddr = port_base + GD32VW55X_GPIO_CTL_OFFSET;
  regval  = getreg32(regaddr);
  regval &= ~(3 << shift);
  regval |= mode << shift;
  putreg32(regval, regaddr);

  if (mode == GPIO_MODE_OUTPUT)
    {
      regaddr = port_base + GD32VW55X_GPIO_OSPD_OFFSET;
      regval  = getreg32(regaddr);
      regval &= ~(3 << shift);
      regval |= GPIO_OSPD_25MHZ << shift;
      putreg32(regval, regaddr);
    }

  return 0;
}

void gd32_gpio_write(uint32_t pinset, bool value)
{
  uint32_t port_base = pinset & 0xffff0000;
  int pin            = (pinset >> 8) & 0xff;

  if (value)
    {
      putreg32(1 << pin, port_base + GD32VW55X_GPIO_BOP_OFFSET);
    }
  else
    {
      putreg32(1 << (pin + 16), port_base + GD32VW55X_GPIO_BOP_OFFSET);
    }
}

bool gd32_gpio_read(uint32_t pinset)
{
  uint32_t port_base = pinset & 0xffff0000;
  int pin            = (pinset >> 8) & 0xff;

  return (getreg32(port_base + GD32VW55X_GPIO_ISTAT_OFFSET) &
          (1 << pin)) != 0;
}
