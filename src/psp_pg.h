// primitive graphics for Hello World PSP
# ifndef _PSP_PG_H_
# define _PSP_PG_H_

//#define PG_RGB(r, g, b) ((r)|((g)<<5)|((b)<<10))
# define psp_pg_rgb(r, g, b)  (((b & 0xf8) << 7) | ((g & 0xf8) << 2) | (r >> 3))

#define  systemRedShift      0
#define  systemGreenShift    5
#define  systemBlueShift    10



 void psp_pg_wait_v();
 void psp_pg_frame(long mode,long frame);
 void psp_pg_flip();
 char *psp_pg_get_vram_addr(unsigned long x,unsigned long y);
 char *psp_pg_get_vramf_addr(unsigned long x,unsigned long y);

 void psp_pg_draw_rectangle(int x, int y, int w, int h, int border, int mode);
 void psp_pg_fill_rectangle(int x, int y, int w, int h, int color, int mode);
 void psp_pg_put_pixel(int x, int y,int color);

 void psp_pg_put_char(int x, int y, int color, int bgcolor, char c, int drawfg, int drawbg);
 void psp_pg_back_put_char(int x, int y, int color, char c);

 void psp_pg_print(int x,int y,const char *str, int color);
 void psp_pg_fill_print(int x,int y,const char *str, int color, int bgcolor);
 void psp_pg_back_char(int x, int y, int color, char c);
 void psp_pg_back_print(int x,int y,const char *str, int color);

 void psp_pg_init();
 u32* psp_pg_load_background_png(const char* filename);
 void psp_pg_blit_background(u32* bitmap);
 void psp_pg_menu_background();
 void psp_pg_clear_screen(unsigned long color);

 void psp_pg_save_png(const char* filename);

#define PG_SCREEN_WIDTH  480
#define PG_SCREEN_HEIGHT 272

#define PG_DEF       0
#define PG_XOR       1

# endif

