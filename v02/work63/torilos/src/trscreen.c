#include <torilos/trscreen.h>
#include <torilos/graphic01.h>

HND_SC *init_screen(HND_SC *hnd_sc) {

	char *vramaddr;
	int i;
	
	hnd_sc->vram_base = (char*)VRAM_ADDRESS;
	hnd_sc->width = SCREEN_X_WIDTH;
	hnd_sc->height = SCREEN_Y_WIDTH;
	hnd_sc->fg_color = COL8_WHITE;
	hnd_sc->bg_color = COL8_DARK_SKYBLUE;
	hnd_sc->line_number = 0;

	/* setup palette */
	init_palette(); 
	
	/* paint background */
	for(i=0; i<hnd_sc->width*hnd_sc->height; i++) {
		vramaddr = hnd_sc->vram_base + i;
		*vramaddr = COL8_DARK_SKYBLUE;
	}

	return hnd_sc;
}

