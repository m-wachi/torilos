#ifndef TRTRERMMGR_H
#define TRTRERMMGR_H

#include "trscreen.h"
#include "trconsole01.h"

#define TRTRM_CNT_CONSOLE	2

typedef struct trtrm_cnsl_cell {
	char bgcolor;
	char forecolor;
	char chara;
} TRTRM_CNSL_CELL;


typedef struct trtrm_cnsl_line {
	TRTRM_CNSL_CELL cell[TCNS_LINE_SIZE];
} TRTRM_CNSL_LINE;

typedef struct trtrm_cnsl {
	TRTRM_CNSL_LINE line[CNSL_DISP_LINES+1];
	int cursor_x;
	int cursor_y;
} TRTRM_CNSL;


TCNS_CONSOLE* trtrm_init(HND_SC* hnd_sc);
TCNS_CONSOLE* trtrm_new_console();
TRTRM_CNSL* trtrm_new_edt_console();
void trtrm_close_edt_console();
void trtrm_set_cur_console(TCNS_CONSOLE* cnsl);
TCNS_CONSOLE* trtrm_get_cur_console();
void trtrm_puts(char* s);
void trtrm_varval01(char* varname, int value, int mode);
void trtrm_putmulti(char* s);
void trtrm_write(char* s, unsigned int sz);
void trtrm_refresh();
void trtrm_switch_editor(TRTRM_CNSL* edt_cnsl);
void trtrm_switch_normal();
void trtrm_disp_all(TRTRM_CNSL* cnsl);
void trtrm_disp_line(TRTRM_CNSL_LINE* cnsl_line, int y);
void trtrm_edt_putc(TRTRM_CNSL* edt_cnsl, char c, 
		char forecolor, char bgcolor);
void trtrm_edt_clear_line(TRTRM_CNSL* edt_cnsl);

#endif
