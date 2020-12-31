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

/* $Log: actions.c,v $
 * Revision 1.15  1995/01/11  18:20:01  ecd
 * major update to support HP48 G/GX
 *
 * Revision 1.14  1994/12/07  20:20:50  ecd
 * changed shutdown again: wake on TIMER1CTRL & XTRA
 *
 * Revision 1.14  1994/12/07  20:20:50  ecd
 * changed shutdown again: wake on TIMER1CTRL & XTRA
 *
 * Revision 1.13  1994/11/28  02:00:51  ecd
 * changed do_configure for internal debugging
 *
 * Revision 1.12  1994/11/02  14:40:38  ecd
 * removed call to debug in do_shutdown()
 *
 * Revision 1.11  1994/10/09  20:29:47  ecd
 * no real change, was just fiddling around with the display.
 *
 * Revision 1.10  1994/10/06  16:30:05  ecd
 * added refresh_display()
 *
 * Revision 1.9  1994/10/05  08:36:44  ecd
 * changed shutdown
 *
 * Revision 1.8  1994/10/01  10:12:53  ecd
 * fixed bug in shutdown
 *
 * Revision 1.7  1994/09/30  12:37:09  ecd
 * changed shutdown instruction
 *
 * Revision 1.6  1994/09/18  22:47:20  ecd
 * fixed bug with overflow in timerdiff
 *
 * Revision 1.5  1994/09/18  15:29:22  ecd
 * added SHUTDN implementation,
 * started Real Time support.
 *
 * Revision 1.4  1994/09/13  16:57:00  ecd
 * changed to plain X11
 *
 * Revision 1.3  1994/08/31  18:23:21  ecd
 * changed memory read routines.
 *
 * Revision 1.2  1994/08/27  11:28:59  ecd
 * changed keyboard interrupt handling.
 *
 * Revision 1.1  1994/08/26  11:09:02  ecd
 * Initial revision
 *
 *
 * $Id: actions.c,v 1.15 1995/01/11 18:20:01 ecd Exp ecd $
 */

# if 0
#define DEBUG_INTERRUPT 1
/* #define DEBUG_RSTK 1 */
#define DEBUG_KBD_INT 1
#define DEBUG_SHUTDOWN 1
#define DEBUG_CONFIG 1
/* #define DEBUG_ID 1 */
# endif

#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "hp48.h"
#include "hp48_emu.h"
#include "device.h"
#include "psp_kbd.h"
#include "timer.h"
#include "romio.h"

static int	interrupt_called = 0;
extern long	nibble_masks[16];

int		conf_bank1 = 0x00000;
int		conf_bank2 = 0x00000;

void
do_in(void)
{
  int i, in, out;

  out = 0;
  for (i = 2; i >= 0; i--) {
    out <<= 4;
    out |= saturn.OUT[i];
  }
  in = 0;
  for (i = 0; i < 9; i++)
    if (out & (1 << i))
      in |= saturn.keybuf.rows[i];
#ifdef DEBUG_INOUT
  fprintf(stdout, "saturn.OUT=%.3x, saturn.IN=%.4x\n", out, in);
#endif
  for (i = 0; i < 4; i++) {
    saturn.IN[i] = in & 0xf;
    in >>= 4;
  }
}

void
clear_program_stat(int n)
{
  saturn.PSTAT[n] = 0;
}

void
set_program_stat(int n)
{
  saturn.PSTAT[n] = 1;
}

int
get_program_stat(int n)
{
  return saturn.PSTAT[n];
}

void
register_to_status(unsigned char *r)
{
  int i;

  for (i = 0; i < 12; i++) {
    saturn.PSTAT[i] = (r[i / 4] >> (i % 4)) & 1;
  }
}

void
status_to_register(unsigned char *r)
{
  int i;

  for (i = 0; i < 12; i++) {
    if (saturn.PSTAT[i]) {
      r[i / 4] |= 1 << (i % 4);
    } else {
      r[i / 4] &= ~(1 << (i % 4)) & 0xf;
    }
  }
}

void
swap_register_status(unsigned char *r)
{
  int i, tmp;

  for (i = 0; i < 12; i++) {
    tmp = saturn.PSTAT[i];
    saturn.PSTAT[i] = (r[i / 4] >> (i % 4)) & 1;
    if (tmp) {
      r[i / 4] |= 1 << (i % 4);
    } else {
      r[i / 4] &= ~(1 << (i % 4)) & 0xf;
    }
  }
}

