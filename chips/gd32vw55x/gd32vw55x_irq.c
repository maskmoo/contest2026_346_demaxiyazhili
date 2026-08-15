/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_irq.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>
#include <assert.h>
#include <nuttx/arch.h>
#include <nuttx/irq.h>
#include "riscv_internal.h"
#include "hardware/gd32vw55x_eclic.h"
#include "chip.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define ECLIC_IRQ_LEVEL   1

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void up_irqinitialize(void)
{
  int i;

  up_irq_save();

  putreg8(4 << ECLIC_CFG_NLBITS_SHIFT, GD32VW55X_ECLIC_CFG);
  putreg8(0, GD32VW55X_ECLIC_MTH);

  for (i = 0; i < GD32VW55X_NIRQS; i++)
    {
      putreg8(0, GD32VW55X_ECLIC_INTIE(i));
      putreg8(0, GD32VW55X_ECLIC_INTIP(i));
      putreg8(ECLIC_INTATTR_TRIG_LEVEL, GD32VW55X_ECLIC_INTATTR(i));
      putreg8(ECLIC_INTCTL_LEVEL(ECLIC_IRQ_LEVEL),
              GD32VW55X_ECLIC_INTCTL(i));
    }

#if defined(CONFIG_STACK_COLORATION) && CONFIG_ARCH_INTERRUPTSTACK > 15
  size_t intstack_size = (CONFIG_ARCH_INTERRUPTSTACK & ~15);
  riscv_stack_color(g_intstackalloc, intstack_size);
#endif

  riscv_exception_attach();

#ifndef CONFIG_SUPPRESS_INTERRUPTS
  up_irq_enable();
#endif
}

void up_disable_irq(int irq)
{
  int extirq = irq - RISCV_IRQ_ASYNC;
  DEBUGASSERT(extirq >= 0 && extirq < GD32VW55X_NIRQS);
  putreg8(0, GD32VW55X_ECLIC_INTIE(extirq));
}

void up_enable_irq(int irq)
{
  int extirq = irq - RISCV_IRQ_ASYNC;
  DEBUGASSERT(extirq >= 0 && extirq < GD32VW55X_NIRQS);
  putreg8(ECLIC_INTIE_IE, GD32VW55X_ECLIC_INTIE(extirq));
}

void gd32vw55x_irq_set_trigger(int irq, uint8_t trig)
{
  int extirq = irq - RISCV_IRQ_ASYNC;
  uint8_t regval;

  DEBUGASSERT(extirq >= 0 && extirq < GD32VW55X_NIRQS);

  regval = getreg8(GD32VW55X_ECLIC_INTATTR(extirq));
  regval = (regval & ~ECLIC_INTATTR_TRIG_MASK) | trig;
  putreg8(regval, GD32VW55X_ECLIC_INTATTR(extirq));
}

irqstate_t up_irq_enable(void)
{
  return READ_AND_SET_CSR(CSR_MSTATUS, MSTATUS_MIE);
}

void riscv_ack_irq(int irq)
{
  int extirq = irq - RISCV_IRQ_ASYNC;
  uint8_t attr;

  if (extirq < 0 || extirq >= GD32VW55X_NIRQS)
    {
      return;
    }

  attr = getreg8(GD32VW55X_ECLIC_INTATTR(extirq));
  if ((attr & ECLIC_INTATTR_TRIG_MASK) != ECLIC_INTATTR_TRIG_LEVEL)
    {
      putreg8(0, GD32VW55X_ECLIC_INTIP(extirq));
    }
}
