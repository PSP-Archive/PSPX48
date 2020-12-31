/*
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

#include <pspctrl.h>
#include <pspkernel.h>
#include <pspdebug.h>

#include "global.h"
#include "hp48.h"
#include "psp_kbd.h"
#include "psp_gr.h"
#include "psp_pg.h"
#include "psp_battery.h"
#include "psp_menu.h"
#include "psp_menu_help.h"


# define KBD_MIN_START_TIME       800000
# define KBD_MAX_EVENT_TIME       500000
# define KBD_MIN_EVENT_TIME       180000
# define KBD_MIN_ANALOG_TIME1     150000
# define KBD_MIN_ANALOG_TIME2     100000
# define KBD_MIN_BATTCHECK_TIME 90000000 

 static SceCtrlData    loc_button_data;
 static SceCtrlData    loc_analog_data;
 static unsigned int   loc_last_event_time = 0;
 static unsigned int   loc_last_analog_time = 0;
 static int            loc_analog_x_released = 0;
 static int            loc_analog_y_released = 0;
 static long           first_time_stamp = -1;
 static char           loc_button_press[ KBD_MAX_BUTTONS ]; 
 static char           loc_button_release[ KBD_MAX_BUTTONS ]; 
 static unsigned int   loc_button_mask[ KBD_MAX_BUTTONS ] =
 {
   PSP_CTRL_UP         , /*  KBD_UP         */
   PSP_CTRL_RIGHT      , /*  KBD_RIGHT      */
   PSP_CTRL_DOWN       , /*  KBD_DOWN       */
   PSP_CTRL_LEFT       , /*  KBD_LEFT       */
   PSP_CTRL_TRIANGLE   , /*  KBD_TRIANGLE   */
   PSP_CTRL_CIRCLE     , /*  KBD_CIRCLE     */
   PSP_CTRL_CROSS      , /*  KBD_CROSS      */
   PSP_CTRL_SQUARE     , /*  KBD_SQUARE     */
   PSP_CTRL_SELECT     , /*  KBD_SELECT     */
   PSP_CTRL_START      , /*  KBD_START      */
   PSP_CTRL_HOME       , /*  KBD_HOME       */
   PSP_CTRL_HOLD       , /*  KBD_HOLD       */
   PSP_CTRL_LTRIGGER   , /*  KBD_LTRIGGER   */
   PSP_CTRL_RTRIGGER   , /*  KBD_RTRIGGER   */
 };

int
key_event(int b, int button_pressed)
{
  int code;
  int i, r, c;
  int all_up;

  code = hp48_buttons_gx[b].code;
  if (button_pressed) {
    hp48_buttons_gx[b].key_down = 1;
    hp48_buttons_gx[b].pressed = 1;
    //DrawButton(b);
    if (code == 0x8000) {
      for (i = 0; i < 9; i++)
        saturn.keybuf.rows[i] |= 0x8000;
      do_kbd_int();
    } else {
      r = code >> 4;
      c = 1 << (code & 0xf);
      if ((saturn.keybuf.rows[r] & c) == 0) {
        if (saturn.kbd_ien) {
          do_kbd_int();
        }
        saturn.keybuf.rows[r] |= c;
      }
    }

  } else {
    hp48_buttons_gx[b].key_down = 0;
    all_up = 1;
    for (i = HP48_BUTTON_A; i <= HP48_LAST_BUTTON; i++) {
      if (hp48_buttons_gx[i].pressed) {
        if (hp48_buttons_gx[i].key_down) {
          all_up = 0;
        } else {
          hp48_buttons_gx[i].pressed = 0;
          //DrawButton(i);
          if (code == 0x8000) {
            for (i = 0; i < 9; i++)
              saturn.keybuf.rows[i] &= ~0x8000;
          } else {
            r = code >> 4;
            c = 1 << (code & 0xf);
            saturn.keybuf.rows[r] &= ~c;
          }
        }
      }
    }
    if (all_up) {
      memset(&saturn.keybuf, 0, sizeof(saturn.keybuf));
    }
  }
  return 0;
}

