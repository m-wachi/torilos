#include <torilos/graphic01.h>
#include <torilos/myasmfunc01.h>

void set_palette(int start, int end, unsigned char *rgb) {
	int i, eflags;
	eflags = io_load_eflags();	/* load eflags */
	io_cli(); 					/* mask interrupt */
	io_out8(0x03c8, start);

	for (i = start; i < (end+1) * 3; i++) {
		unsigned char temp = rgb[i];
		temp = temp / 4;
		io_out8(0x03c9, temp);
		/* io_out8(0x03c9, rgb[i+1] / 4); 
		   io_out8(0x03c9, rgb[i+2] / 4); */
	}

	io_store_eflags(eflags);	/* restore eflags */
	return;
}

void init_mouse_cursor8(char *mouse, char bc) {
	static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 16 + x] = COL8_BLACK;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 16 + x] = COL8_WHITE;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 16 + x] = bc;
			}
		}
	}
	return;
}

void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize) {
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, 
			  int x0, int y0, int x1, int y1) {
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

void putfonts8_asc(char *vram, int xsize, int x, int y, 
				   char c, unsigned char *s) {
	extern char _hankaku[4096];
	for (; *s != 0x00; s++) {
		putfont8(vram, xsize, x, y, c, _hankaku + *s * 16);
		x += 8;
	}
	return;
}


void putfont8(char *vram, int xsize, int x, int y, char color, char *font) {
	int i;
	char *p, d /* data */;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = color; }
		if ((d & 0x40) != 0) { p[1] = color; }
		if ((d & 0x20) != 0) { p[2] = color; }
		if ((d & 0x10) != 0) { p[3] = color; }
		if ((d & 0x08) != 0) { p[4] = color; }
		if ((d & 0x04) != 0) { p[5] = color; }
		if ((d & 0x02) != 0) { p[6] = color; }
		if ((d & 0x01) != 0) { p[7] = color; }
	}
	return;
}


void init_palette(void) {
	/* 'static char' means assembler DB */
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:black */
		0xff, 0x00, 0x00,	/*  1:bright red */
		0x00, 0xff, 0x00,	/*  2:bright green */
		0xff, 0xff, 0x00,	/*  3:bright yellow */
		0x00, 0x00, 0xff,	/*  4:bright blue */
		0xff, 0x00, 0xff,	/*  5:bright purple */
		0x00, 0xff, 0xff,	/*  6:bright skyblue */
		0xff, 0xff, 0xff,	/*  7:white */
		0xc6, 0xc6, 0xc6,	/*  8:light gray */
		0x84, 0x00, 0x00,	/*  9:dark red */
		0x00, 0x84, 0x00,	/* 10:dark green */
		0x84, 0x84, 0x00,	/* 11:dark yellow */
		0x00, 0x00, 0x84,	/* 12:dark blue */
		0x84, 0x00, 0x84,	/* 13:dark purple */
		0x00, 0x84, 0x84,	/* 14:dark skyblue */
		0x84, 0x84, 0x84	/* 15:dark gray */
	};

	/* unsigned char *p */; 
	
	set_palette(0, 15, table_rgb);
	 
	/*
	p = 0x1180;
	set_palette(0, 15, p);  
	 */

	return;

}


