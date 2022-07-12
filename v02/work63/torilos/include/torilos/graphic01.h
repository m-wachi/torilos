#ifndef GRAPHIC01_H
#define GRAPHIC01_H

 
// 320x200 8bit color
/*
#define VRAM_ADDRESS      0xa0000
#define SCREEN_X_WIDTH    320
#define SCREEN_Y_WIDTH    200
*/

// VESA 640x480 8bit color
#define VRAM_ADDRESS      0xe0000000
#define SCREEN_X_WIDTH    640
#define SCREEN_Y_WIDTH    480


#define COL8_BLACK        0
#define COL8_RED          1
#define COL8_GREEN        2
#define COL8_YELLOW       3
#define COL8_WHITE        7
#define COL8_DARKGREEN    10
#define COL8_DARKBLUE     12
#define COL8_DARK_SKYBLUE 14

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);

void putfont8(char *vram, int xsize, int x, int y, char color, char *font);
void putfonts8_asc(char*, int, int, int, char, unsigned char*);

void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
				 int pysize, int px0, int py0, char *buf, int bxsize);

void boxfill8(unsigned char *vram, int xsize, unsigned char c, 
			  int x0, int y0, int x1, int y1);

#endif /* GRAPHIC01_H */