int
hp48_decode_key(int psp_b, int button_pressed)
{
  int wake = 0;
  int reverse_analog = HP48.psp_reverse_analog;

  if (reverse_analog) {
    if ((psp_b >= KBD_JOY_UP  ) &&
        (psp_b <= KBD_JOY_LEFT)) {
      psp_b = psp_b - KBD_JOY_UP + KBD_UP;
    } else
    if ((psp_b >= KBD_UP  ) &&
        (psp_b <= KBD_LEFT)) {
      psp_b = psp_b - KBD_UP + KBD_JOY_UP;
    }
  }

  switch ( psp_b )  
  {
    case KBD_CROSS    : wake = 1;
                        key_event( HP48_BUTTON_ENTER, button_pressed);
    break;
    case KBD_SQUARE   : wake = 1;
                        key_event( hp48_cursor_button, button_pressed);
    break;
    case KBD_TRIANGLE : wake = 1;
                        key_event( HP48_BUTTON_BS, button_pressed);
    break;
    case KBD_CIRCLE   : wake = 1;
                        key_event( HP48_BUTTON_ALPHA, button_pressed);
    break;

    case KBD_RIGHT    : wake = 1;
                        key_event( HP48_BUTTON_UP, button_pressed);
    break;
    case KBD_LEFT     : wake = 1;
                        key_event( HP48_BUTTON_DOWN, button_pressed);
    break;
    case KBD_DOWN     : wake = 1;
                        key_event( HP48_BUTTON_RIGHT, button_pressed);
    break;
    case KBD_UP       : wake = 1;
                        key_event( HP48_BUTTON_LEFT, button_pressed);
    break;

    case KBD_START    : wake = 1;
                        key_event( HP48_BUTTON_ON, button_pressed);
    break;
    case KBD_SELECT   : wake = 1;
                        key_event( HP48_BUTTON_NXT, button_pressed);
    break;
    case KBD_LTRIGGER : wake = 1;
                        key_event( HP48_BUTTON_SHL, button_pressed);
    break;
    case KBD_RTRIGGER : wake = 1;
                        key_event( HP48_BUTTON_SHR, button_pressed);
    break;
    case KBD_JOY_UP     : if (button_pressed) hp48_move_cursor_button_left();
    break;
    case KBD_JOY_RIGHT  : if (button_pressed) hp48_move_cursor_button_up();
    break;
    case KBD_JOY_DOWN   : if (button_pressed) hp48_move_cursor_button_right();
    break;
    case KBD_JOY_LEFT   : if (button_pressed) hp48_move_cursor_button_down();
    break;
  }

  return wake;
}

# define ANALOG_THRESHOLD  40

void 
kbd_get_analog_direction(int Analog_x, int Analog_y, int *x, int *y)
{
  int DeltaX = 0;
  int DeltaY = 0;
  int DirX   = 0;
  int DirY   = 0;

  *x = 0;
  *y = 0;

  if (Analog_x <=        ANALOG_THRESHOLD)  { DeltaX = 128 - Analog_x; DirX = -1; }
  else 
  if (Analog_x >= (255 - ANALOG_THRESHOLD)) { DeltaX = Analog_x - 128; DirX = 1; }

  if (Analog_y <=        ANALOG_THRESHOLD)  { DeltaY = 128 - Analog_y; DirY = -1; }
  else 
  if (Analog_y >= (255 - ANALOG_THRESHOLD)) { DeltaY = Analog_y - 128; DirY = 1; }

  if (DeltaX > DeltaY) DirY = 0;
  else                 DirX = 0;

  *x = DirX;
  *y = DirY;
}

