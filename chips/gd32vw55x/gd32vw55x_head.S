/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_head.S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "include/chip.h"
#include "gd32vw55x_memorymap.h"
#include "hardware/gd32vw55x_eclic.h"
#include "riscv_internal.h"

/****************************************************************************
 * Public Symbols
 ****************************************************************************/

  .extern __trap_vec

  .section .text
  .global __start
  .type   __start, function

__start:

  csrw CSR_MIE, zero
  csrci CSR_MSTATUS, MSTATUS_MIE

  la   sp, GD32VW55X_IDLESTACK_TOP

  csrsi CSR_MCACHE_CTL, MCACHE_CTL_IC_EN

  li   t0, MMISC_CTL_NMI_CAUSE_FFF
  csrs CSR_MMISC_CTL, t0

  la   t0, __trap_vec
  csrw CSR_MTVT, t0

  la   t0, __trap_vec
  csrw CSR_MTVT2, t0
  csrsi CSR_MTVT2, 0x1

  la   t0, __trap_vec
  ori  t0, t0, ECLIC_MTVEC_MODE
  csrw CSR_MTVEC, t0

#ifdef CONFIG_ARCH_FPU
  li   t0, MSTATUS_FS_INIT
  csrs CSR_MSTATUS, t0
  fscsr zero
#endif

  jal  x1, __gd32vw55x_start

1:
  wfi
  j    1b

  .global _init
  .global _fini

_init:
_fini:
  ret
