/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_irq.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#ifndef __CHIPS_GD32VW55X_GD32VW55X_IRQ_H
#define __CHIPS_GD32VW55X_GD32VW55X_IRQ_H

/****************************************************************************
 * GD32VW55x ECLIC interrupt numbers (offset from RISCV_IRQ_ASYNC).
 * 116 interrupt inputs total.
 ****************************************************************************/

#define GD32VW55X_NIRQS              116

#define GD32VW55X_IRQ_BORLPC         (RISCV_IRQ_ASYNC + 0)
#define GD32VW55X_IRQ_FWDGT           (RISCV_IRQ_ASYNC + 1)
#define GD32VW55X_IRQ_WWDGT           (RISCV_IRQ_ASYNC + 2)
#define GD32VW55X_IRQ_LVD             (RISCV_IRQ_ASYNC + 3)
#define GD32VW55X_IRQ_PMU             (RISCV_IRQ_ASYNC + 4)
#define GD32VW55X_IRQ_TAMPER          (RISCV_IRQ_ASYNC + 5)
#define GD32VW55X_IRQ_RTC             (RISCV_IRQ_ASYNC + 6)
#define GD32VW55X_IRQ_RTC_ALARM       (RISCV_IRQ_ASYNC + 7)
#define GD32VW55X_IRQ_DMA_CH0         (RISCV_IRQ_ASYNC + 8)
#define GD32VW55X_IRQ_DMA_CH1         (RISCV_IRQ_ASYNC + 9)
#define GD32VW55X_IRQ_DMA_CH2         (RISCV_IRQ_ASYNC + 10)
#define GD32VW55X_IRQ_DMA_CH3         (RISCV_IRQ_ASYNC + 11)
#define GD32VW55X_IRQ_DMA_CH4         (RISCV_IRQ_ASYNC + 12)
#define GD32VW55X_IRQ_DMA_CH5         (RISCV_IRQ_ASYNC + 13)
#define GD32VW55X_IRQ_DMA_CH6         (RISCV_IRQ_ASYNC + 14)
#define GD32VW55X_IRQ_DMA_CH7         (RISCV_IRQ_ASYNC + 15)
#define GD32VW55X_IRQ_ADC             (RISCV_IRQ_ASYNC + 16)
#define GD32VW55X_IRQ_EXTI0           (RISCV_IRQ_ASYNC + 17)
#define GD32VW55X_IRQ_EXTI1           (RISCV_IRQ_ASYNC + 18)
#define GD32VW55X_IRQ_EXTI2           (RISCV_IRQ_ASYNC + 19)
#define GD32VW55X_IRQ_EXTI3           (RISCV_IRQ_ASYNC + 20)
#define GD32VW55X_IRQ_EXTI4           (RISCV_IRQ_ASYNC + 21)
#define GD32VW55X_IRQ_EXTI5_9         (RISCV_IRQ_ASYNC + 22)
#define GD32VW55X_IRQ_TIMER0_BRK      (RISCV_IRQ_ASYNC + 23)
#define GD32VW55X_IRQ_TIMER0_UP       (RISCV_IRQ_ASYNC + 24)
#define GD32VW55X_IRQ_TIMER0_TRG_CMT  (RISCV_IRQ_ASYNC + 25)
#define GD32VW55X_IRQ_TIMER0_CH       (RISCV_IRQ_ASYNC + 26)
#define GD32VW55X_IRQ_TIMER1          (RISCV_IRQ_ASYNC + 27)
#define GD32VW55X_IRQ_TIMER2          (RISCV_IRQ_ASYNC + 28)
#define GD32VW55X_IRQ_I2C0_EV         (RISCV_IRQ_ASYNC + 29)
#define GD32VW55X_IRQ_I2C0_ER         (RISCV_IRQ_ASYNC + 30)
#define GD32VW55X_IRQ_I2C1_EV         (RISCV_IRQ_ASYNC + 31)
#define GD32VW55X_IRQ_I2C1_ER         (RISCV_IRQ_ASYNC + 32)
#define GD32VW55X_IRQ_SPI             (RISCV_IRQ_ASYNC + 33)
#define GD32VW55X_IRQ_USART0          (RISCV_IRQ_ASYNC + 34)
#define GD32VW55X_IRQ_UART1           (RISCV_IRQ_ASYNC + 35)
#define GD32VW55X_IRQ_UART2           (RISCV_IRQ_ASYNC + 36)
#define GD32VW55X_IRQ_EXTI10_15       (RISCV_IRQ_ASYNC + 37)
#define GD32VW55X_IRQ_TIMER15         (RISCV_IRQ_ASYNC + 39)
#define GD32VW55X_IRQ_TIMER16         (RISCV_IRQ_ASYNC + 40)
#define GD32VW55X_IRQ_TIMER5          (RISCV_IRQ_ASYNC + 46)
#define GD32VW55X_IRQ_TRNG            (RISCV_IRQ_ASYNC + 55)
#define GD32VW55X_IRQ_BLE             (RISCV_IRQ_ASYNC + 73)
#define GD32VW55X_IRQ_WIFI            (RISCV_IRQ_ASYNC + 75)

#endif