void
clear_status(void)
{
  int i;

  for (i = 0; i < 12; i++) {
    saturn.PSTAT[i] = 0;
  }
}

void
set_register_nibble(unsigned char *reg, int n, unsigned char val)
{
  reg[n] = val;
}

unsigned char
get_register_nibble(unsigned char *reg, int n)
{
  return reg[n];
}

void
set_register_bit(unsigned char *reg, int n)
{
  reg[n/4] |= (1 << (n%4));
}

void
clear_register_bit(unsigned char *reg, int n)
{
  reg[n/4] &= ~(1 << (n%4));
}

int
get_register_bit(unsigned char *reg, int n)
{
  return ((int)(reg[n/4] & (1 << (n%4))) > 0)?1:0;
}

short conf_tab_sx[] = { 1, 2, 2, 2, 2, 0 };
short conf_tab_gx[] = { 1, 2, 2, 2, 2, 0 };

void
do_reset(void)
{
  int i;

  for (i = 0; i < 6; i++)
    {
      if (opt_gx)
        saturn.mem_cntl[i].unconfigured = conf_tab_gx[i];
      else
        saturn.mem_cntl[i].unconfigured = conf_tab_sx[i];
      saturn.mem_cntl[i].config[0] = 0x0;
      saturn.mem_cntl[i].config[1] = 0x0;
    }

#ifdef DEBUG_CONFIG
  fprintf(stdout, "%.5lx: RESET\n", saturn.PC);
  for (i = 0; i < 6; i++)
    {
      if (saturn.mem_cntl[i].unconfigured)
        fprintf(stdout, "MEMORY CONTROLLER %d is unconfigured\n", i);
      else
        fprintf(stdout, "MEMORY CONTROLLER %d is configured to %.5lx, %.5lx\n",
                i, saturn.mem_cntl[i].config[0], saturn.mem_cntl[i].config[1]);
    }
#endif
}

void
do_inton(void)
{
  saturn.kbd_ien = 1;
}

void
do_intoff(void)
{
  saturn.kbd_ien = 0;
}

void
do_return_interupt(void)
{
  if (saturn.int_pending) {
#ifdef DEBUG_INTERRUPT
    fprintf(stdout, "PC = %.5lx: RTI SERVICE PENDING INTERRUPT\n",
            saturn.PC);
#endif
    saturn.int_pending = 0;
    saturn.intenable = 0;
    saturn.PC = 0xf;
  } else {
#ifdef DEBUG_INTERRUPT
    fprintf(stdout, "PC = %.5lx: RETURN INTERRUPT to ", saturn.PC);
#endif
    saturn.PC = pop_return_addr();
#ifdef DEBUG_INTERRUPT
    fprintf(stdout, "%.5lx\n", saturn.PC);
#endif
    saturn.intenable = 1;

    if (adj_time_pending) {
      schedule_event = 0;
      sched_adjtime = 0;
    }

  }
}

void
do_interupt(void)
{
  interrupt_called = 1;
  if (saturn.intenable) {
#ifdef DEBUG_INTERRUPT
    fprintf(stdout, "PC = %.5lx: INTERRUPT\n", saturn.PC);
#endif
    push_return_addr(saturn.PC);
    saturn.PC = 0xf;
    saturn.intenable = 0;
  }
}

void
do_kbd_int(void)
{
  interrupt_called = 1;
  if (saturn.intenable) {
#ifdef DEBUG_KBD_INT
    fprintf(stdout, "PC = %.5lx: KBD INT\n", saturn.PC);
#endif
    push_return_addr(saturn.PC);
    saturn.PC = 0xf;
    saturn.intenable = 0;
  } else {
#ifdef DEBUG_KBD_INT
    fprintf(stdout, "PC = %.5lx: KBD INT PENDING\n", saturn.PC);
#endif
    saturn.int_pending = 1;
  }
}

void
do_reset_interrupt_system(void)
{
  int i, gen_intr;

  saturn.kbd_ien = 1;
  gen_intr = 0;
  for (i = 0; i < 9; i++) {
    if (saturn.keybuf.rows[i] != 0) {
      gen_intr = 1;
      break;
    }
  }
  if (gen_intr) {
    do_kbd_int();
  }
}

