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

# ifndef _KBD_H_
# define _KBD_H_

# define KBD_UP           0
# define KBD_RIGHT        1
# define KBD_DOWN         2
# define KBD_LEFT         3
# define KBD_TRIANGLE     4
# define KBD_CIRCLE       5
# define KBD_CROSS        6
# define KBD_SQUARE       7
# define KBD_SELECT       8
# define KBD_START        9
# define KBD_HOME        10
# define KBD_HOLD        11
# define KBD_LTRIGGER    12
# define KBD_RTRIGGER    13

# define KBD_MAX_BUTTONS 14

# define KBD_JOY_UP      14
# define KBD_JOY_RIGHT   15
# define KBD_JOY_DOWN    16
# define KBD_JOY_LEFT    17

# define KBD_ALL_BUTTONS 18

# define PSP_ALL_BUTTON_MASK 0xFFFF

#define HP48_BUTTON_A        0
#define HP48_BUTTON_B        1
#define HP48_BUTTON_C        2
#define HP48_BUTTON_D        3
#define HP48_BUTTON_E        4
#define HP48_BUTTON_F        5

#define HP48_BUTTON_MTH        6
#define HP48_BUTTON_PRG        7
#define HP48_BUTTON_CST        8
#define HP48_BUTTON_VAR        9
#define HP48_BUTTON_UP        10
#define HP48_BUTTON_NXT        11

#define HP48_BUTTON_COLON        12
#define HP48_BUTTON_STO        13
#define HP48_BUTTON_EVAL        14
#define HP48_BUTTON_LEFT        15
#define HP48_BUTTON_DOWN        16
#define HP48_BUTTON_RIGHT        17

#define HP48_BUTTON_SIN        18
#define HP48_BUTTON_COS        19
#define HP48_BUTTON_TAN        20
#define HP48_BUTTON_SQRT        21
#define HP48_BUTTON_POWER        22
#define HP48_BUTTON_INV        23

#define HP48_BUTTON_ENTER        24
#define HP48_BUTTON_NEG        25
#define HP48_BUTTON_EEX        26
#define HP48_BUTTON_DEL        27
#define HP48_BUTTON_BS        28

#define HP48_BUTTON_ALPHA        29
#define HP48_BUTTON_7        30
#define HP48_BUTTON_8        31
#define HP48_BUTTON_9        32
#define HP48_BUTTON_DIV        33

#define HP48_BUTTON_SHL        34
#define HP48_BUTTON_4        35
#define HP48_BUTTON_5        36
#define HP48_BUTTON_6        37
#define HP48_BUTTON_MUL        38

#define HP48_BUTTON_SHR        39
#define HP48_BUTTON_1        40
#define HP48_BUTTON_2        41
#define HP48_BUTTON_3        42
#define HP48_BUTTON_MINUS        43

#define HP48_BUTTON_ON        44
#define HP48_BUTTON_0        45
#define HP48_BUTTON_PERIOD        46
#define HP48_BUTTON_SPC        47
#define HP48_BUTTON_PLUS        48

#define HP48_LAST_BUTTON        48


  typedef struct hp48_button_t {
    char                *name;
    short                pressed;
    short                key_down;
    int                  code;
    int                  x, y;
    unsigned int         w, h;
  } hp48_button_t;

  extern int  psp_update_keys(void);
  extern void kbd_wait_start(void);
  extern void psp_init_keyboard(void);
  extern void psp_kbd_wait_no_button(void);


# endif
