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


  int hp48_cursor_button = HP48_BUTTON_ON;
  int hp48_cursor_button_on = 0;

  hp48_button_t hp48_buttons_gx[] = 
  {
    { "A",       0, 0, 0x14, 301,   6, 18, 32 }, 
    { "B",       0, 0, 0x84, 301,  51, 18, 32 }, 
    { "C",       0, 0, 0x83, 301,  96, 18, 32 }, 
    { "D",       0, 0, 0x82, 301, 141, 18, 32 }, 
    { "E",       0, 0, 0x81, 301, 186, 18, 32 }, 
    { "F",       0, 0, 0x80, 301, 231, 18, 32 }, 

    { "MTH",     0, 0, 0x24, 262,   6, 19, 32 }, 
    { "PRG",     0, 0, 0x74, 262,  51, 19, 32 }, 
    { "CST",     0, 0, 0x73, 262,  96, 19, 32 }, 
    { "VAR",     0, 0, 0x72, 262, 141, 19, 32 }, 
    { "UP",      0, 0, 0x71, 262, 186, 19, 32 }, 
    { "NXT",     0, 0, 0x70, 262, 231, 19, 32 }, 

    { "COLON",   0, 0, 0x04, 225,   6, 19, 32 }, 
    { "STO",     0, 0, 0x64, 225,  51, 19, 32 }, 
    { "EVAL",    0, 0, 0x63, 225,  96, 19, 32 }, 
    { "LEFT",    0, 0, 0x62, 225, 141, 19, 32 }, 
    { "DOWN",    0, 0, 0x61, 225, 186, 19, 32 }, 
    { "RIGHT",   0, 0, 0x60, 225, 231, 19, 32 }, 

    { "SIN",     0, 0, 0x34, 188,   6, 19, 32 }, 
    { "COS",     0, 0, 0x54, 188,  51, 19, 32 }, 
    { "TAN",     0, 0, 0x53, 188,  96, 19, 32 }, 
    { "SQRT",    0, 0, 0x52, 188, 141, 19, 32 }, 
    { "POWER",   0, 0, 0x51, 188, 186, 19, 32 }, 
    { "INV",     0, 0, 0x50, 188, 231, 19, 32 }, 

    { "ENTER",   0, 0, 0x44, 151,   6, 19, 77 }, 
    { "NEG",     0, 0, 0x43, 151,  96, 19, 32 }, 
    { "EEX",     0, 0, 0x42, 151, 141, 19, 32 }, 
    { "DEL",     0, 0, 0x41, 151, 186, 19, 32 }, 
    { "BS",      0, 0, 0x40, 151, 231, 19, 32 }, 

    { "ALPHA",   0, 0, 0x35, 114,   6, 18, 32 }, 
    { "7",       0, 0, 0x33, 114,  60, 18, 41 }, 
    { "8",       0, 0, 0x32, 114, 114, 18, 41 }, 
    { "9",       0, 0, 0x31, 114, 168, 18, 41 }, 
    { "DIV",     0, 0, 0x30, 114, 222, 18, 41 }, 

    { "SHL",     0, 0, 0x25,  77,   6, 18, 32 }, 
    { "4",       0, 0, 0x23,  77,  60, 18, 41 }, 
    { "5",       0, 0, 0x22,  77, 114, 18, 41 }, 
    { "6",       0, 0, 0x21,  77, 168, 18, 41 }, 
    { "MUL",     0, 0, 0x20,  77, 222, 18, 41 }, 

    { "SHR",     0, 0, 0x15,  39,   6, 19, 32 }, 
    { "1",       0, 0, 0x13,  39,  60, 19, 41 }, 
    { "2",       0, 0, 0x12,  39, 114, 19, 41 }, 
    { "3",       0, 0, 0x11,  39, 168, 19, 41 }, 
    { "MINUS",   0, 0, 0x10,  39, 222, 19, 41 }, 

    { "ON",      0, 0, 0x8000, 2,   6, 19, 32 }, 
    { "0" ,      0, 0, 0x03,   2,  60, 19, 41 }, 
    { "PERIOD",  0, 0, 0x02,   2, 114, 19, 41 }, 
    { "SPC",     0, 0, 0x01,   2, 168, 19, 41 }, 
    { "PLUS",    0, 0, 0x00,   2, 222, 19, 41 }, 

    { 0 }
  };


  u32* psp_skin_background = NULL;
  u32* psp_menu_background = NULL;