void
do_unconfigure(void)
{
  int i;
  unsigned int conf;

  conf = 0;
  for (i = 4; i >= 0; i--) {
    conf <<= 4;
    conf |= saturn.C[i];
  }

  for (i = 0; i < 6; i++)
    {
      if (saturn.mem_cntl[i].config[0] == conf)
        {
          if (opt_gx)
            saturn.mem_cntl[i].unconfigured = conf_tab_gx[i];
          else
            saturn.mem_cntl[i].unconfigured = conf_tab_sx[i];
          saturn.mem_cntl[i].config[0] = 0x0;
          saturn.mem_cntl[i].config[1] = 0x0;
          break;
        }
    }

#ifdef DEBUG_CONFIG
  fprintf(stdout, "%.5lx: UNCNFG %.5x:\n", saturn.PC, conf);
  for (i = 0; i < 6; i++)
    {
      if (saturn.mem_cntl[i].unconfigured)
        fprintf(stdout, "MEMORY CONTROLLER %d is unconfigured\n", i);
      else
        fprintf(stdout, "MEMORY CONTROLLER %d is configured to %.5lx, %.5lx\n",
                i, saturn.mem_cntl[i].config[0], saturn.mem_cntl[i].config[1]);
    }
#endif
}

void
do_configure(void)
{
  int i;
  unsigned long conf;

  conf = 0;
  for (i = 4; i >= 0; i--) {
    conf <<= 4;
    conf |= saturn.C[i];
  }

  for (i = 0; i < 6; i++)
    {
      if (saturn.mem_cntl[i].unconfigured)
        {
          saturn.mem_cntl[i].unconfigured--;
          saturn.mem_cntl[i].config[saturn.mem_cntl[i].unconfigured] = conf;
          break;
        }
    }

#ifdef DEBUG_CONFIG
  fprintf(stdout, "%.5lx: CONFIG %.5lx:\n", saturn.PC, conf);
  for (i = 0; i < 6; i++)
    {
      if (saturn.mem_cntl[i].unconfigured)
        fprintf(stdout, "MEMORY CONTROLLER %d is unconfigured\n", i);
      else
        fprintf(stdout, "MEMORY CONTROLLER %d at %.5lx, %.5lx\n",
                i, saturn.mem_cntl[i].config[0], saturn.mem_cntl[i].config[1]);
    }
#endif
}

int
get_identification(void)
{
  int i;
  static int chip_id[]
             = { 0, 0, 0, 0, 0x05, 0xf6, 0x07, 0xf8, 0x01, 0xf2, 0, 0 };
  int id;

  for (i = 0; i < 6; i++)
    {
      if (saturn.mem_cntl[i].unconfigured)
        break;
    }
  if (i < 6)
    id = chip_id[2 * i + (2 - saturn.mem_cntl[i].unconfigured)];
  else
    id = 0;
    
#ifdef DEBUG_ID
  fprintf(stdout, "%.5lx: C=ID, returning: %x\n", saturn.PC, id);
  for (i = 0; i < 6; i++)
    {
      if (saturn.mem_cntl[i].unconfigured == 2)
        fprintf(stdout, "MEMORY CONTROLLER %d is unconfigured\n", i);
      else if (saturn.mem_cntl[i].unconfigured == 1)
        {
          if (i == 0)
            fprintf(stdout, "MEMORY CONTROLLER %d unconfigured\n", i);
          else
            fprintf(stdout, "MEMORY CONTROLLER %d configured to ????? %.5lx\n",
                i, saturn.mem_cntl[i].config[1]);
        }
      else
        fprintf(stdout, "MEMORY CONTROLLER %d configured to %.5lx, %.5lx\n",
                i, saturn.mem_cntl[i].config[0], saturn.mem_cntl[i].config[1]);
    }
#endif

  for (i = 0; i < 3; i++)
    {
      saturn.C[i] = id & 0x0f;
      id >>= 4;
    }
  return 0;
}

