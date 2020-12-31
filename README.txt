
    Welcome to PSPX48

Original Authors of X48

  Eddie C. Dost (edc@dressler.de)
  Iñigo Serna Robledo (imaseroi@bi.ehu.es)

Author of the PSP port version 

  Ludovic.Jacomme (ludovic.jacomme@gmail.com) also known as Zx-81

1. INTRODUCTION
   ------------

  This is a port on PSP and enhancement of *THE* X48 HP48 GX 
  emulator for X Window Systems.

  This package is under GPL Copyright, read COPYING file for
  more information about it.


2. INSTALLATION
   ------------

  Unzip the zip file, and copy the content of the directory fw1.5
  on the psp/game directory (or psp/game150 for custom firmware).
  Get the rom file of your HP48 and save it as 
  psp/game/__SCE__pspx48/rom for 1.5 FW,
  or psp/game150/__SCE__pspx48/rom for custom firmware 3.x-OE.

  Unzip the zip file, and copy the content of the directory fw1.5
  on the psp/game directory (or psp/game150 for custom firmware).
  If you use the fw3.x version then unzip it under the psp/gamexxx
  folder of your psp (where xxx is the version number of your firmware
  for example 340 for 3.40-OE).

  Get the rom file of your HP48GX and save it as psp/game/pspx48/rom file.
  Get the rom file of your HP48GX and save it as psp/game/pspx48/rom 

  One rom file is provided but you have to own a HP48 to have the right
  to use it ;).

  (I use this version http://www.hpcalc.org/hp48/pc/emulators/gxrom-r.zip)

  Many HP48 stuff can be found on http://www.hpcalc.org/ and  
  http://transnull.com/wp/hp48/.

  For any comments or questions on this version, please visit 
  http://zx81.zx81.free.fr or http://zx81.dcemu.co.uk 

3. CONTROL
   ------------

  Inside the main emulator window :

   PSP        HP48
   
   Circle     Up
   Square     Down
   Triangle   Left 
   Cross      Right
   Right      Alpha
   Up         Valid Key on cursor
   Down       ENTER
   Left       Backspace
   L1         Shift R
   R1         Shift L
   Analog     Move Cursor on keyboard
   Start      ON
   Select     Next (usefull for menus)

   Press Start  + L + R   to exit and return to eloader.
   Press Start  + L       to display help
   Press Select + L       to enter in the emulator menu


4. LOADING HP48 APPLICATION FILES
   ------------

  I suggest you first, to read the manual of the HP48, because loading
  applications on this calculator is very complex !

  Usefull informations on how to install application for the HP48 can be
  found here : http://www.hpcalc.org/install.php

  You can save any HP48 files (with .lib or .48 file extensions) on your 
  PSP memory stick. Then, while inside the PSPX48 emulator, just press
  simultaneously SELECT+L to enter in the emulator menu, and then using 
  the file selector choose one application to load in the RAM of 
  your emulator.

  Back to the main emulator screen, wait few seconds, and press the Start 
  button of your PSP.

  + Library installation:

     If you have loaded a library file, you should see a line such as :

          Library 1262: ....

     To install it, you have then to press 0 then ENTER, then STO.
     After you have to switch OFF the HP48 (using Left Shift + ON), and
     switch it ON.
     If you go then to the library menu (Left Shift + 2), you should see
     now your library.

  + Program installation: 

     If you have loaded a program file, you should see :

       1: DIR
          ..... << .... >> 

     To install it, you have then to enter a variable name such as 'A' + ENTER
     then STO.  You can go to the variable menu (using VAR key) and then launch
     your program.

  You can find many applications and libraries on http://www.hpcalc.org/ 

5. COMPILATION
   ------------

  It has been developped under Linux using gcc with PSPSDK. 
  To rebuild the homebrew run the Makefile in the src folder.