int
kbd_scan_keyboard(void)
{
  SceCtrlData c;
  int         analog_keyboard = ! HP48.psp_reverse_analog;
  long        delta_stamp;
  int         event;
  int         b;

  int new_Lx;
  int new_Ly;
  int old_Lx;
  int old_Ly;

  event = 0;
  sceCtrlPeekBufferPositive(&c, 1);
  c.Buttons &= PSP_ALL_BUTTON_MASK;

  if ((c.Buttons & (PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER|PSP_CTRL_START)) ==
      (PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER|PSP_CTRL_START)) {
    /* Exit ! */
    psp_exit(0);
  }

  delta_stamp = c.TimeStamp - first_time_stamp;
  if ((delta_stamp < 0) || (delta_stamp > KBD_MIN_BATTCHECK_TIME)) {
    first_time_stamp = c.TimeStamp;
    if (psp_is_low_battery()) {
      psp_main_menu();
      psp_init_keyboard();
      return 0;
    }
  }

  if ((c.Buttons & (PSP_CTRL_LTRIGGER|PSP_CTRL_SELECT)) == (PSP_CTRL_LTRIGGER|PSP_CTRL_SELECT)) {

    psp_main_menu();
    psp_init_keyboard();
    return 0;
  } else
  if ((c.Buttons & (PSP_CTRL_LTRIGGER|PSP_CTRL_START)) == (PSP_CTRL_LTRIGGER|PSP_CTRL_START)) {

    psp_help_menu(1);
    psp_init_keyboard();
    return 0;
  } else
  if ((c.Buttons & (PSP_CTRL_RTRIGGER|PSP_CTRL_START)) == (PSP_CTRL_RTRIGGER|PSP_CTRL_START)) {

    psp_kbd_wait_no_button();
    HP48.psp_reverse_analog = ! HP48.psp_reverse_analog;
    psp_init_keyboard();
    return 0;
  }

  /* Check Analog Device */
  kbd_get_analog_direction(loc_analog_data.Lx,loc_analog_data.Ly,&old_Lx,&old_Ly);
  kbd_get_analog_direction(c.Lx, c.Ly, &new_Lx, &new_Ly);

  if ( ((  analog_keyboard) && ((c.TimeStamp - loc_last_analog_time) > KBD_MIN_ANALOG_TIME1)) ||
       ((! analog_keyboard) && ((c.TimeStamp - loc_last_analog_time) > KBD_MIN_ANALOG_TIME2)) )
  {
    /* Analog device has moved */
    if (new_Lx > 0) {
      if (old_Lx > 0) event |= hp48_decode_key(KBD_JOY_LEFT , 0);
      if ((old_Lx != new_Lx) || analog_keyboard) event |= hp48_decode_key(KBD_JOY_RIGHT, 1);
      loc_analog_x_released = 0;
    
    } else 
    if (new_Lx < 0) {
      if (old_Lx  < 0) event |= hp48_decode_key(KBD_JOY_RIGHT, 0);
      if ((old_Lx != new_Lx) || analog_keyboard) event |= hp48_decode_key(KBD_JOY_LEFT , 1);
      loc_analog_x_released = 0;
    
    } else {
      if (old_Lx  > 0) event |= hp48_decode_key(KBD_JOY_LEFT , 0);
      else
      if (old_Lx  < 0) event |= hp48_decode_key(KBD_JOY_RIGHT, 0);
      else if (! loc_analog_x_released) {
        event |= hp48_decode_key(KBD_JOY_LEFT  , 0);
        event |= hp48_decode_key(KBD_JOY_RIGHT , 0);
        loc_analog_x_released = 1;
      }
    }

    if (new_Ly < 0) {
      if (old_Ly  > 0) event |= hp48_decode_key(KBD_JOY_DOWN , 0);
      if ((old_Ly != new_Ly) || analog_keyboard) event |= hp48_decode_key(KBD_JOY_UP   , 1);
      loc_analog_y_released = 0;
    
    } else 
    if (new_Ly > 0) {
      if (old_Ly  < 0) event |= hp48_decode_key(KBD_JOY_UP   , 0);
      if ((old_Ly != new_Ly) || analog_keyboard) event |= hp48_decode_key(KBD_JOY_DOWN , 1);
      loc_analog_y_released = 0;
    
    } else {
      if (old_Ly  > 0) event |= hp48_decode_key(KBD_JOY_DOWN , 0);
      else
      if (old_Ly  < 0) event |= hp48_decode_key(KBD_JOY_UP   , 0);
      else if (! loc_analog_y_released) {
        event |= hp48_decode_key(KBD_JOY_DOWN , 0);
        event |= hp48_decode_key(KBD_JOY_UP   , 0);
        loc_analog_y_released = 1;
      }
    }
    loc_last_analog_time = c.TimeStamp;
    memcpy(&loc_analog_data,&c,sizeof(SceCtrlData));
  }

  if  (((c.TimeStamp - loc_last_event_time) > KBD_MIN_EVENT_TIME) || 
        (c.Buttons != loc_button_data.Buttons))
  {
    for (b = 0; b < KBD_MAX_BUTTONS; b++) 
    {
      if (c.Buttons & loc_button_mask[b]) {
        if (!(analog_keyboard) && (b >= KBD_UP) && (b <= KBD_LEFT)) {
          loc_button_press[b]   = 1;
          loc_button_release[b] = 0;
          event = 1;
        } else
        if (!(loc_button_data.Buttons & loc_button_mask[b])) {
          loc_button_press[b] = 1;
          loc_button_release[b] = 0;
          event = 1;
        }
      } else {
        if (loc_button_data.Buttons & loc_button_mask[b]) {
          loc_button_release[b] = 1;
          loc_button_press[b]   = 0;
          event = 1;
        }
      }
    }

    loc_last_event_time = c.TimeStamp;
    memcpy(&loc_button_data,&c,sizeof(SceCtrlData));
  }

  return event;
}

