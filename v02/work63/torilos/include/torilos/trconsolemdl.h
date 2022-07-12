#ifndef TRCONSOLE_MDL_H
#define TRCONSOLE_MDL_H

#include "trscreen.h"
#include "trconsole01.h"
#include "graphic01.h"

#define TCNM_BACKGROUND_COLOR COL8_BLACK 


/* line input handle */
typedef struct hnd_line_input {
	int x, y;
	unsigned char line[100];
} HND_LIPT;

void tcnm_init(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl);
void tcnm_init_screen(HND_SC *hnd_sc);
void lipt_init(HND_LIPT* hnd_lipt);
void lipt_putc(HND_SC* hnd_sc, TCNS_CONSOLE* cnsl, 
			   HND_LIPT* hnd_lipt, unsigned char c);

void tcnm_putc(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char c, char color);
void tcnm_refresh(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl);
void fast_disp_console(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl);
void fast_clear_console(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl);


void disp_console_line(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl);
void disp_console(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl);
void redisp_console(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, int pos);
void tcnm_disp_char(HND_SC *hnd_sc, unsigned char c, int idx_char, int idx_line,
		char color);
void my_puts(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char* s);
void my_puts_col(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char* s, char color);
void tcnm_boxfill(HND_SC* hnd_sc, int idx_char, int idx_line, char color);

#endif
