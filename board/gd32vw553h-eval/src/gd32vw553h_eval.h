/****************************************************************************
 * boards/risc-v/gd32vw55x/gd32vw553h-eval/src/gd32vw553h_eval.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#ifndef __BOARDS_RISCV_GD32VW55X_GD32VW553H_EVAL_SRC_GD32VW553H_EVAL_H
#define __BOARDS_RISCV_GD32VW55X_GD32VW553H_EVAL_SRC_GD32VW553H_EVAL_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/compiler.h>
#include <stdint.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_bringup
 *
 * Description:
 *   Perform architecture-specific initialization.
 *
 ****************************************************************************/

int gd32_bringup(void);

/****************************************************************************
 * Name: gd32_spidev_initialize
 *
 * Description:
 *   Configure the SPI chip select GPIO(s) used by the board.
 *
 ****************************************************************************/

#ifdef CONFIG_GD32VW55X_SPI
void gd32_spidev_initialize(void);
#endif

#endif /* __BOARDS_RISCV_GD32VW55X_GD32VW553H_EVAL_SRC_GD32VW553H_EVAL_H */
