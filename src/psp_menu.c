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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pspctrl.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspiofilemgr.h>

#include "global.h"
#include "psp_kbd.h"
#include "psp_gr.h"
#include "psp_fmgr.h"
#include "psp_pg.h"
#include "psp_battery.h"
#include "psp_menu.h"
#include "psp_menu_help.h"

# define MENU_Y_MSG      200
# define MENU_X_MSG      100

static int psp_menu_dirty = 1;

# define MENU_ANALOG       0
# define MENU_CLOCK        1
# define MENU_SCREENSHOT   2

# define MENU_HELP         3

# define MENU_LOAD_PRG     4
# define MENU_SAVE_STATE   5
# define MENU_SAVE_CFG     6

# define MENU_RESET        7
# define MENU_BACK         8
# define MENU_EXIT         9

# define MAX_MENU_ITEM (MENU_EXIT + 1)


  static menu_item_t menu_list[] =
  { 
    "Swap Analog/Cursor :",
    "Clock frequency    :",
    "Save Screenshot    :",

    "Help",

    "Load Program",
    "Save State",
    "Save Config",

    "Reset HP-48",
    "Back to HP-48",
    "Exit",
  };

  static int cur_menu_id = MENU_LOAD_PRG;

  static int psp_reverse_analog    = 0;
  static int psp_cpu_clock         = 222;

void
string_fill_with_space(char *buffer, int size)
{
  int length = strlen(buffer);
  int index;

  for (index = length; index < size; index++) {
    buffer[index] = ' ';
  }
  buffer[size] = 0;
}

void
psp_display_screen_menu_battery(void)
{
  char buffer[64];

  int Length;
  int color;

  snprintf(buffer, 50, " [%s] ", psp_get_battery_string());
  Length = strlen(buffer);

  if (psp_is_low_battery()) color = PSP_MENU_RED_COLOR;
  else                      color = PSP_MENU_GREEN_COLOR;

  psp_pg_back_print(240 - ((8*Length) / 2),10, buffer, color);
}

static void 
psp_display_screen_menu(void)
{
  char buffer[64];
  int menu_id = 0;
  int color   = 0;
  int x       = 0;
  int y       = 0;
  int y_step  = 0;

  if (psp_menu_dirty) {

    psp_draw_menu_background();
    psp_menu_dirty = 0;

# if 0
    psp_pg_draw_rectangle(10,10,459,249,PSP_MENU_BORDER_COLOR,0);
    psp_pg_draw_rectangle(11,11,457,247,PSP_MENU_BORDER_COLOR,0);
# endif

    psp_pg_back_print( 30,  10, " Start+L+R: EXIT ", PSP_MENU_WARNING_COLOR);

    psp_display_screen_menu_battery();

    psp_pg_back_print( 310, 10, " RTrigger: RESET ", PSP_MENU_NOTE_COLOR);

    psp_pg_back_print(30,  254, " []: Cancel  O/X: Valid  Select: Back ", 
                       PSP_MENU_BORDER_COLOR);

# if 0
    psp_pg_back_print(370, 254, " By Zx-81 ",
                       PSP_MENU_AUTHOR_COLOR);
# endif
  }
  
  x      = 40;
  y      = 60;
  y_step = 10;
  
  for (menu_id = 0; menu_id < MAX_MENU_ITEM; menu_id++) {
    color = PSP_MENU_TEXT_COLOR;
    if (cur_menu_id == menu_id) color = PSP_MENU_SEL_COLOR;
    else
    if (menu_id == MENU_EXIT) color = PSP_MENU_WARNING_COLOR;
    else
    if (menu_id == MENU_HELP) color = PSP_MENU_GREEN_COLOR;

    psp_pg_back_print(x, y, menu_list[menu_id].title, color);

    if (menu_id == MENU_ANALOG) {
      if (psp_reverse_analog) strcpy(buffer,"yes");
      else                    strcpy(buffer,"no ");
      psp_pg_back_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_CLOCK) {
      sprintf(buffer,"%d", psp_cpu_clock);
      psp_pg_back_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SCREENSHOT) {
      sprintf(buffer,"%d", HP48.psp_screenshot_id);
      psp_pg_back_print(190, y, buffer, color);
      y += y_step;
    }
    if (menu_id == MENU_BACK) {
      y += y_step;
    } else
    if (menu_id == MENU_SAVE_CFG) {
      y += y_step;
    }
    if (menu_id == MENU_HELP) {
      y += y_step;
    }

    y += y_step;
  }
}