void
kbd_wait_start(void)
{
  while (1)
  {
    SceCtrlData c;
    sceCtrlReadBufferPositive(&c, 1);
    c.Buttons &= PSP_ALL_BUTTON_MASK;
    if (c.Buttons & PSP_CTRL_START) break;
  }
}

void
psp_init_keyboard(void)
{
  memset(&saturn.keybuf, 0, sizeof(saturn.keybuf));
}

void
psp_kbd_wait_no_button(void)
{
  SceCtrlData c;

  do {
   sceCtrlPeekBufferPositive(&c, 1);
   c.Buttons &= PSP_ALL_BUTTON_MASK;

  } while (c.Buttons != 0);
} 

int
psp_update_keys(void)
{
  int         b;
  int         wake = 0;

  static char first_time = 1;
  static long first_time_stamp = -1;
  static int release_pending = 0;
  static int release_button  = 0;

  if (first_time) {

    SceCtrlData c;
    sceCtrlPeekBufferPositive(&c, 1);
    c.Buttons &= PSP_ALL_BUTTON_MASK;

    if (first_time_stamp == -1) first_time_stamp = c.TimeStamp;
    if ((! c.Buttons) && ((c.TimeStamp - first_time_stamp) < KBD_MIN_START_TIME)) return 0;
    
    update_display();
    redraw_annunc();

    first_time      = 0;
    release_pending = 0;
    release_button  = HP48_BUTTON_ON;

    for (b = 0; b < KBD_MAX_BUTTONS; b++) {
      loc_button_release[b] = 0;
      loc_button_press[b] = 0;
    }
    sceCtrlPeekBufferPositive(&loc_button_data, 1);
    loc_button_data.Buttons &= PSP_ALL_BUTTON_MASK;
    sceCtrlPeekBufferPositive(&loc_analog_data, 1);
    loc_analog_data.Buttons &= PSP_ALL_BUTTON_MASK;

    psp_main_menu();
    psp_init_keyboard();

    return 0;
  }

  if (psp_screenshot_mode) {
    psp_screenshot_mode--;
    if (psp_screenshot_mode <= 0) {
      hp48_hide_cursor_button();
      psp_save_screenshot();
      psp_screenshot_mode = 0;
      hp48_show_cursor_button();
    }
  }

  hp48_show_cursor_button();

  if (release_pending)
  {
    release_pending = 0;
    for (b = 0; b < KBD_MAX_BUTTONS; b++) {
      if (loc_button_release[b]) {
        loc_button_release[b] = 0;
        loc_button_press[b] = 0;
        wake |= hp48_decode_key(b, 0);
      }
    }
  }

  wake |= kbd_scan_keyboard();

  /* check press event */
  for (b = 0; b < KBD_MAX_BUTTONS; b++) {
    if (loc_button_press[b]) {
      loc_button_press[b] = 0;
      release_pending     = 0;
      wake |= hp48_decode_key(b, 1);
    }
  }
  /* check release event */
  for (b = 0; b < KBD_MAX_BUTTONS; b++) {
    if (loc_button_release[b]) {
      release_pending = 1;
      break;
    }
  }

  return wake;
}
