/*
 *  This file is part of hp48, an emulator of the HP-48sx Calculator.
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

/* $Log: global.h,v $
 * Revision 1.5  1994/12/08  22:28:39  ecd
 * added generic define for SYSV_TIME if SYSV is defined
 *
 * Revision 1.4  1994/12/07  20:16:41  ecd
 * added more functions missing in SunOS includes
 *
 * Revision 1.4  1994/12/07  20:16:41  ecd
 * added more functions missing in SunOS includes
 *
 * Revision 1.3  1994/11/02  14:51:27  ecd
 * new define for SYSV_TIME
 *
 * Revision 1.2  1994/10/06  16:28:03  ecd
 * added #define USE_SHM
 *
 * Revision 1.1  1994/09/07  12:53:20  ecd
 * Initial revision
 *
 *
 * $Id: global.h,v 1.5 1994/12/08 22:28:39 ecd Exp ecd $
 */

#ifndef _GLOBAL_H
#define _GLOBAL_H 1

#include <pspkernel.h>
#include <pspdebug.h>
#include <psppower.h>
#include <pspiofilemgr.h>

  extern char  *progname;
  extern struct saturn_t saturn;
  extern int	    verbose;
  extern int	    quiet;
  extern int     useSerial;
  extern char   *serialLine;
  extern int     initialize;
  extern int     resetOnStartup;
  extern char   *romFileName;

# ifndef MAX_PATH
# define MAX_PATH 128
# endif

   typedef struct HP48_t {

     char          psp_home_path[MAX_PATH];
     char          psp_screenshot_path[MAX_PATH];
     int           psp_screenshot_id;
     unsigned int  psp_cpu_clock;
     unsigned char psp_reverse_analog;
     int           psp_skin_id;
 
   } HP48_t;

  extern HP48_t HP48;
  extern int    psp_screenshot_mode;

  extern int  psp_exit (int);

  extern void hp48_init_resource (int argc, char **argv);
  extern int  hp48_parse_configuration();
  extern int  hp48_save_configuration();

# ifdef DEBUG
  extern void fprintf(stdout,const char *Format, ...);
# endif

#endif
