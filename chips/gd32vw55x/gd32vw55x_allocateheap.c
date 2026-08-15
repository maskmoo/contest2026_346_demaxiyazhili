/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_allocateheap.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/arch.h>
#include "gd32vw55x_memorymap.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void up_allocate_heap(void **heap_start, size_t *heap_size)
{
  *heap_start = (void *)&_ebss;
  *heap_size  = GD32VW55X_IDLESTACK_BASE - (uintptr_t)&_ebss;
}