void
psp_init_graphic()
{
  char BackgroundFileName[128];
  strcpy(BackgroundFileName, "x48.png");
 
  psp_pg_init();

  if (psp_skin_background != NULL) {
    free(psp_skin_background);
    psp_skin_background = NULL;
  }
  psp_skin_background = psp_pg_load_background_png(BackgroundFileName);
  psp_pg_blit_background(psp_skin_background);

  if (psp_menu_background == NULL) {
    psp_menu_background = psp_pg_load_background_png("background.png");
  }
}

void
hp48_show_cursor_button(void)
{
  int b = hp48_cursor_button;
  if (! hp48_cursor_button_on) {
    hp48_cursor_button_on = 1;
    psp_pg_fill_rectangle( hp48_buttons_gx[b].x, 
                     hp48_buttons_gx[b].y,
                     hp48_buttons_gx[b].w,
                     hp48_buttons_gx[b].h, psp_pg_rgb(0xFF,0xFF,0xFF), PG_XOR);
  }
}

void
hp48_hide_cursor_button(void)
{
  int b = hp48_cursor_button;
  if (hp48_cursor_button_on) {
    hp48_cursor_button_on = 0;
    psp_pg_fill_rectangle( hp48_buttons_gx[b].x, 
                     hp48_buttons_gx[b].y,
                     hp48_buttons_gx[b].w,
                     hp48_buttons_gx[b].h, psp_pg_rgb(0xFF,0xFF,0xFF), PG_XOR);
  }
}

void
hp48_move_cursor_button_right(void)
{
  if ((hp48_cursor_button >= HP48_BUTTON_ON) && 
      (hp48_cursor_button <  HP48_BUTTON_PLUS)) {
    hp48_hide_cursor_button();
    hp48_cursor_button++;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_SHR  ) && 
      (hp48_cursor_button <  HP48_BUTTON_MINUS)) {
    hp48_hide_cursor_button();
    hp48_cursor_button++;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_SHL) && 
      (hp48_cursor_button <  HP48_BUTTON_MUL)) {
    hp48_hide_cursor_button();
    hp48_cursor_button++;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_ALPHA) && 
      (hp48_cursor_button <  HP48_BUTTON_DIV)) {
    hp48_hide_cursor_button();
    hp48_cursor_button++;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_ENTER) && 
      (hp48_cursor_button <  HP48_BUTTON_BS)) {
    hp48_hide_cursor_button();
    hp48_cursor_button++;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_SIN) && 
      (hp48_cursor_button <  HP48_BUTTON_INV)) {
    hp48_hide_cursor_button();
    hp48_cursor_button++;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_COLON) && 
      (hp48_cursor_button <  HP48_BUTTON_RIGHT)) {
    hp48_hide_cursor_button();
    hp48_cursor_button++;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_MTH) && 
      (hp48_cursor_button <  HP48_BUTTON_NXT)) {
    hp48_hide_cursor_button();
    hp48_cursor_button++;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_A) && 
      (hp48_cursor_button <  HP48_BUTTON_F)) {
    hp48_hide_cursor_button();
    hp48_cursor_button++;
  }
}

void
hp48_move_cursor_button_left(void)
{
  if ((hp48_cursor_button >  HP48_BUTTON_ON) && 
      (hp48_cursor_button <= HP48_BUTTON_PLUS)) {
    hp48_hide_cursor_button();
    hp48_cursor_button--;
  } else
  if ((hp48_cursor_button >  HP48_BUTTON_SHR  ) && 
      (hp48_cursor_button <= HP48_BUTTON_MINUS)) {
    hp48_hide_cursor_button();
    hp48_cursor_button--;
  } else
  if ((hp48_cursor_button >  HP48_BUTTON_SHL) && 
      (hp48_cursor_button <= HP48_BUTTON_MUL)) {
    hp48_hide_cursor_button();
    hp48_cursor_button--;
  } else
  if ((hp48_cursor_button >  HP48_BUTTON_ALPHA) && 
      (hp48_cursor_button <= HP48_BUTTON_DIV)) {
    hp48_hide_cursor_button();
    hp48_cursor_button--;
  } else
  if ((hp48_cursor_button >  HP48_BUTTON_ENTER) && 
      (hp48_cursor_button <= HP48_BUTTON_BS)) {
    hp48_hide_cursor_button();
    hp48_cursor_button--;
  } else
  if ((hp48_cursor_button >  HP48_BUTTON_SIN) && 
      (hp48_cursor_button <= HP48_BUTTON_INV)) {
    hp48_hide_cursor_button();
    hp48_cursor_button--;
  } else
  if ((hp48_cursor_button >  HP48_BUTTON_COLON) && 
      (hp48_cursor_button <= HP48_BUTTON_RIGHT)) {
    hp48_hide_cursor_button();
    hp48_cursor_button--;
  } else
  if ((hp48_cursor_button >  HP48_BUTTON_MTH) && 
      (hp48_cursor_button <= HP48_BUTTON_NXT)) {
    hp48_hide_cursor_button();
    hp48_cursor_button--;
  } else
  if ((hp48_cursor_button >  HP48_BUTTON_A) && 
      (hp48_cursor_button <= HP48_BUTTON_F)) {
    hp48_hide_cursor_button();
    hp48_cursor_button--;
  }
}

