// primitive graphics for Hello World PSP

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>
#include <png.h>

#include "global.h"

#include <pspdisplay.h>
#include "psp_pg.h"

 extern u32* psp_menu_background;
 extern unsigned char psp_font[];

//constants
#define    PIXELSIZE  1        //in short
#define    LINESIZE  512        //in short
#define    FRAMESIZE  0x44000      //in byte

//480*272 = 60*38
#define CMAX_X 60
#define CMAX_Y 38

//variables
char *pg_vramtop=(char *)0x04000000;
long pg_screenmode;
long pg_showframe;
long pg_drawframe = 0;

void 
psp_pg_wait_v()
{
  sceDisplayWaitVblankStart();
}

char *
psp_pg_get_vram_addr(unsigned long x,unsigned long y)
{
  return pg_vramtop+(pg_drawframe?FRAMESIZE:0)+x*PIXELSIZE*2+y*LINESIZE*2+0x40000000;
}

char *
psp_pg_get_vramf_addr(unsigned long x,unsigned long y)
{
  char *pg_vramf = (char *)psp_menu_background;
  return pg_vramf+x*PIXELSIZE*2+y*LINESIZE*2;
}

static void 
user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
	// ignore PNG warnings
}


u32* 
psp_pg_load_background_png(const char* BackgroundFileName)
{
  char filename[128];

	u32* vram32;
	u16* vram16;
	int bufferwidth;
	int pixelformat;
	int unknown;
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type, x, y;
	u32* line;
	FILE *fp;

  strcpy(filename, HP48.psp_home_path);
  strcat(filename,"/");
  strcat(filename, BackgroundFileName);

	if ((fp = fopen(filename, "rb")) == NULL) return NULL;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		return NULL;
	}
	png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, user_warning_fn);
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	line = (u32*) malloc(width * 4);
	if (!line) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	sceDisplayWaitVblankStart();  // if framebuf was set with PSP_DISPLAY_SETBUF_NEXTFRAME, wait until it is changed
	sceDisplayGetFrameBuf((void**)&vram32, &bufferwidth, &pixelformat, &unknown);

  vram32 = (u32 *)malloc(bufferwidth * 272 * sizeof(short));
  memset(vram32, 0, bufferwidth * 272 * sizeof(short));
	vram16 = (u16*) vram32;
	for (y = 0; y < height; y++) {
		png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
		for (x = 0; x < width; x++) {
			u32 color32 = line[x];
			u16 color16;
			int r = color32 & 0xff;
			int g = (color32 >> 8) & 0xff;
			int b = (color32 >> 16) & 0xff;
			switch (pixelformat) {
				case PSP_DISPLAY_PIXEL_FORMAT_565:
					color16 = (r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11);
					vram16[x + y * bufferwidth] = color16;
					break;
				case PSP_DISPLAY_PIXEL_FORMAT_5551:
					color16 = (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10);
					vram16[x + y * bufferwidth] = color16;
					break;
				case PSP_DISPLAY_PIXEL_FORMAT_4444:
					color16 = (r >> 4) | ((g >> 4) << 4) | ((b >> 4) << 8);
					vram16[x + y * bufferwidth] = color16;
					break;
				case PSP_DISPLAY_PIXEL_FORMAT_8888:
					color32 = r | (g << 8) | (b << 16);
					vram32[x + y * bufferwidth] = color32;
					break;
			}
		}
	}
	free(line);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	fclose(fp);

  return vram32;
}


