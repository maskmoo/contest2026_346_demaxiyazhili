/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_clockconfig.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>
#include "riscv_internal.h"
#include "gd32vw55x_clockconfig.h"
#include "hardware/gd32vw55x_rcu.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define GD32VW55X_SYSTIMER_MTIMECTL  (GD32VW55X_SYSTIMER_BASE + 0x0ff8)
#define SYSTIMER_MTIMECTL_CLKSRC     (1 << 2)

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void gd32vw55x_clockconfig(void)
{
  uint32_t regval;

  modifyreg32(GD32VW55X_RCU_CTL, 0, RCU_CTL_IRC16MEN);
  modifyreg32(GD32VW55X_RCU_CFG0, RCU_CFG0_SCS_MASK, 0);
  putreg32(0, GD32VW55X_RCU_CFG0);
  modifyreg32(GD32VW55X_RCU_CFG1,
              RCU_CFG1_RFPLLCALEN | RCU_CFG1_RFPLLPU, 0);
  modifyreg32(GD32VW55X_RCU_CTL,
              RCU_CTL_PLLDIGEN | RCU_CTL_PLLDIGPU |
              RCU_CTL_RFCKMEN | RCU_CTL_HXTALEN, 0);
  putreg32(0, GD32VW55X_RCU_PLL);
  putreg32(0, GD32VW55X_RCU_PLLDIGCFG0);
  putreg32(0x07800000, GD32VW55X_RCU_PLLDIGCFG1);
  putreg32(0, GD32VW55X_RCU_INT);

  modifyreg32(GD32VW55X_RCU_CTL, 0, RCU_CTL_HXTALPU);
  modifyreg32(GD32VW55X_RCU_CTL, 0, RCU_CTL_HXTALEN);
  modifyreg32(GD32VW55X_RCU_CTL, 0, RCU_CTL_HXTALREADY);

  modifyreg32(GD32VW55X_RCU_CFG0, 0,
              RCU_CFG0_AHBPSC_DIV1 |
              RCU_CFG0_APB2PSC_DIV1 |
              RCU_CFG0_APB1PSC_DIV2);

  modifyreg32(GD32VW55X_RCU_PLL, 0, RCU_PLL_PLLDIGSEL);

  regval = ((960 << 21) / (GD32VW55X_HXTAL_FREQ / 1000000)) & 0x7fffffff;
  putreg32(regval, GD32VW55X_RCU_PLLDIGCFG1);

  modifyreg32(GD32VW55X_RCU_PLLDIGCFG0, 0, RCU_PLLDIGCFG0_OSEL_480M);
  modifyreg32(GD32VW55X_RCU_PLLDIGCFG0, 0, RCU_PLLDIGCFG0_SYSDIV(2));

  modifyreg32(GD32VW55X_RCU_CFG1, 0, RCU_CFG1_RFPLLCALEN | RCU_CFG1_BGPU);
  modifyreg32(GD32VW55X_RCU_CTL, 0, RCU_CTL_PLLDIGEN | RCU_CTL_PLLDIGPU);

  while ((getreg32(GD32VW55X_RCU_CTL) & RCU_CTL_PLLDIGSTB) == 0)
    {
    }

  modifyreg32(GD32VW55X_RCU_CFG0, RCU_CFG0_SCS_MASK, RCU_CFG0_SCS_PLLDIG);

  while ((getreg32(GD32VW55X_RCU_CFG0) & RCU_CFG0_SCSS_MASK) !=
         RCU_CFG0_SCSS_PLLDIG)
    {
    }

  modifyreg32(GD32VW55X_SYSTIMER_MTIMECTL, 0, SYSTIMER_MTIMECTL_CLKSRC);
}
