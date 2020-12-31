
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

#include "hp48.h"
#include "global.h"

  HP48_t HP48;
  int   psp_screenshot_mode = 0;

  /* hp48 stuff */
  char  *progname;
  saturn_t saturn;
  int	    verbose;
  int	    quiet;
  int     useSerial;
  char   *serialLine;
  int     initialize;
  int     resetOnStartup;
  char   *romFileName;


int
hp48_save_configuration(void)
{
  char  chFileName[MAX_PATH + 1];
  FILE* FileDesc;
  int   error = 0;

  strncpy(chFileName, HP48.psp_home_path, sizeof(chFileName)-10);
  strcat(chFileName, "/pspx48.cfg");

  FileDesc = fopen(chFileName, "w");
  if (FileDesc != (FILE *)0 ) {

    fprintf(FileDesc, "psp_cpu_clock=%d\n"      , HP48.psp_cpu_clock);
    fprintf(FileDesc, "psp_reverse_analog=%d\n" , HP48.psp_reverse_analog);

    fclose(FileDesc);

  } else {
    error = 1;
  }

  return error;
}

int
hp48_parse_configuration(void)
{
  char  chFileName[MAX_PATH + 1];
  char  Buffer[512];
  char *Scan;
  unsigned int Value;
  FILE* FileDesc;

  strncpy(chFileName, HP48.psp_home_path, sizeof(chFileName)-10);
  strcat(chFileName, "/pspx48.cfg");

  FileDesc = fopen(chFileName, "r");
  if (FileDesc == (FILE *)0 ) return 0;

  while (fgets(Buffer,512, FileDesc) != (char *)0) {

    Scan = strchr(Buffer,'\n');
    if (Scan) *Scan = '\0';
    /* For this #@$% of windows ! */
    Scan = strchr(Buffer,'\r');
    if (Scan) *Scan = '\0';
    if (Buffer[0] == '#') continue;

    Scan = strchr(Buffer,'=');
    if (! Scan) continue;

    *Scan = '\0';
    Value = atoi(Scan+1);

    if (!strcasecmp(Buffer,"psp_cpu_clock"))      HP48.psp_cpu_clock = Value;
    else
    if (!strcasecmp(Buffer,"psp_reverse_analog")) HP48.psp_reverse_analog = Value;
  }

  fclose(FileDesc);

  return 0;
}


void
hp48_init_resource(int argc, char **argv)
{
  verbose     = 0;
  quiet       = 0;
  useSerial   = 0;
  serialLine = "/dev/tty";
  initialize = 0;
  resetOnStartup = 1;
  romFileName =   "rom";

  getcwd(HP48.psp_home_path, sizeof(HP48.psp_home_path));
  strcpy(HP48.psp_screenshot_path, HP48.psp_home_path);
  strcat(HP48.psp_screenshot_path, "/scr");

  HP48.psp_screenshot_id  = 0;
  HP48.psp_cpu_clock      = 222;
  HP48.psp_reverse_analog = 0;

  hp48_parse_configuration();

  scePowerSetClockFrequency(HP48.psp_cpu_clock, HP48.psp_cpu_clock, HP48.psp_cpu_clock/2);
}

int
psp_exit(int status)
{
  sceKernelExitGame();
  return 0;
}