void
psp_pg_save_png(const char* filename)
{
  int w = 480;
  int h = 272;
  u8* pix = (u8*)psp_pg_get_vram_addr(0,0);
  u8 writeBuffer[512 * 3];

  FILE *fp = fopen(filename,"wb");

  if(!fp) return;
  
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                NULL,
                                                NULL,
                                                NULL);
  if(!png_ptr) {
    fclose(fp);
    return;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);

  if(!info_ptr) {
    png_destroy_write_struct(&png_ptr,NULL);
    fclose(fp);
    return;
  }

  png_init_io(png_ptr,fp);

  png_set_IHDR(png_ptr,
               info_ptr,
               w,
               h,
               8,
               PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png_ptr,info_ptr);

  u8 *b = writeBuffer;

  int sizeX = w;
  int sizeY = h;
  int y;
  int x;

  u16 *p = (u16 *)pix;
  for(y = 0; y < sizeY; y++) {
     for(x = 0; x < sizeX; x++) {
      u16 v = p[x];
       
      *b++ = ((v >> systemRedShift  ) & 0x001f) << 3; // R
      *b++ = ((v >> systemGreenShift) & 0x001f) << 3; // G 
      *b++ = ((v >> systemBlueShift ) & 0x001f) << 3; // B
    }
    p += 512;
    png_write_row(png_ptr,writeBuffer);
     
    b = writeBuffer;
  }

  png_write_end(png_ptr, info_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);
}

void
psp_pg_blit_background(u32* bitmap)
{
  u32* vram32;
  if (bitmap) {
    vram32 = (u32 *)psp_pg_get_vram_addr(0,0);
    memcpy(vram32, bitmap, 512 * 272 * sizeof(short));
  }
}

void 
psp_pg_init()
{
  sceDisplaySetMode(0,PG_SCREEN_WIDTH,PG_SCREEN_HEIGHT);
  psp_pg_frame(1,1);
}

void 
psp_pg_clear_screen(unsigned long color) 
{
  unsigned char *vptr0;    //pointer to vram
  unsigned long i;

  vptr0=(unsigned char *)psp_pg_get_vram_addr(0,0);
  for (i=0; i<FRAMESIZE/2; i++) {
    *(unsigned short *)vptr0=color;
    vptr0+=PIXELSIZE*2;
  }
}

void 
psp_pg_frame(long mode,long frame)
{
  pg_screenmode=mode;
  frame=(frame?1:0);
  pg_showframe=frame;
  if (mode==0) {
    //screen off
    pg_drawframe=frame;
    sceDisplaySetFrameBuf(0,0,0,1);
  } else if (mode==1) {
    //show/draw same
    pg_drawframe=frame;
    sceDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,1);
  } else if (mode==2) {
    //show/draw different
    pg_drawframe=(frame?0:1);
    sceDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,1);
  }
}

void 
psp_pg_flip()
{
  pg_showframe=(pg_showframe?0:1);
  pg_drawframe=(pg_drawframe?0:1);
  sceDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,0);
}


void 
psp_pg_draw_rectangle(int x, int y, int w, int h, int border, int mode) 
{
  unsigned short *vram = (unsigned short*)psp_pg_get_vram_addr(x, y);
  int xo, yo;
  if (mode == PG_XOR) {
    for (xo = 0; xo < w; xo++) {
      vram[xo] ^=  border;
      vram[xo + h * LINESIZE] ^=  border;
    }
    for (yo = 0; yo < h; yo++) {
      vram[yo * LINESIZE] ^=  border;
      vram[w + yo * LINESIZE] ^=  border;
    }
  } else {
    for (xo = 0; xo < w; xo++) {
      vram[xo] =  border;
      vram[xo + h * LINESIZE] =  border;
    }
    for (yo = 0; yo < h; yo++) {
      vram[yo * LINESIZE] =  border;
      vram[w + yo * LINESIZE] =  border;
    }
  }
}

void 
psp_pg_fill_rectangle(int x, int y, int w, int h, int color, int mode)
{
  unsigned short *vram = (unsigned short*)psp_pg_get_vram_addr(x, y);
  int xo, yo;
  if (mode == PG_XOR) {
    for (xo = 0; xo <= w; xo++) {
      for (yo = 0; yo <= h; yo++) {
        if ( ((xo == 0) && ((yo == 0) || (yo == h))) ||
             ((xo == w) && ((yo == 0) || (yo == h))) ) {
          /* Skip corner */
        } else {
          vram[xo + yo * LINESIZE] ^=  color;
        }
      }
    }
  } else {
    for (xo = 0; xo <= w; xo++) {
      for (yo = 0; yo <= h; yo++) {
        vram[xo + yo * LINESIZE] =  color;
      }
    }
  }
}

