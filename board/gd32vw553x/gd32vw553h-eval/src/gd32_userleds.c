/****************************************************************************
 * boards/risc-v/gd32vw55x/gd32vw553h-eval/src/gd32_userleds.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/board.h>
#include <arch/board/board.h>

#ifdef CONFIG_USERLED_LOWER
#  include <nuttx/leds/userled.h>
#endif

/****************************************************************************
 * Private Data
 ****************************************************************************/

#ifdef CONFIG_USERLED_LOWER
static const struct userled_s g_ledlist[] =
{
  {
    .ledno  = BOARD_LED1,
    .gpiono = GPIO_LED1,
  },
  {
    .ledno  = BOARD_LED2,
    .gpiono = GPIO_LED2,
  },
  {
    .ledno  = BOARD_LED3,
    .gpiono = GPIO_LED3,
  },
};
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_userled_initialize
 ****************************************************************************/

#ifdef CONFIG_USERLED_LOWER
void board_userled_initialize(void)
{
  /* Configure LED GPIOs as outputs */

  for (int i = 0; i < BOARD_NLEDS; i++)
    {
      gd32_gpio_config(g_ledlist[i].gpiono);
    }
}
#endif

/****************************************************************************
 * Name: board_userled
 ****************************************************************************/

#ifdef CONFIG_USERLED_LOWER
void board_userled(int led, bool ledon)
{
  if (led < BOARD_NLEDS)
    {
      gd32_gpio_write(g_ledlist[led].gpiono, ledon);
    }
}
#endif

/****************************************************************************
 * Name: board_userled_all
 ****************************************************************************/

#ifdef CONFIG_USERLED_LOWER
void board_userled_all(uint8_t ledset)
{
  for (int i = 0; i < BOARD_NLEDS; i++)
    {
      gd32_gpio_write(g_ledlist[i].gpiono, (ledset & (1 << i)) != 0);
    }
}
#endif
