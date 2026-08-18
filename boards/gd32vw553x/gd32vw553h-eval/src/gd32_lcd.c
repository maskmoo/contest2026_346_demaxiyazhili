/****************************************************************************
 * boards/risc-v/gd32vw55x/gd32vw553h-eval/src/gd32_lcd.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <inttypes.h>
#include <stdbool.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/arch.h>
#include <nuttx/board.h>
#include <nuttx/spi/spi.h>
#include <nuttx/lcd/ili9341.h>
#include <nuttx/video/fb.h>

#include <arch/board/board.h>

#include "gd32vw55x_gpio.h"
#include "gd32vw55x_spi.h"
#include "gd32vw553h-eval.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* SPI frequency: 20MHz (PCLK2=80MHz, PSC_4 = 20MHz) */

#ifndef CONFIG_GD32VW553H_EVAL_ILI9341_SPIFREQUENCY
#  define CONFIG_GD32VW553H_EVAL_ILI9341_SPIFREQUENCY   20000000
#endif

/* LCD device number */

#ifdef CONFIG_GD32VW553H_EVAL_ILI9341_LCDDEVICE
#  define ILI9341_LCD_DEVICE  CONFIG_GD32VW553H_EVAL_ILI9341_LCDDEVICE
#else
#  define ILI9341_LCD_DEVICE  0
#endif

/* Display orientation */

#ifdef CONFIG_LCD_LANDSCAPE
#  define GD32_ILI9341_MADCTL_PARAM  (ILI9341_MEMORY_ACCESS_CONTROL_MV | \
                                      ILI9341_MEMORY_ACCESS_CONTROL_BGR)
#  define ILI9341_XRES               320
#  define ILI9341_YRES               240
#elif defined(CONFIG_LCD_PORTRAIT)
#  define GD32_ILI9341_MADCTL_PARAM  ILI9341_MEMORY_ACCESS_CONTROL_BGR
#  define ILI9341_XRES               240
#  define ILI9341_YRES               320
#elif defined(CONFIG_LCD_RLANDSCAPE)
#  define GD32_ILI9341_MADCTL_PARAM  (ILI9341_MEMORY_ACCESS_CONTROL_MV | \
                                      ILI9341_MEMORY_ACCESS_CONTROL_MX | \
                                      ILI9341_MEMORY_ACCESS_CONTROL_BGR)
#  define ILI9341_XRES               320
#  define ILI9341_YRES               240
#elif defined(CONFIG_LCD_RPORTRAIT)
#  define GD32_ILI9341_MADCTL_PARAM  (ILI9341_MEMORY_ACCESS_CONTROL_MY | \
                                      ILI9341_MEMORY_ACCESS_CONTROL_BGR)
#  define ILI9341_XRES               240
#  define ILI9341_YRES               320
#else
#  define GD32_ILI9341_MADCTL_PARAM  (ILI9341_MEMORY_ACCESS_CONTROL_MV | \
                                      ILI9341_MEMORY_ACCESS_CONTROL_BGR)
#  define ILI9341_XRES               320
#  define ILI9341_YRES               240
#endif

/****************************************************************************
 * Private Type Definition
 ****************************************************************************/

struct gd32_ili9341_lcd_s
{
  /* Publicly visible device structure */

  struct ili9341_lcd_s dev;

  /* Reference to spi device structure */

