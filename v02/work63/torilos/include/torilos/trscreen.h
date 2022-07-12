#ifndef TRSCREEN_H
#define TRSCREEN_H

/* screen handle */
typedef struct my_screen_handle {
	char* vram_base;
	int width;
	int height;
	char fg_color;
	char bg_color;
	int line_number;
} HND_SC;

HND_SC *init_screen(HND_SC* hnd_sc);
void draw_init_mcursor(char* mcursor, HND_SC *hnd_sc);

#endif