void
do_shutdown(void)
{
  int wake, alarms;
  t1_t2_ticks ticks;

  if (device.display_touched) {
    device.display_touched = 0;
    update_display();
  }
  
  stop_timer(RUN_TIMER);
  start_timer(IDLE_TIMER);

  if (is_zero_register(saturn.OUT, OUT_FIELD)) {
#ifdef DEBUG_SHUTDOWN
    fprintf(stdout, "%.5lx: SHUTDN: PC = 0\n", saturn.PC);
#endif
    saturn.intenable = 1;
    saturn.int_pending = 0;
  }

#ifdef DEBUG_SHUTDOWN
  fprintf(stdout, "%.5lx:\tSHUTDN: Timer 1 Control = %x, Timer 1 = %d\n",
          saturn.PC, saturn.t1_ctrl, saturn.timer1);
  fprintf(stdout, "%.5lx:\tSHUTDN: Timer 2 Control = %x, Timer 2 = %ld\n",
          saturn.PC, saturn.t2_ctrl, saturn.timer2);
#endif

  wake  = 0;
  alarms = 0;

  do {
    ticks = get_t1_t2();
    if (saturn.t2_ctrl & 0x01) {
      saturn.timer2 = ticks.t2_ticks;
    }
    saturn.timer1 = set_t1 - ticks.t1_ticks;

    hp48_check_load_prog_file();

    interrupt_called = 0;
    if (psp_update_keys()) {
      if (interrupt_called)
        wake = 1;
    }

    if (saturn.timer2 <= 0)
      {
        if (saturn.t2_ctrl & 0x04)
          {
            wake = 1;
          }
        if (saturn.t2_ctrl & 0x02)
          {
            wake = 1;
            saturn.t2_ctrl |= 0x08;
            do_interupt();
          }
      }

    if (saturn.timer1 < 0)
      {
        saturn.timer1 &= 0x0f;
        if (saturn.t1_ctrl & 0x04)
          {
            wake = 1;
          }
        if (saturn.t1_ctrl & 0x03)
          {
            wake = 1;
            saturn.t1_ctrl |= 0x08;
            do_interupt();
          }
      }

    if (wake == 0) {
      interrupt_called = 0;
      receive_char();
      if (interrupt_called)
        wake = 1;
    }

    alarms++;

    
  } while (wake == 0);

  stop_timer(IDLE_TIMER);
  start_timer(RUN_TIMER);
}

void
set_hardware_stat(int op)
{
  if (op & 1) saturn.XM = 1;
  if (op & 2) saturn.SB = 1;
  if (op & 4) saturn.SR = 1;
  if (op & 8) saturn.MP = 1;
}

void
clear_hardware_stat(int op)
{
  if (op & 1) saturn.XM = 0;
  if (op & 2) saturn.SB = 0;
  if (op & 4) saturn.SR = 0;
  if (op & 8) saturn.MP = 0;
}

int
is_zero_hardware_stat(int op)
{
  if (op & 1) if (saturn.XM != 0) return 0;
  if (op & 2) if (saturn.SB != 0) return 0;
  if (op & 4) if (saturn.SR != 0) return 0;
  if (op & 8) if (saturn.MP != 0) return 0;
  return 1;
}

void
push_return_addr(long addr)
{
  int i;

  if (++saturn.rstkp >= NR_RSTK) {
#if 0
    fprintf(stdout, "%.5lx: RSTK overflow !!!\n", saturn.PC);
    for (i = saturn.rstkp - 1; i >= 0; i--) {
      fprintf(stdout, "\tRSTK[%d] %.5lx\n", i, saturn.rstk[i]);
    }
#endif
    for (i = 1; i < NR_RSTK; i++)
      saturn.rstk[i-1] = saturn.rstk[i];
    saturn.rstkp--;
  }
  saturn.rstk[saturn.rstkp] = addr;
#ifdef DEBUG_RSTK
  fprintf(stdout, "PUSH %.5x:\n", addr);
  for (i = saturn.rstkp; i >= 0; i--) {
    fprintf(stdout, "RSTK[%d] %.5x\n", i, saturn.rstk[i]);
  }
#endif
}

long
pop_return_addr(void)
{
#ifdef DEBUG_RSTK
  int i;

  for (i = saturn.rstkp; i >= 0; i--) {
    fprintf(stdout, "RSTK[%d] %.5x\n", i, saturn.rstk[i]);
  }
  fprintf(stdout, "POP %.5x:\n",
          (saturn.rstkp >= 0) ? saturn.rstk[saturn.rstkp]:0);
#endif
  if (saturn.rstkp < 0)
    return 0;
  return saturn.rstk[saturn.rstkp--];
}