  struct spi_dev_s *spi;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct gd32_ili9341_lcd_s g_lcddev;
static struct lcd_dev_s *g_lcd = NULL;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_ili93414ws_select
 *
 * Description:
 *   Select the SPI device and configure for LCD communication.
 *
 ****************************************************************************/

static void gd32_ili93414ws_select(struct ili9341_lcd_s *lcd)
{
  struct gd32_ili9341_lcd_s *priv = (struct gd32_ili9341_lcd_s *)lcd;

  /* Lock the SPI bus */

  SPI_LOCK(priv->spi, true);

  /* Select the LCD */

  SPI_SELECT(priv->spi, SPIDEV_DISPLAY(0), true);

  /* Set frequency and mode */

  SPI_SETFREQUENCY(priv->spi, CONFIG_GD32VW553H_EVAL_ILI9341_SPIFREQUENCY);
  SPI_SETMODE(priv->spi, SPIDEV_MODE0);
}

/****************************************************************************
 * Name: gd32_ili93414ws_deselect
 *
 * Description:
 *   Deselect the SPI device.
 *
 ****************************************************************************/

static void gd32_ili93414ws_deselect(struct ili9341_lcd_s *lcd)
{
  struct gd32_ili9341_lcd_s *priv = (struct gd32_ili9341_lcd_s *)lcd;

  /* De-select the LCD */

  SPI_SELECT(priv->spi, SPIDEV_DISPLAY(0), false);

  /* Unlock the SPI bus */

  SPI_LOCK(priv->spi, false);
}

/****************************************************************************
 * Name: gd32_ili93414ws_cmddata
 *
 * Description:
 *   Set the D/C pin for command or data mode.
 *
 ****************************************************************************/

static inline void gd32_ili93414ws_cmddata(bool cmd)
{
  /* D/C low = command, D/C high = data */

  gd32_gpio_write(GPIO_LCD_DC, !cmd);
}

/****************************************************************************
 * Name: gd32_ili93414ws_sendcmd
 *
 * Description:
 *   Send a command to the LCD driver.
 *
 ****************************************************************************/

static int gd32_ili93414ws_sendcmd(struct ili9341_lcd_s *lcd,
                                   const uint8_t cmd)
{
  struct gd32_ili9341_lcd_s *priv = (struct gd32_ili9341_lcd_s *)lcd;

  lcdinfo("cmd=%02x\n", cmd);

  /* Set to command mode */

  gd32_ili93414ws_cmddata(true);

  /* Send the command byte */

  SPI_SEND(priv->spi, cmd);

  /* Return to data mode */

  gd32_ili93414ws_cmddata(false);

  return OK;
}

/****************************************************************************
 * Name: gd32_ili93414ws_sendparam
 *
 * Description:
 *   Send a parameter to the LCD driver.
 *
 ****************************************************************************/

static int gd32_ili93414ws_sendparam(struct ili9341_lcd_s *lcd,
                                     const uint8_t param)
{
  struct gd32_ili9341_lcd_s *priv = (struct gd32_ili9341_lcd_s *)lcd;

  lcdinfo("param=%02x\n", param);

  /* D/C is already in data mode from sendcmd or previous sendparam */

  SPI_SEND(priv->spi, param);

  return OK;
}

/****************************************************************************
 * Name: gd32_ili93414ws_recvparam
 *
 * Description:
 *   Receive a parameter from the LCD driver.
 *
 ****************************************************************************/

static int gd32_ili93414ws_recvparam(struct ili9341_lcd_s *lcd,
                                     uint8_t *param)
{
  struct gd32_ili9341_lcd_s *priv = (struct gd32_ili9341_lcd_s *)lcd;

  /* Receive one byte */

  *param = (uint8_t)SPI_SEND(priv->spi, 0xff);

  lcdinfo("param=%02x\n", *param);

  return OK;
}

/****************************************************************************
 * Name: gd32_ili93414ws_sendgram
 *
 * Description:
 *   Send pixel data to the LCD GRAM.
 *
 ****************************************************************************/

static int gd32_ili93414ws_sendgram(struct ili9341_lcd_s *lcd,
                                    const uint16_t *wd, uint32_t nwords)
{
  struct gd32_ili9341_lcd_s *priv = (struct gd32_ili9341_lcd_s *)lcd;
  const uint16_t *src = wd;

  lcdinfo("wd=%p, nwords=%" PRIu32 "\n", wd, nwords);

  /* Send pixel data (16-bit per pixel, high byte first) */

  while (nwords-- > 0)
    {
      uint16_t pixel = *src++;
      SPI_SEND(priv->spi, (pixel >> 8));
      SPI_SEND(priv->spi, (pixel & 0xff));
    }

  return OK;
}

/****************************************************************************
 * Name: gd32_ili93414ws_recvgram
 *
 * Description:
 *   Receive pixel data from the LCD GRAM.
 *   Note: Not supported for write-only SPI LCD.
 *
 ****************************************************************************/

static int gd32_ili93414ws_recvgram(struct ili9341_lcd_s *lcd,
                                    uint16_t *wd, uint32_t nwords)
{
  lcdinfo("wd=%p, nwords=%" PRIu32 "\n", wd, nwords);

  /* Not supported for write-only LCD */

  return -ENOSYS;
}

/****************************************************************************
 * Name: gd32_ili93414ws_backlight
 *
 * Description:
 *   Set the backlight level.
 *
 ****************************************************************************/

static int gd32_ili93414ws_backlight(struct ili9341_lcd_s *lcd, int level)
{
  /* Backlight control not implemented */

  return OK;
}

/****************************************************************************
 * Name: gd32_ili93414ws_initialize
 *
 * Description:
 *   Initialize the ILI9341 4-wire SPI sub-driver.
 *
 ****************************************************************************/

static struct ili9341_lcd_s *gd32_ili93414ws_initialize(void)
{
  struct spi_dev_s *spi;
  struct gd32_ili9341_lcd_s *priv = &g_lcddev;

