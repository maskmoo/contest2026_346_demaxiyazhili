/****************************************************************************
 * boards/risc-v/gd32vw55x/gd32vw553h-eval/src/gd32_boot.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <debug.h>
#include <nuttx/board.h>
#include <arch/board/board.h>
#include "gd32vw55x.h"
#include "gd32vw553h_eval.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32vw55x_boardinitialize
 *
 * Description:
 *   All GD32VW55x architectures must provide the following entry point.
 *   This entry point is called early in the initialization -- after all
 *   memory has been configured and mapped but before any devices have
 *   been initialized.
 *
 ****************************************************************************/

void gd32vw55x_boardinitialize(void)
{
}

/****************************************************************************
 * Name: board_late_initialize
 *
 ****************************************************************************/

#ifdef CONFIG_BOARD_LATE_INITIALIZE
void board_late_initialize(void)
{
  gd32_bringup();
}
#endif
