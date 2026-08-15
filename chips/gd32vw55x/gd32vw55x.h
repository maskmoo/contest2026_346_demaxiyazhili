/****************************************************************************
 * chips/gd32vw55x/gd32vw55x.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#ifndef __CHIPS_GD32VW55X_GD32VW55X_H
#define __CHIPS_GD32VW55X_GD32VW55X_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifndef __ASSEMBLY__

void gd32vw55x_boardinitialize(void);
void gd32vw55x_irq_set_trigger(int irq, uint8_t trig);

#endif /* __ASSEMBLY__ */
#endif /* __CHIPS_GD32VW55X_GD32VW55X_H */
