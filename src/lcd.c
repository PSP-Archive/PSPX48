/*
 *  This file is part of x48, an emulator of the HP-48sx Calculator.
 *  Copyright (C) 1994  Eddie C. Dost  (ecd@dressler.de)
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

/* $Log: lcd.c,v $
 * Revision 1.13  1995/01/11  18:20:01  ecd
 * major update to support HP48 G/GX
 *
 * Revision 1.12  1994/12/08  22:14:50  ecd
 * fixed bug with XShmPutImage causing errors in init_display
 *
 * Revision 1.11  1994/12/07  20:20:50  ecd
 * added support for icon colors
 *
 * Revision 1.10  1994/11/28  02:00:51  ecd
 * added support for colors on icon
 *
 * Revision 1.9  1994/11/02  14:44:28  ecd
 * minor fixes
 *
 * Revision 1.8  1994/10/09  20:32:02  ecd
 * implemented bit offset stuff.
 *
 * Revision 1.7  1994/10/06  16:30:05  ecd
 * added Shared Memory stuff
 *
 * Revision 1.6  1994/10/05  08:36:44  ecd
 * pixmaps for nibble updates
 *
 * Revision 1.5  1994/09/30  12:37:09  ecd
 * new display code makes x48 a lot faster
 *
 * Revision 1.4  1994/09/18  15:29:22  ecd
 * turned off unused rcsid message
 *
 * Revision 1.3  1994/09/13  16:57:00  ecd
 * changed to plain X11
 *
 * Revision 1.2  1994/08/31  18:23:21  ecd
 * changed display initialization.
 *
 * Revision 1.1  1994/08/26  11:09:02  ecd
 * Initial revision
 *
 * $Id: lcd.c,v 1.13 1995/01/11 18:20:01 ecd Exp ecd $
 */


#include "global.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "hp48.h"
#include "hp48_emu.h"
#include "psp_kbd.h"
#include "psp_gr.h"
#include "device.h"

static int last_annunc_state = -1;

display_t display;

#define DISP_ROWS	       64

#define NIBS_PER_BUFFER_ROW    (NIBBLES_PER_ROW + 2)

unsigned char disp_buf[DISP_ROWS][NIBS_PER_BUFFER_ROW];
unsigned char lcd_buffer[DISP_ROWS][NIBS_PER_BUFFER_ROW];

void
init_display(void)
{
  display.on = (int)(saturn.disp_io & 0x8) >> 3;

  display.disp_start = (saturn.disp_addr & 0xffffe);
  display.offset = (saturn.disp_io & 0x7);

  display.lines = (saturn.line_count & 0x3f);
  if (display.lines == 0)
    display.lines = 63;

  if (display.offset > 3)
    display.nibs_per_line = (NIBBLES_PER_ROW+saturn.line_offset+2) & 0xfff;
  else
    display.nibs_per_line = (NIBBLES_PER_ROW+saturn.line_offset) & 0xfff;

  display.disp_end = display.disp_start +
	             (display.nibs_per_line * (display.lines + 1));

  display.menu_start = saturn.menu_addr;
  display.menu_end = saturn.menu_addr + 0x110;

  display.contrast = saturn.contrast_ctrl;
  display.contrast |= ((saturn.disp_test & 0x1) << 4);

  display.annunc = saturn.annunc;

  memset(disp_buf, 0xf0, sizeof(disp_buf));
  memset(lcd_buffer, 0xf0, sizeof(lcd_buffer));
}

static inline void
draw_nibble(int c, int r, int val)
{
  int x, y;

  //x = (c * 8) + 5;
  //y = (r * 2) + 20;
  x = (c * 8);
  y = (r * 2) + 20;
  val &= 0x0f;
  if (val != lcd_buffer[r][c]) {
    psp_display_nibbles(val, x, y);
    lcd_buffer[r][c] = val;
  }
}

static inline void
draw_row(long addr, int row)
{
  int i, v;
  int line_length;

  line_length = NIBBLES_PER_ROW;
  if ((display.offset > 3) && (row <= display.lines))
    line_length += 2;
  for (i = 0; i < line_length; i++) {
    v = read_nibble(addr + i);
    if (v != disp_buf[row][i]) {
      disp_buf[row][i] = v;
      draw_nibble(i, row, v);
    }
  }
}

void
update_display(void)
{
  int i, j;
  long addr;
  static int old_offset = -1;
  static int old_lines = -1;
  if (display.on) {
    addr = display.disp_start;
      if (display.offset != old_offset) {
        memset(disp_buf, 0xf0,
               (size_t)((display.lines+1) * NIBS_PER_BUFFER_ROW));
        memset(lcd_buffer, 0xf0,
               (size_t)((display.lines+1) * NIBS_PER_BUFFER_ROW));
        old_offset = display.offset;
      }
      if (display.lines != old_lines) {
        memset(&disp_buf[56][0], 0xf0, (size_t)(8 * NIBS_PER_BUFFER_ROW));
        memset(&lcd_buffer[56][0], 0xf0, (size_t)(8 * NIBS_PER_BUFFER_ROW));
        old_lines = display.lines;
      }
      for (i = 0; i <= display.lines; i++) {
        draw_row(addr, i);
        addr += display.nibs_per_line;
      }
    if (i < DISP_ROWS) {
      addr = display.menu_start;
        for (; i < DISP_ROWS; i++) {
          draw_row(addr, i);
          addr += NIBBLES_PER_ROW;
        }
    }
  } else {
      memset(disp_buf, 0xf0, sizeof(disp_buf));
      for (i = 0; i < 64; i++) {
        for (j = 0; j < NIBBLES_PER_ROW; j++) {
          draw_nibble(j, i, 0x00);
        }
      }
  }
}

void
redraw_display(void)
{
  psp_clear_lcd();
  memset(disp_buf, 0, sizeof(disp_buf));
  memset(lcd_buffer, 0, sizeof(lcd_buffer));
  update_display();
}

void
disp_draw_nibble(word_20 addr, word_4 val)
{
  long offset;
  int x, y;

  offset = (addr - display.disp_start);
  x = offset % display.nibs_per_line;
  if (x < 0 || x > 35)
    return;
  if (display.nibs_per_line != 0) {
    y = offset / display.nibs_per_line;
    if (y < 0 || y > 63)
      return;
      if (val != disp_buf[y][x]) {
        disp_buf[y][x] = val;
        draw_nibble(x, y, val);
      }
  } else {
      for (y = 0; y < display.lines; y++) {
        if (val != disp_buf[y][x]) {
          disp_buf[y][x] = val;
          draw_nibble(x, y, val);
        }
      }
  }
}

void
menu_draw_nibble(word_20 addr, word_4 val)
{
  long offset;
  int x, y;

  offset = (addr - display.menu_start);
    x = offset % NIBBLES_PER_ROW;
    y = display.lines + (offset / NIBBLES_PER_ROW) + 1;
    if (val != disp_buf[y][x]) {
      disp_buf[y][x] = val;
      draw_nibble(x, y, val);
    }
}

void
draw_annunc(void)
{
  int val;
  val = display.annunc;
  if (val == last_annunc_state)
    return;
  last_annunc_state = val;
  psp_display_annunc(val);
}

void
redraw_annunc(void)
{
  last_annunc_state = -1;
  draw_annunc();
}