char *
make_hexstr(long addr, int n)
{
  static char str[44];
  int i, t, trunc;

  trunc = 0;
  if (n > 40) {
    n = 40;
    trunc = 1;
  }
  for (i = 0; i < n; i++) {
    t = read_nibble(addr+i);
    if (t <= 9)
      str[i] = '0' + t;
    else
      str[i] = 'a' + (t - 10);
  }
  str[n] = '\0';
  if (trunc) {
    str[n] = '.';
    str[n+1] = '.';
    str[n+2] = '.';
    str[n+3] = '\0';
  }
  return str;
}

void
load_constant(unsigned char *reg, int n, long addr)
{
  int i, p;

  p = saturn.P;
  for (i = 0; i < n; i++) {
    reg[p] = read_nibble(addr + i);
    p = (p + 1) & 0xf;
  }
}

void
load_addr(word_20 *dat, long addr, int n)
{
  int i;

  for (i = 0; i < n; i++) {
    *dat &= ~nibble_masks[i];
    *dat |= read_nibble(addr + i) << (i * 4);
  }
}

void
load_address(unsigned char *reg, long addr, int n)
{
  int i;

  for (i = 0; i < n; i++) {
    reg[i] = read_nibble(addr + i);
  }
}

void
register_to_address(unsigned char *reg, word_20 *dat, int s)
{
  int i, n;

  if (s)
    n = 4;
  else 
    n = 5;
  for (i = 0; i < n; i++) {
    *dat &= ~nibble_masks[i];
    *dat |= (reg[i] & 0x0f) << (i * 4);
  }
}

void
address_to_register(word_20 dat, unsigned char *reg, int s)
{
  int i, n;

  if (s)
    n = 4;
  else 
    n = 5;
  for (i = 0; i < n; i++) {
    reg[i] = dat & 0x0f;
    dat >>= 4;
  }
}

long
dat_to_addr(unsigned char *dat)
{
  int i;
  long addr;

  addr = 0;
  for (i = 4; i >= 0; i--) {
    addr <<= 4;
    addr |= (dat[i] & 0xf);
  }
  return addr;
}
 
void
addr_to_dat(long addr, unsigned char *dat)
{
  int i;

  for (i = 0; i < 5; i++) {
    dat[i] = (addr & 0xf);
    addr >>= 4;
  }
}
 
void
add_address(word_20 *dat, int add)
{
  *dat += add;
  if (*dat & (word_20)0xfff00000) {
    saturn.CARRY = 1;
  } else {
    saturn.CARRY = 0;
  }
  *dat &= 0xfffff;
}

static int start_fields[] = {
  -1,  0,  2,  0, 15,  3,  0,  0,
  -1,  0,  2,  0, 15,  3,  0,  0, 
   0,  0,  0
};

static int end_fields[] = {
  -1, -1,  2,  2, 15, 14,  1, 15,
  -1, -1,  2,  2, 15, 14,  1,  4,
   3,  2,  0
};

static inline int
get_start(int code)
{
  int s;

  if ((s = start_fields[code]) == -1) {
    s = saturn.P;
  }
  return s;
}

static inline int
get_end(int code)
{
  int e;

  if ((e = end_fields[code]) == -1) {
    e = saturn.P;
  }
  return e;
}

void
store(word_20 dat, unsigned char *reg, int code)
{
  int i, s, e;

  s = get_start(code);
  e = get_end(code);
  for (i = s; i <= e; i++) {
    write_nibble(dat++, reg[i]);
  }
}

void
store_n(word_20 dat, unsigned char *reg, int n)
{
  int i;

  for (i = 0; i < n; i++) {
    write_nibble(dat++, reg[i]);
  }
}

void
recall(unsigned char *reg, word_20 dat, int code)
{
  int i, s, e;

  s = get_start(code);
  e = get_end(code);
  for (i = s; i <= e; i++) {
    reg[i] = read_nibble_crc(dat++);
  }
}

void
recall_n(unsigned char *reg, word_20 dat, int n)
{
  int i;

  for (i = 0; i < n; i++) {
    reg[i] = read_nibble_crc(dat++);
  }
}

