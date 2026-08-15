/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_timerisr.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/arch.h>
#include "riscv_internal.h"
#include "gd32vw55x_clockconfig.h"
#include "include/chip.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define SYSTIMER_MTIME     (GD32VW55X_SYSTIMER_BASE + 0x0000)
#define SYSTIMER_MTIMEH    (GD32VW55X_SYSTIMER_BASE + 0x0004)
#define SYSTIMER_MTIMECMP  (GD32VW55X_SYSTIMER_BASE + 0x0008)
#define SYSTIMER_MTIMECMPH (GD32VW55X_SYSTIMER_BASE + 0x000c)

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void up_timer_initialize(void)
{
  uint64_t mtimecmp;

  putreg32(0, SYSTIMER_MTIMECMP);
  putreg32(0, SYSTIMER_MTIMECMPH);

  mtimecmp = (uint64_t)GD32VW55X_MTIME_FREQ / TICK_PER_SEC;
  putreg32((uint32_t)mtimecmp, SYSTIMER_MTIMECMP);
  putreg32((uint32_t)(mtimecmp >> 32), SYSTIMER_MTIMECMPH);

  up_enable_irq(RISCV_IRQ_MTIMER);
}

uint64_t up_timer_gettime(void)
{
  uint32_t lo;
  uint32_t hi;

  do
    {
      hi = getreg32(SYSTIMER_MTIMEH);
      lo = getreg32(SYSTIMER_MTIME);
    }
  while (hi != getreg32(SYSTIMER_MTIMEH));

  return ((uint64_t)hi << 32) | lo;
}
