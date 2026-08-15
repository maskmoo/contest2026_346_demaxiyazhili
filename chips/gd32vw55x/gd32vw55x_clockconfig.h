/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_clockconfig.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#ifndef __CHIPS_GD32VW55X_GD32VW55X_CLOCKCONFIG_H
#define __CHIPS_GD32VW55X_GD32VW55X_CLOCKCONFIG_H

#define GD32VW55X_HXTAL_FREQ     40000000
#define GD32VW55X_SYSCLK_FREQ    160000000
#define GD32VW55X_AHB_FREQ       GD32VW55X_SYSCLK_FREQ
#define GD32VW55X_PCLK2_FREQ     GD32VW55X_SYSCLK_FREQ
#define GD32VW55X_PCLK1_FREQ     (GD32VW55X_SYSCLK_FREQ / 2)
#define GD32VW55X_MTIME_FREQ     GD32VW55X_SYSCLK_FREQ

#ifndef __ASSEMBLY__
void gd32vw55x_clockconfig(void);
#endif

#endif
