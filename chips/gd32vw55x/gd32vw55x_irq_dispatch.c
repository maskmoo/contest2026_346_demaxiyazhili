/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_irq_dispatch.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/irq.h>
#include "riscv_internal.h"
#include "hardware/gd32vw55x_eclic.h"
#include "chip.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void *riscv_dispatch_irq(uintptr_t vector, uintptr_t *regs)
{
  int irq = (vector & ~0xf) + (vector & 0xf);

  if (irq == RISCV_IRQ_MTIMER)
    {
      uint64_t mtimecmp;
      uint32_t lo;
      uint32_t hi;

      lo = getreg32(GD32VW55X_SYSTIMER_BASE + 0x0008);
      hi = getreg32(GD32VW55X_SYSTIMER_BASE + 0x000c);
      mtimecmp = ((uint64_t)hi << 32) | lo;
      mtimecmp += (uint64_t)GD32VW55X_MTIME_FREQ / TICK_PER_SEC;
      putreg32((uint32_t)mtimecmp, GD32VW55X_SYSTIMER_BASE + 0x0008);
      putreg32((uint32_t)(mtimecmp >> 32), GD32VW55X_SYSTIMER_BASE + 0x000c);

      regs = riscv_doirq(irq, regs);
    }
  else
    {
      regs = riscv_doirq(irq, regs);
    }

  return regs;
}
