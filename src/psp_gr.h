/*
 *  xtiger port on PSP 
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

#ifndef _PSP_GR_H_
#define _PSP_GR_H_

  extern int PSP_LCD_BACKGROUND;
  extern int PSP_LCD_FOREGROUND;
  extern int PSP_LCD_X;
  extern int PSP_LCD_Y;

  extern long          *hp48_xor_color;
  extern hp48_button_t  hp48_buttons_gx[];

  extern int hp48_cursor_button;
  extern int hp48_cursor_button_on;

  extern void  psp_init_graphic(void);
  extern void  psp_change_skin(int skin);

  extern void hp48_move_cursor_button_right(void);
  extern void hp48_move_cursor_button_left(void);
  extern void hp48_move_cursor_button_up(void);
  extern void hp48_move_cursor_button_down(void);
  extern void hp48_hide_cursor_button(void);
  extern void hp48_show_cursor_button(void);
  extern void psp_draw_menu_background();
  
  extern u32* psp_skin_background;
  extern u32* psp_menu_background;

# endif