void 
psp_pg_back_rectangle(int x, int y, int w, int h)
{
  unsigned short *vram  = (unsigned short*)psp_pg_get_vram_addr(x, y);
  unsigned short *vramf = (unsigned short*)psp_pg_get_vramf_addr(x,y);
  int xo, yo;
  for (xo = 0; xo <= w; xo++) {
    for (yo = 0; yo <= h; yo++) {
      vram[xo + yo * LINESIZE] = vramf[xo + yo * LINESIZE];
    }
  }
}

void 
psp_pg_put_pixel(int x, int y, int color) 
{
  unsigned short *vram = (unsigned short*)psp_pg_get_vram_addr(x, y);
  vram[0] = color;
}


void 
psp_pg_put_char(int x, int y, int color, int bgcolor, char c, int drawfg, int drawbg)
{
  int cx;
  int cy;
  int b;
  int index;
  unsigned short *vram;

  vram  = (unsigned short*)psp_pg_get_vram_addr(x, y);
  index = ((unsigned int)c) * 8;

  for (cy=0; cy<8; cy++) {
    b=0x80;
    for (cx=0; cx<8; cx++) {
      if (psp_font[index] & b) {
        if (drawfg) vram[cx + cy * LINESIZE] = color;
      } else {
        if (drawbg) vram[cx + cy * LINESIZE] = bgcolor;
      }
      b = b >> 1;
    }
    index++;
  }
}

void 
psp_pg_back_put_char(int x, int y, int color, char c)
{
  int cx;
  int cy;
  int b;
  int index;
  unsigned short *vram;
  unsigned short *vramf;

  vram  = (unsigned short*)psp_pg_get_vram_addr(x, y);
  vramf = (unsigned short*)psp_pg_get_vramf_addr(x,y);

  index = ((unsigned int)c) * 8;

  for (cy=0; cy<8; cy++) {
    b=0x80;
    for (cx=0; cx<8; cx++) {
      if (psp_font[index] & b) {
        vram[cx + cy * LINESIZE] = color;
      } else {
        vram[cx + cy * LINESIZE] = vramf[cx + cy * LINESIZE];
      }
      b = b >> 1;
    }
    index++;
  }
}

void 
psp_pg_print(int x,int y,const char *str, int color)
{
  int index;
  int x0 = x;

  for (index = 0; str[index] != '\0'; index++) {
    psp_pg_put_char(x, y, color, 0, str[index], 1, 0);
    x += 8;
    if (x >= (PG_SCREEN_WIDTH - 8)) {
      x = x0; y++;
    }
    if (y >= (PG_SCREEN_HEIGHT - 8)) break;
  }
}

void 
psp_pg_back_print(int x,int y,const char *str, int color)
{
  int index;
  int x0 = x;

  for (index = 0; str[index] != '\0'; index++) {
    psp_pg_back_put_char(x, y, color, str[index]);
    x += 8;
    if (x >= (PG_SCREEN_WIDTH - 8)) {
      x = x0; y++;
    }
    if (y >= (PG_SCREEN_HEIGHT - 8)) break;
  }
}

void 
psp_pg_fill_print(int x,int y,const char *str, int color, int bgcolor)
{
  int index;
  int x0 = x;

  for (index = 0; str[index] != '\0'; index++) {
    psp_pg_put_char(x, y, color, bgcolor, str[index], 1, 1);
    x += 8;
    if (x >= (PG_SCREEN_WIDTH - 8)) {
      x = x0; y++;
    }
    if (y >= (PG_SCREEN_HEIGHT - 8)) break;
  }
}

