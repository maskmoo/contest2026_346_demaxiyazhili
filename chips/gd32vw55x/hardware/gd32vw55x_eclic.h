/****************************************************************************
 * chips/gd32vw55x/hardware/gd32vw55x_eclic.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#ifndef __CHIPS_GD32VW55X_HARDWARE_GD32VW55X_ECLIC_H
#define __CHIPS_GD32VW55X_HARDWARE_GD32VW55X_ECLIC_H

#include "gd32vw55x_memorymap.h"

#define GD32VW55X_ECLIC_CFG          (GD32VW55X_ECLIC_BASE + 0x0000)
#define GD32VW55X_ECLIC_INFO         (GD32VW55X_ECLIC_BASE + 0x0004)
#define GD32VW55X_ECLIC_MTH          (GD32VW55X_ECLIC_BASE + 0x000b)

#define GD32VW55X_ECLIC_INTIP(n)     (GD32VW55X_ECLIC_BASE + 0x1000 + 4 * (n))
#define GD32VW55X_ECLIC_INTIE(n)     (GD32VW55X_ECLIC_BASE + 0x1001 + 4 * (n))
#define GD32VW55X_ECLIC_INTATTR(n)   (GD32VW55X_ECLIC_BASE + 0x1002 + 4 * (n))
#define GD32VW55X_ECLIC_INTCTL(n)    (GD32VW55X_ECLIC_BASE + 0x1003 + 4 * (n))

#define ECLIC_CFG_NLBITS_SHIFT       1
#define ECLIC_CFG_NLBITS_MASK        (0xf << ECLIC_CFG_NLBITS_SHIFT)

#define ECLIC_INTIP_IP               (1 << 0)
#define ECLIC_INTIE_IE               (1 << 0)

#define ECLIC_INTATTR_SHV            (1 << 0)
#define ECLIC_INTATTR_TRIG_SHIFT     1
#define ECLIC_INTATTR_TRIG_MASK      (3 << ECLIC_INTATTR_TRIG_SHIFT)
#define ECLIC_INTATTR_TRIG_LEVEL     (0 << ECLIC_INTATTR_TRIG_SHIFT)
#define ECLIC_INTATTR_TRIG_RISING    (1 << ECLIC_INTATTR_TRIG_SHIFT)
#define ECLIC_INTATTR_TRIG_FALLING   (3 << ECLIC_INTATTR_TRIG_SHIFT)

#define ECLIC_INTCTL_LEVEL(l)        ((((l) & 0xf) << 4) | 0x0f)

#define CSR_MTVT                     0x307
#define CSR_NUCLEI_MINTSTATUS        0x346
#define CSR_MCACHE_CTL               0x7ca
#define CSR_MMISC_CTL                0x7d0
#define CSR_MTVT2                    0x7ec

#define MCACHE_CTL_IC_EN             (1 << 0)
#define MMISC_CTL_NMI_CAUSE_FFF      (1 << 9)
#define ECLIC_MTVEC_MODE             0x03

#endif