static void
psp_main_menu_reset(void)
{
  /* Reset ! */
  psp_menu_dirty = 1;
  psp_display_screen_menu();
  psp_pg_back_print(MENU_X_MSG, MENU_Y_MSG, "RESET HP48 !", PSP_MENU_WARNING_COLOR);
  psp_menu_dirty = 1;
  hp48_hardware_reset();
  sleep(1);
}

#define MAX_CLOCK_VALUES 4
static int clock_values[MAX_CLOCK_VALUES] = { 222, 266, 300, 333 };

static void
psp_main_menu_clock(int step)
{
  int index;
  for (index = 0; index < MAX_CLOCK_VALUES; index++) {
    if (psp_cpu_clock == clock_values[index]) break;
  }
  if (step > 0) {
    index++;
    if (index >= MAX_CLOCK_VALUES) index = 0;
    psp_cpu_clock = clock_values[index];

  } else {
    index--;

    if (index < 0) index = MAX_CLOCK_VALUES - 1;
    psp_cpu_clock = clock_values[index];
  }
}

static void
psp_main_menu_load()
{
  int ret;

  ret = psp_fmgr_menu(FMGR_FORMAT_48);

  if (ret ==  1) /* load OK */
  {
    psp_menu_dirty = 1;
    psp_display_screen_menu();
    psp_pg_back_print(MENU_X_MSG, MENU_Y_MSG, "File loaded !", PSP_MENU_NOTE_COLOR);
    psp_menu_dirty = 1;
    sleep(1);
  }
  else 
  if (ret == -1) /* Load Error */
  {
    psp_menu_dirty = 1;
    psp_display_screen_menu();
    psp_pg_back_print(MENU_X_MSG, MENU_Y_MSG, "Can't load file !", PSP_MENU_WARNING_COLOR);
    psp_menu_dirty = 1;
    sleep(1);
  }
}

static void
psp_main_menu_screenshot(void)
{
  psp_screenshot_mode = 10;
}

static void
psp_main_menu_validate()
{
  /* Validate */
  HP48.psp_cpu_clock      = psp_cpu_clock;
  HP48.psp_reverse_analog = psp_reverse_analog;

  scePowerSetClockFrequency(HP48.psp_cpu_clock, HP48.psp_cpu_clock, HP48.psp_cpu_clock/2);
}

static void
psp_main_menu_save_config()
{
  int error;

  psp_main_menu_validate();

  error = hp48_save_configuration();

  if (! error) /* save OK */
  {
    psp_menu_dirty = 1;
    psp_display_screen_menu();
    psp_pg_back_print(MENU_X_MSG, MENU_Y_MSG, "File saved !", PSP_MENU_NOTE_COLOR);
    psp_menu_dirty = 1;
    sleep(1);
  }
  else 
  {
    psp_menu_dirty = 1;
    psp_display_screen_menu();
    psp_pg_back_print(MENU_X_MSG, MENU_Y_MSG, "Can't save file !", PSP_MENU_WARNING_COLOR);
    psp_menu_dirty = 1;
    sleep(1);
  }
}

static void
psp_main_menu_save_state()
{
  int error;

  psp_main_menu_validate();

  psp_menu_dirty = 1;
  psp_display_screen_menu();
  psp_pg_back_print(MENU_X_MSG, MENU_Y_MSG, "Please wait ...", PSP_MENU_NOTE_COLOR);
  psp_menu_dirty = 1;

  error = ! hp48_save_emulator_stuff();

  if (! error) /* save OK */
  {
    psp_display_screen_menu();
    psp_pg_back_print(MENU_X_MSG, MENU_Y_MSG, "File saved !", PSP_MENU_NOTE_COLOR);
    psp_menu_dirty = 1;
    sleep(1);
  }
  else 
  {
    psp_display_screen_menu();
    psp_pg_back_print(MENU_X_MSG, MENU_Y_MSG, "Can't save file !", PSP_MENU_WARNING_COLOR);
    psp_menu_dirty = 1;
    sleep(1);
  }
}

