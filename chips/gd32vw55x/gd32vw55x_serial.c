/****************************************************************************
 * chips/gd32vw55x/gd32vw55x_serial.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <debug.h>
#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/serial/serial.h>
#include <arch/board/board.h>
#include "riscv_internal.h"
#include "gd32vw55x_config.h"
#include "gd32vw55x_clockconfig.h"
#include "gd32vw55x_lowputc.h"
#include "hardware/gd32vw55x_uart.h"
#include "include/chip.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#if defined(CONFIG_UART1_SERIAL_CONSOLE)
#  define CONSOLE_DEV     g_uart1port
#  define TTYS0_DEV       g_uart1port
#  define UART1_ASSIGNED  1
#elif defined(CONFIG_UART2_SERIAL_CONSOLE)
#  define CONSOLE_DEV     g_uart2port
#  define TTYS0_DEV       g_uart2port
#  define UART2_ASSIGNED  1
#elif defined(CONFIG_USART0_SERIAL_CONSOLE)
#  define CONSOLE_DEV     g_usart0port
#  define TTYS0_DEV       g_usart0port
#  define USART0_ASSIGNED 1
#elif defined(CONFIG_GD32VW55X_UART1)
#  define TTYS0_DEV       g_uart1port
#  define UART1_ASSIGNED  1
#elif defined(CONFIG_GD32VW55X_UART2)
#  define TTYS0_DEV       g_uart2port
#  define UART2_ASSIGNED  1
#elif defined(CONFIG_GD32VW55X_USART0)
#  define TTYS0_DEV       g_usart0port
#  define USART0_ASSIGNED 1
#endif

#if defined(CONFIG_GD32VW55X_USART0) && !defined(USART0_ASSIGNED)
#  define TTYS1_DEV       g_usart0port
#  define USART0_ASSIGNED 1
#elif defined(CONFIG_GD32VW55X_UART1) && !defined(UART1_ASSIGNED)
#  define TTYS1_DEV       g_uart1port
#  define UART1_ASSIGNED  1
#elif defined(CONFIG_GD32VW55X_UART2) && !defined(UART2_ASSIGNED)
#  define TTYS1_DEV       g_uart2port
#  define UART2_ASSIGNED  1
#endif

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct gd32vw55x_uart_s
{
  uint32_t uartbase;
  uint32_t uartclk;
  uint32_t baud;
  uint8_t  irq;
  uint32_t im;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int  gd32vw55x_setup(struct uart_dev_s *dev);
static void gd32vw55x_shutdown(struct uart_dev_s *dev);
static int  gd32vw55x_attach(struct uart_dev_s *dev);
static void gd32vw55x_detach(struct uart_dev_s *dev);
static int  gd32vw55x_interrupt(int irq, void *context, void *arg);
static int  gd32vw55x_ioctl(struct file *filep, int cmd, unsigned long arg);
static int  gd32vw55x_receive(struct uart_dev_s *dev, unsigned int *status);
static void gd32vw55x_rxint(struct uart_dev_s *dev, bool enable);
static bool gd32vw55x_rxavailable(struct uart_dev_s *dev);
static void gd32vw55x_send(struct uart_dev_s *dev, int ch);
static void gd32vw55x_txint(struct uart_dev_s *dev, bool enable);
static bool gd32vw55x_txready(struct uart_dev_s *dev);
static bool gd32vw55x_txempty(struct uart_dev_s *dev);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct uart_ops_s g_uart_ops =
{
  .setup       = gd32vw55x_setup,
  .shutdown    = gd32vw55x_shutdown,
  .attach      = gd32vw55x_attach,
  .detach      = gd32vw55x_detach,
  .ioctl       = gd32vw55x_ioctl,
  .receive     = gd32vw55x_receive,
  .rxint       = gd32vw55x_rxint,
  .rxavailable = gd32vw55x_rxavailable,
  .send        = gd32vw55x_send,
  .txint       = gd32vw55x_txint,
  .txready     = gd32vw55x_txready,
  .txempty     = gd32vw55x_txempty,
};

#ifdef CONFIG_GD32VW55X_USART0
static char g_usart0rxbuffer[CONFIG_USART0_RXBUFSIZE];
static char g_usart0txbuffer[CONFIG_USART0_TXBUFSIZE];
static struct gd32vw55x_uart_s g_usart0priv =
{
  .uartbase = GD32VW55X_USART0_BASE,
  .uartclk  = GD32VW55X_PCLK1_FREQ,
  .baud     = CONFIG_USART0_BAUD,
  .irq      = GD32VW55X_IRQ_USART0,
};
static uart_dev_t g_usart0port =
{
  .recv = { .size = CONFIG_USART0_RXBUFSIZE, .buffer = g_usart0rxbuffer, },
  .xmit = { .size = CONFIG_USART0_TXBUFSIZE, .buffer = g_usart0txbuffer, },
  .ops  = &g_uart_ops,
  .priv = &g_usart0priv,
};
#endif

#ifdef CONFIG_GD32VW55X_UART1
static char g_uart1rxbuffer[CONFIG_UART1_RXBUFSIZE];
static char g_uart1txbuffer[CONFIG_UART1_TXBUFSIZE];
static struct gd32vw55x_uart_s g_uart1priv =
{
  .uartbase = GD32VW55X_UART1_BASE,
  .uartclk  = GD32VW55X_PCLK1_FREQ,
  .baud     = CONFIG_UART1_BAUD,
  .irq      = GD32VW55X_IRQ_UART1,
};
static uart_dev_t g_uart1port =
{
  .recv = { .size = CONFIG_UART1_RXBUFSIZE, .buffer = g_uart1rxbuffer, },
  .xmit = { .size = CONFIG_UART1_TXBUFSIZE, .buffer = g_uart1txbuffer, },
  .ops  = &g_uart_ops,
  .priv = &g_uart1priv,
};
#endif

#ifdef CONFIG_GD32VW55X_UART2
static char g_uart2rxbuffer[CONFIG_UART2_RXBUFSIZE];
static char g_uart2txbuffer[CONFIG_UART2_TXBUFSIZE];
static struct gd32vw55x_uart_s g_uart2priv =
{
  .uartbase = GD32VW55X_UART2_BASE,
  .uartclk  = GD32VW55X_PCLK2_FREQ,
  .baud     = CONFIG_UART2_BAUD,
  .irq      = GD32VW55X_IRQ_UART2,
};
static uart_dev_t g_uart2port =
{
  .recv = { .size = CONFIG_UART2_RXBUFSIZE, .buffer = g_uart2rxbuffer, },
  .xmit = { .size = CONFIG_UART2_TXBUFSIZE, .buffer = g_uart2txbuffer, },
  .ops  = &g_uart_ops,
  .priv = &g_uart2priv,
};
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void gd32vw55x_restoreint(struct gd32vw55x_uart_s *priv)
{
  modifyreg32(priv->uartbase + GD32VW55X_UART_CTL0_OFFSET,
              UART_CTL0_RBNEIE | UART_CTL0_TBEIE, priv->im);
}

static int gd32vw55x_setup(struct uart_dev_s *dev)
{
  struct gd32vw55x_uart_s *priv = (struct gd32vw55x_uart_s *)dev->priv;
  gd32vw55x_uart_configure(priv->uartbase, priv->uartclk, priv->baud);
  priv->im = 0;
  return OK;
}

static void gd32vw55x_shutdown(struct uart_dev_s *dev)
{
  struct gd32vw55x_uart_s *priv = (struct gd32vw55x_uart_s *)dev->priv;
  priv->im = 0;
  gd32vw55x_restoreint(priv);
}

static int gd32vw55x_attach(struct uart_dev_s *dev)
{
  struct gd32vw55x_uart_s *priv = (struct gd32vw55x_uart_s *)dev->priv;
  int ret;
  ret = irq_attach(priv->irq, gd32vw55x_interrupt, dev);
  if (ret == OK)
    {
      up_enable_irq(priv->irq);
    }
  return ret;
}

static void gd32vw55x_detach(struct uart_dev_s *dev)
{
  struct gd32vw55x_uart_s *priv = (struct gd32vw55x_uart_s *)dev->priv;
  up_disable_irq(priv->irq);
  irq_detach(priv->irq);
}

static int gd32vw55x_interrupt(int irq, void *context, void *arg)
{
  struct uart_dev_s *dev = (struct uart_dev_s *)arg;
  struct gd32vw55x_uart_s *priv;
  uint32_t status;

  DEBUGASSERT(dev != NULL && dev->priv != NULL);
  priv = (struct gd32vw55x_uart_s *)dev->priv;

  status = getreg32(priv->uartbase + GD32VW55X_UART_STAT_OFFSET);

  if ((status & (UART_STAT_ORERR | UART_STAT_FERR |
                 UART_STAT_NERR | UART_STAT_PERR)) != 0)
    {
      putreg32(UART_INTC_OREC | UART_INTC_FEC |
               UART_INTC_NEC | UART_INTC_PEC,
               priv->uartbase + GD32VW55X_UART_INTC_OFFSET);
    }

  if ((status & UART_STAT_RBNE) != 0 &&
      (priv->im & UART_CTL0_RBNEIE) != 0)
    {
      uart_recvchars(dev);
    }

  if ((status & UART_STAT_TBE) != 0 &&
      (priv->im & UART_CTL0_TBEIE) != 0)
    {
      uart_xmitchars(dev);
    }

  return OK;
}

static int gd32vw55x_ioctl(struct file *filep, int cmd, unsigned long arg)
{
  return -ENOTTY;
}

static int gd32vw55x_receive(struct uart_dev_s *dev, unsigned int *status)
{
  struct gd32vw55x_uart_s *priv = (struct gd32vw55x_uart_s *)dev->priv;
  *status = getreg32(priv->uartbase + GD32VW55X_UART_STAT_OFFSET);
  return getreg32(priv->uartbase + GD32VW55X_UART_RDATA_OFFSET) & 0xff;
}

static void gd32vw55x_rxint(struct uart_dev_s *dev, bool enable)
{
  struct gd32vw55x_uart_s *priv = (struct gd32vw55x_uart_s *)dev->priv;
  irqstate_t flags;
  flags = enter_critical_section();
  if (enable)
    {
#ifndef CONFIG_SUPPRESS_SERIAL_INTS
      priv->im |= UART_CTL0_RBNEIE;
#endif
    }
  else
    {
      priv->im &= ~UART_CTL0_RBNEIE;
    }
  gd32vw55x_restoreint(priv);
  leave_critical_section(flags);
}

static bool gd32vw55x_rxavailable(struct uart_dev_s *dev)
{
  struct gd32vw55x_uart_s *priv = (struct gd32vw55x_uart_s *)dev->priv;
  return (getreg32(priv->uartbase + GD32VW55X_UART_STAT_OFFSET) &
          UART_STAT_RBNE) != 0;
}

static void gd32vw55x_send(struct uart_dev_s *dev, int ch)
{
  struct gd32vw55x_uart_s *priv = (struct gd32vw55x_uart_s *)dev->priv;
  putreg32((uint32_t)ch, priv->uartbase + GD32VW55X_UART_TDATA_OFFSET);
}

static void gd32vw55x_txint(struct uart_dev_s *dev, bool enable)
{
  struct gd32vw55x_uart_s *priv = (struct gd32vw55x_uart_s *)dev->priv;
  irqstate_t flags;
  flags = enter_critical_section();
  if (enable)
    {
#ifndef CONFIG_SUPPRESS_SERIAL_INTS
      priv->im |= UART_CTL0_TBEIE;
      gd32vw55x_restoreint(priv);
      uart_xmitchars(dev);
#endif
    }
  else
    {
      priv->im &= ~UART_CTL0_TBEIE;
      gd32vw55x_restoreint(priv);
    }
  leave_critical_section(flags);
}

static bool gd32vw55x_txready(struct uart_dev_s *dev)
{
  struct gd32vw55x_uart_s *priv = (struct gd32vw55x_uart_s *)dev->priv;
  return (getreg32(priv->uartbase + GD32VW55X_UART_STAT_OFFSET) &
          UART_STAT_TBE) != 0;
}

static bool gd32vw55x_txempty(struct uart_dev_s *dev)
{
  struct gd32vw55x_uart_s *priv = (struct gd32vw55x_uart_s *)dev->priv;
  return (getreg32(priv->uartbase + GD32VW55X_UART_STAT_OFFSET) &
          UART_STAT_TC) != 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifdef USE_EARLYSERIALINIT
void riscv_earlyserialinit(void)
{
#ifdef HAVE_SERIAL_CONSOLE
  CONSOLE_DEV.isconsole = true;
  gd32vw55x_setup(&CONSOLE_DEV);
#endif
}
#endif

void riscv_serialinit(void)
{
#ifdef HAVE_SERIAL_CONSOLE
  uart_register("/dev/console", &CONSOLE_DEV);
#endif
#ifdef TTYS0_DEV
  uart_register("/dev/ttyS0", &TTYS0_DEV);
#endif
#ifdef TTYS1_DEV
  uart_register("/dev/ttyS1", &TTYS1_DEV);
#endif
#ifdef TTYS2_DEV
  uart_register("/dev/ttyS2", &TTYS2_DEV);
#endif
}

void up_putc(int ch)
{
#ifdef HAVE_SERIAL_CONSOLE
  riscv_lowputc(ch);
#endif
}