void
hp48_move_cursor_button_up(void)
{
  if ((hp48_cursor_button >= HP48_BUTTON_ON) && 
      (hp48_cursor_button <= HP48_BUTTON_PLUS)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_ON) + HP48_BUTTON_SHR;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_SHR  ) && 
      (hp48_cursor_button <= HP48_BUTTON_MINUS)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_SHR) + HP48_BUTTON_SHL;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_SHL) && 
      (hp48_cursor_button <= HP48_BUTTON_MUL)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_SHL) + HP48_BUTTON_ALPHA;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_ALPHA) && 
      (hp48_cursor_button <= HP48_BUTTON_DIV)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_ALPHA) + HP48_BUTTON_ENTER;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_ENTER) && 
      (hp48_cursor_button <= HP48_BUTTON_BS)) {
    hp48_hide_cursor_button();
    if (hp48_cursor_button == HP48_BUTTON_ENTER) {
      hp48_cursor_button = HP48_BUTTON_SIN;
    } else {
      hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_ENTER) + HP48_BUTTON_COS;
    }
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_SIN) && 
      (hp48_cursor_button <= HP48_BUTTON_INV)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_SIN) + HP48_BUTTON_COLON;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_COLON) && 
      (hp48_cursor_button <= HP48_BUTTON_RIGHT)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_COLON) + HP48_BUTTON_MTH;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_MTH) && 
      (hp48_cursor_button <= HP48_BUTTON_NXT)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_MTH) + HP48_BUTTON_A;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_A) && 
      (hp48_cursor_button <= HP48_BUTTON_F)) {
    /* Nothing to do */
  }
}

void
hp48_move_cursor_button_down(void)
{
  if ((hp48_cursor_button >= HP48_BUTTON_ON) && 
      (hp48_cursor_button <= HP48_BUTTON_PLUS)) {
     /* Nothing to do */
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_SHR  ) && 
      (hp48_cursor_button <= HP48_BUTTON_MINUS)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_SHR) + HP48_BUTTON_ON;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_SHL) && 
      (hp48_cursor_button <= HP48_BUTTON_MUL)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_SHL) + HP48_BUTTON_SHR;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_ALPHA) && 
      (hp48_cursor_button <= HP48_BUTTON_DIV)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_ALPHA) + HP48_BUTTON_SHL;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_ENTER) && 
      (hp48_cursor_button <= HP48_BUTTON_BS)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_ENTER) + HP48_BUTTON_ALPHA;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_SIN) && 
      (hp48_cursor_button <= HP48_BUTTON_INV)) {
    hp48_hide_cursor_button();
    if ((hp48_cursor_button == HP48_BUTTON_SIN) || (hp48_cursor_button == HP48_BUTTON_COS)) {
      hp48_cursor_button = HP48_BUTTON_ENTER;
    } else {
      hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_COS) + HP48_BUTTON_ENTER;
    }
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_COLON) && 
      (hp48_cursor_button <= HP48_BUTTON_RIGHT)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_COLON) + HP48_BUTTON_SIN;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_MTH) && 
      (hp48_cursor_button <= HP48_BUTTON_NXT)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_MTH) + HP48_BUTTON_COLON;
  } else
  if ((hp48_cursor_button >= HP48_BUTTON_A) && 
      (hp48_cursor_button <= HP48_BUTTON_F)) {
    hp48_hide_cursor_button();
    hp48_cursor_button = (hp48_cursor_button - HP48_BUTTON_A) + HP48_BUTTON_MTH;
  }
}

/* Cursor */

void
psp_draw_menu_background()
{
  psp_pg_blit_background(psp_menu_background);
}

void
psp_save_screenshot(void)
{
  char TmpFileName[MAX_PATH];
  sprintf(TmpFileName,"%s/scr/screenshot_%d.png", HP48.psp_home_path, HP48.psp_screenshot_id++);
  if (HP48.psp_screenshot_id >= 10) HP48.psp_screenshot_id = 0;
  psp_pg_save_png(TmpFileName);
}


