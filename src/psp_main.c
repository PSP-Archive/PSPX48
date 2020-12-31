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

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <pspctrl.h>

#include "global.h"
#include "hp48.h"
#include "psp_kbd.h"
#include "psp_gr.h"

#define STDOUT_FILE  "stdout.txt"
#define STDERR_FILE  "stderr.txt"

static void cleanup_output(void);

# ifndef PSPFW30X
PSP_MODULE_INFO("PSPX48", 0x1000, 1, 1);
# else
PSP_MODULE_INFO("PSPX48", 0x0, 1, 1);
PSP_HEAP_SIZE_KB(12*1024);
# endif
PSP_MAIN_THREAD_ATTR(0);
PSP_MAIN_THREAD_STACK_SIZE_KB(64);

int 
x48_psp_exit_callback(int arg1, int arg2, void *common)
{
  cleanup_output();
  sceKernelExitGame();
  return 0;
}

int x48_psp_callback_thread(SceSize args, void *argp)
{
  int cbid;
	cbid = sceKernelCreateCallback("Exit Callback", 
				       x48_psp_exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

  sceKernelSleepThreadCB();
  return 0;
}

int 
x48_psp_setup_callbacks(void)
{
  int thid = 0;

  sceCtrlSetSamplingCycle(0);
  sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

  thid = sceKernelCreateThread("update_thread", 
             x48_psp_callback_thread, 0x11, 0xFA0, 0, 0);
  if(thid >= 0)
    sceKernelStartThread(thid, 0, 0);
  return thid;
}

/* Remove the output files if there was no output written */
static void cleanup_output(void)
{
#ifndef NO_STDIO_REDIRECT
  FILE *file;
  int empty;
#endif

  /* Flush the output in case anything is queued */
  fclose(stdout);
  fclose(stderr);

#ifndef NO_STDIO_REDIRECT
  /* See if the files have any output in them */
  file = fopen(STDOUT_FILE, "rb");
  if ( file ) {
    empty = (fgetc(file) == EOF) ? 1 : 0;
    fclose(file);
    if ( empty ) {
      remove(STDOUT_FILE);
    }
  }
  file = fopen(STDERR_FILE, "rb");
  if ( file ) {
    empty = (fgetc(file) == EOF) ? 1 : 0;
    fclose(file);
    if ( empty ) {
      remove(STDERR_FILE);
    }
  }
#endif
}

extern void _fini(void);


int
main(int argc, char **argv)
{
  pspDebugScreenInit();
  /*
   *  init PSP stuff
   */
  x48_psp_setup_callbacks();

  sceCtrlSetSamplingCycle(0);
  sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
  /*
   *  init x48 stuff
   */
  hp48_init_resource(argc,argv);
  /*
   * initialize emulator stuff
   */
  init_emulator();

  /*
   *  Create the HP-48 window
   */
  psp_init_graphic();

  /*
   * can't be done before windows exist
   */
  init_active_stuff();


  /* Emulation loop */
  do {
    emulate();
  } while (1);

  /* Never reached */
  return 0;
}
