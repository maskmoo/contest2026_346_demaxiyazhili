/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_start.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>
#include <nuttx/init.h>
#include <arch/board/board.h>
#include "riscv_internal.h"
#include "gd32vw55x.h"
#include "gd32vw55x_clockconfig.h"
#include "gd32vw55x_lowputc.h"
#include "chip.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifdef CONFIG_DEBUG_FEATURES
#  define showprogress(c) riscv_lowputc(c)
#else
#  define showprogress(c)
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void __gd32vw55x_start(void)
{
  const uint32_t *src;
  uint32_t *dest;

  for (dest = (uint32_t *)_sbss; dest < (uint32_t *)_ebss; )
    {
      *dest++ = 0;
    }

  for (src = (const uint32_t *)_eronly,
       dest = (uint32_t *)_sdata; dest < (uint32_t *)_edata;
      )
    {
      *dest++ = *src++;
    }

  gd32vw55x_clockconfig();
  gd32vw55x_lowsetup();

  showprogress('A');

#ifdef USE_EARLYSERIALINIT
  riscv_earlyserialinit();
#endif

  showprogress('B');
  gd32vw55x_boardinitialize();
  showprogress('C');

  nx_start();

  for (; ; );
}