int
psp_main_menu_exit(void)
{
  SceCtrlData c;
 
  psp_menu_dirty = 1;
  psp_display_screen_menu();
  psp_pg_back_print(MENU_X_MSG, MENU_Y_MSG, "press X to confirm !", PSP_MENU_WARNING_COLOR);
  psp_menu_dirty = 1;

  psp_kbd_wait_no_button();

  do
  {
    sceCtrlReadBufferPositive(&c, 1);
    c.Buttons &= PSP_ALL_BUTTON_MASK;

    if (c.Buttons & PSP_CTRL_CROSS) { 
      psp_exit(0);
    }

  } while (c.Buttons == 0);

  psp_kbd_wait_no_button();

  return 0;
}


int 
psp_main_menu(void)
{
  SceCtrlData c;
  long        new_pad;
  long        old_pad;
  int         last_time;
  int         end_menu;

  psp_kbd_wait_no_button();

  psp_pg_flip();

  old_pad   = 0;
  last_time = 0;
  end_menu  = 0;

  psp_menu_dirty = 1;

  psp_cpu_clock      = HP48.psp_cpu_clock;
  psp_reverse_analog = HP48.psp_reverse_analog;

  while (! end_menu)
  {
    psp_display_screen_menu();

    while (1)
    {
      sceCtrlReadBufferPositive(&c, 1);
      c.Buttons &= PSP_ALL_BUTTON_MASK;

      if (c.Buttons) break;
    }

    new_pad = c.Buttons;

    if ((old_pad != new_pad) || ((c.TimeStamp - last_time) > PSP_MENU_MIN_TIME)) {
      last_time = c.TimeStamp;
      old_pad = new_pad;

    } else continue;

    if ((c.Buttons & (PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER|PSP_CTRL_START)) ==
        (PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER|PSP_CTRL_START)) {
      /* Exit ! */
      psp_exit(0);
    } else
    if ((c.Buttons & PSP_CTRL_RTRIGGER) == PSP_CTRL_RTRIGGER) {
      psp_main_menu_reset();
      end_menu = 1;
    } else
    if ((new_pad & PSP_CTRL_CROSS ) || 
        (new_pad & PSP_CTRL_CIRCLE) || 
        (new_pad & PSP_CTRL_RIGHT ) ||
        (new_pad & PSP_CTRL_LEFT  )) 
    {
      int step;

      if (new_pad & PSP_CTRL_LEFT)  step = -1;
      else 
      if (new_pad & PSP_CTRL_RIGHT) step =  1;
      else                          step =  0;

      switch (cur_menu_id ) 
      {
        case MENU_ANALOG    : psp_reverse_analog = ! psp_reverse_analog;
        break;              
        case MENU_CLOCK     : psp_main_menu_clock( step );
        break;

        case MENU_LOAD_PRG  : psp_menu_dirty = 1;
                              psp_main_menu_load();
                              old_pad = new_pad = 0;
                              psp_menu_dirty = 1;
        break;              
        case MENU_SCREENSHOT : psp_main_menu_screenshot();
                               end_menu = 1;
        break;              

        case MENU_SAVE_CFG   : psp_main_menu_save_config();
        break;

        case MENU_SAVE_STATE : psp_main_menu_save_state();
        break;
        case MENU_RESET      : psp_main_menu_reset();
                               end_menu = 1;
        break;

        case MENU_BACK      : end_menu = 1;
        break;

        case MENU_EXIT      : psp_main_menu_exit();
        break;

        case MENU_HELP      : psp_menu_dirty = 1;
                              psp_help_menu(0);
                              old_pad = new_pad = 0;
                              psp_menu_dirty = 1;
        break;              
      }

    } else
    if(new_pad & PSP_CTRL_UP) {

      if (cur_menu_id > 0) cur_menu_id--;
      else                 cur_menu_id = MAX_MENU_ITEM-1;

    } else
    if(new_pad & PSP_CTRL_DOWN) {

      if (cur_menu_id < (MAX_MENU_ITEM-1)) cur_menu_id++;
      else                                 cur_menu_id = 0;

    } else  
    if(new_pad & PSP_CTRL_SQUARE) {
      /* Cancel */
      end_menu = -1;
    } else 
    if((new_pad & PSP_CTRL_SELECT) == PSP_CTRL_SELECT) {
      /* Back to HP48 */
      end_menu = 1;
    }
  }
 
  psp_kbd_wait_no_button();

  psp_pg_clear_screen( PSP_MENU_BLACK_COLOR );
  psp_pg_flip();

  if (end_menu > 0) {
    psp_main_menu_validate();
  }

  return 1;
}

