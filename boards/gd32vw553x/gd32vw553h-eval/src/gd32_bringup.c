/****************************************************************************
 * boards/risc-v/gd32vw55x/gd32vw553h-eval/src/gd32_bringup.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <sys/types.h>
#include <debug.h>
#include <nuttx/fs/fs.h>

#ifdef CONFIG_USERLED_LOWER
#  include <nuttx/leds/userled.h>
#endif

#ifdef CONFIG_GD32VW55X_SPI
#  include <nuttx/spi/spi_transfer.h>
#endif

#include <arch/board/board.h>
#include "gd32vw553h_eval.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_bringup
 ****************************************************************************/

int gd32_bringup(void)
{
  int ret = OK;

#ifdef CONFIG_USERLED_LOWER
  ret = userled_lower_initialize("/dev/userleds");
  if (ret < 0)
    {
      ferr("ERROR: failed to register /dev/userleds: %d\n", ret);
    }
#endif

#ifdef CONFIG_FS_PROCFS
  ret = nx_mount(NULL, "/proc", "procfs", 0, NULL);
  if (ret < 0)
    {
      ferr("ERROR: Failed to mount procfs at /proc: %d\n", ret);
    }
#endif

  return ret;
}
