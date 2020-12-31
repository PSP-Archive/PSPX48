/*
 *  x48 port on PSP 
 *
 *  Copyright (C) 2006 Ludovic Jacomme (ludovic.jacomme@gmail.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>

# include <pspctrl.h>

#include "global.h"
#include "psp_kbd.h"
#include "psp_pg.h"
#include "psp_gr.h"

#include "hp48.h"

#include "device.h"
#include "annunc.h"

/* Colors */

# define PSP_LCD_BACKGROUND psp_pg_rgb(202, 221, 92)
# define PSP_LCD_FOREGROUND psp_pg_rgb(0x0, 0x0, 0x0)

# define PSP_LCD_X          330
# define PSP_LCD_Y          3
# define PSP_LCD_DX         144
# define PSP_LCD_DY         264
# define PSP_ANNUNC_X       460

/* Plane for LCD */
static unsigned char loc_nibbles[16][2] =
{
  { 0x00, 0x00 },  /* ---- */
  { 0x03, 0x03 },  /* *--- */
  { 0x0c, 0x0c },  /* -*-- */
  { 0x0f, 0x0f },  /* **-- */
  { 0x30, 0x30 },  /* --*- */
  { 0x33, 0x33 },  /* *-*- */
  { 0x3c, 0x3c },  /* -**- */
  { 0x3f, 0x3f },  /* ***- */
  { 0xc0, 0xc0 },  /* ---* */
  { 0xc3, 0xc3 },  /* *--* */
  { 0xcc, 0xcc },  /* -*-* */
  { 0xcf, 0xcf },  /* **-* */
  { 0xf0, 0xf0 },  /* --** */
  { 0xf3, 0xf3 },  /* *-** */
  { 0xfc, 0xfc },  /* -*** */
  { 0xff, 0xff }  /* **** */
};


int
psp_display_nibbles(int id, int x, int y)
{
  unsigned char mask;
  int x1;
  int y1;
  int xp;
  int yp;
  int color;
  int color_fg = PSP_LCD_FOREGROUND;
  int color_bg = PSP_LCD_BACKGROUND;

  for (y1 = 0; y1 < 2; y1++ ) {
    mask = 0x80;
    for (x1 = 7; x1 >= 0; x1--) {
      xp = PSP_LCD_X + PSP_LCD_DX - (y + y1);
      yp = PSP_LCD_Y + x + x1;
      if (loc_nibbles[id][y1] & mask) color = color_fg;
      else                            color = color_bg;
      if (yp <= (PSP_LCD_Y + PSP_LCD_DY)) {
        psp_pg_put_pixel( xp, yp, color);
      }
      mask = mask >> 1;
    }
  }
  return 0;
}

struct ann_struct {
  int            bit;
  int            x;
  int            y;
  unsigned int   width;
  unsigned int   height;
  unsigned char *bits;
} ann_tbl[] = {
  { ANN_LEFT,     460,  16, ann_left_width, ann_left_height, ann_left_bits },
  { ANN_RIGHT,    460,  61, ann_right_width, ann_right_height, ann_right_bits },
  { ANN_ALPHA,    460, 106, ann_alpha_width, ann_alpha_height, ann_alpha_bits },
  { ANN_BATTERY,  460, 151, ann_battery_width, ann_battery_height, ann_battery_bits },
  { ANN_BUSY,     460, 196, ann_busy_width, ann_busy_height, ann_busy_bits },
  { ANN_IO,       460, 241, ann_io_width, ann_io_height, ann_io_bits },
  { 0 }
};

static int 
psp_display_ann_bitmap(int x, int y, short *bits)
{
  unsigned short mask;
  int x1;
  int y1;
  int xp;
  int yp;
  int color;
  int color_fg = PSP_LCD_FOREGROUND;
  int color_bg = PSP_LCD_BACKGROUND;

  for (y1 = 0; y1 < 12; y1++ ) {
    mask = 0x8000;
    for (x1 = 15; x1 >= 0; x1--) {
      xp = x + 12 - y1;
      yp = y + x1;
      if (bits[y1] & mask) color = color_fg;
      else                 color = color_bg;
      if (yp < (PSP_LCD_Y + PSP_LCD_DY)) {
        psp_pg_put_pixel( xp, yp, color);
      }
      mask = mask >> 1;
    }
  }
  
  return 0;
   
}

int
psp_display_annunc(int val)
{
  int i;
  for (i = 0; ann_tbl[i].bit; i++)
  {
    if ((ann_tbl[i].bit & val) == ann_tbl[i].bit)
    {
      psp_display_ann_bitmap(ann_tbl[i].x, 
                             ann_tbl[i].y,
                             (short *)ann_tbl[i].bits );
    }
    else
    {
      psp_pg_fill_rectangle(ann_tbl[i].x, 
                            ann_tbl[i].y,
                            ann_tbl[i].width  + 1,
                            ann_tbl[i].height + 2,
                            PSP_LCD_BACKGROUND, PG_DEF);
    }
  }
  return 0;
}

int 
psp_clear_lcd(void)
{
  psp_pg_fill_rectangle( PSP_LCD_X, PSP_LCD_Y, PSP_LCD_DX, PSP_LCD_DY, PSP_LCD_BACKGROUND, PG_DEF);
  return 0;
}