  lcdinfo("Initialize ILI9341 4-wire SPI subdriver\n");

  /* Initialize SPI bus */

  spi = gd32_spibus_initialize(0);
  if (spi == NULL)
    {
      lcderr("ERROR: Failed to initialize SPI bus\n");
      return NULL;
    }

  /* Configure GPIO for LCD control */

  gd32_gpio_config(GPIO_LCD_RESET);
  gd32_gpio_config(GPIO_LCD_DC);

  /* Reset the LCD */

  gd32_gpio_write(GPIO_LCD_RESET, false);
  up_mdelay(20);
  gd32_gpio_write(GPIO_LCD_RESET, true);
  up_mdelay(120);

  /* Initialize the driver structure */

  priv->dev.select    = gd32_ili93414ws_select;
  priv->dev.deselect  = gd32_ili93414ws_deselect;
  priv->dev.sendcmd   = gd32_ili93414ws_sendcmd;
  priv->dev.sendparam = gd32_ili93414ws_sendparam;
  priv->dev.recvparam = gd32_ili93414ws_recvparam;
  priv->dev.sendgram  = gd32_ili93414ws_sendgram;
  priv->dev.recvgram  = gd32_ili93414ws_recvgram;
  priv->dev.backlight = gd32_ili93414ws_backlight;
  priv->spi           = spi;

  return &priv->dev;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_lcd_initialize
 *
 * Description:
 *   Initialize the LCD video hardware.
 *
 ****************************************************************************/

int board_lcd_initialize(void)
{
  struct ili9341_lcd_s *dev;

  lcdinfo("board_lcd_initialize\n");

  /* Check if already initialized */

  if (g_lcd != NULL)
    {
      return OK;
    }

  /* Initialize the SPI sub-driver */

  dev = gd32_ili93414ws_initialize();
  if (dev == NULL)
    {
      lcderr("ERROR: Failed to initialize ILI9341 sub-driver\n");
      return -ENODEV;
    }

  /* Initialize the ILI9341 LCD driver */

  g_lcd = ili9341_initialize(dev, ILI9341_LCD_DEVICE);
  if (g_lcd == NULL)
    {
      lcderr("ERROR: Failed to initialize ILI9341 driver\n");
      return -ENODEV;
    }

  lcdinfo("LCD initialized successfully\n");
  return OK;
}

/****************************************************************************
 * Name: board_lcd_getdev
 *
 * Description:
 *   Return a reference to the LCD object.
 *
 ****************************************************************************/

struct lcd_dev_s *board_lcd_getdev(int lcddev)
{
  if (lcddev == ILI9341_LCD_DEVICE)
    {
      return g_lcd;
    }

  return NULL;
}

/****************************************************************************
 * Name: board_lcd_uninitialize
 *
 * Description:
 *   Uninitialize the LCD Device.
 *
 ****************************************************************************/

void board_lcd_uninitialize(void)
{
  if (g_lcd != NULL)
    {
      /* Set display off */

      g_lcd->setpower(g_lcd, 0);
      g_lcd = NULL;
    }
}
