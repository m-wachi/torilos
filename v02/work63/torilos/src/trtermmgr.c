#include <torilos/trconsole01.h>
#include <torilos/myutil01.h>
#include <torilos/trtermmgr.h>
#include <torilos/graphic01.h>
#include <torilos/trconsolemdl.h>

static HND_SC* ml_trtrm_hnd_sc;

static TCNS_CONSOLE* ml_cur_cnsl;

static TCNS_CONSOLE ml_cnsls[TRTRM_CNT_CONSOLE];

static TRTRM_CNSL ml_edt_cnsls[TRTRM_CNT_CONSOLE];

static int ml_cnt_cnsl;
static int ml_cnt_edt_cnsl;

static int ml_editor_mode;

static TRTRM_CNSL* ml_cur_edt_cnsl;

TCNS_CONSOLE* trtrm_init(HND_SC* hnd_sc) {
	int i;
	
	ml_trtrm_hnd_sc = hnd_sc;
	
	for (i=0; i<TRTRM_CNT_CONSOLE; i++)	
		tcns_init_console(&ml_cnsls[i]);
	
	ml_cnt_cnsl = 0;
	ml_cnt_edt_cnsl = 0;
	
	ml_cur_cnsl = trtrm_new_console();
	
	ml_editor_mode = 0;

	return ml_cur_cnsl;
}

/**
 * @brief create new console
 * @return new console
 */
TCNS_CONSOLE* trtrm_new_console() {
	if (ml_cnt_cnsl >= TRTRM_CNT_CONSOLE)
		return 0;
	
	return &ml_cnsls[ml_cnt_cnsl++];
}

void trtrm_init_edt_console(TRTRM_CNSL* edt_cnsl) {
	int x, y;
	TRTRM_CNSL_CELL* cell;
	for (y = 0; y<CNSL_DISP_LINES; y++) {
		for (x=0; x<TCNS_LINE_SIZE+1; x++) {
			cell = &(edt_cnsl->line[y].cell[x]);
			cell->forecolor = 0;
			cell->bgcolor = 0;
			cell->chara = ' ';
		}
		edt_cnsl->line[y].cell[x].chara = 0;
	}
	edt_cnsl->cursor_x = 0;
	edt_cnsl->cursor_y = 0;
}

/**
 * @brief create new editor-mode console
 * @return new console
 */
TRTRM_CNSL* trtrm_new_edt_console() {
	TRTRM_CNSL* cnsl;
	if (ml_cnt_edt_cnsl >= TRTRM_CNT_CONSOLE)
		return 0;
	cnsl = &ml_edt_cnsls[ml_cnt_edt_cnsl++];
	trtrm_init_edt_console(cnsl);
	return cnsl;
}

void trtrm_close_edt_console() {
	if (ml_cnt_edt_cnsl>0)
		ml_cnt_edt_cnsl--;
}

/**
 * @brief set current console
 * @param cnsl console to be current
 */
void trtrm_set_cur_console(TCNS_CONSOLE* cnsl) {
	ml_cur_cnsl = cnsl;
}

/**
 * @brief get current console
 * @return current console
 */
TCNS_CONSOLE* trtrm_get_cur_console() {
	return ml_cur_cnsl;
}


void trtrm_puts(char* s) {
	my_puts_col(ml_trtrm_hnd_sc, ml_cur_cnsl, s, COL8_WHITE);
}

void trtrm_write(char* s, unsigned int sz) {
	while(sz--) {
		tcnm_putc(ml_trtrm_hnd_sc, ml_cur_cnsl, *s, COL8_WHITE);
		s++;
	}
}


void trtrm_varval01(char* varname, int value, int mode) {
	puts_varval01_col(ml_trtrm_hnd_sc, ml_cur_cnsl, varname, value, mode,
			COL8_WHITE);
}

void trtrm_putmulti(char* s) {
	char buff[256];
	unsigned char *p_s;
	int i, tab_left;

	p_s = (unsigned char*)s;

	i = 0;
	while(*p_s != '\0') {
		if ('\n' == *p_s) {
			buff[i] = '\0';
			my_puts(ml_trtrm_hnd_sc, ml_cur_cnsl, buff);
			i = 0; p_s++;
		} 
		else if ('\t' == *p_s) {
			tab_left = 8 - i % 8;

			while(tab_left-- > 0)
				buff[i++] = ' ';
			p_s++;
		}	
		else {
			buff[i++] = *p_s++;
		}
	}
	buff[i] = '\0';
	my_puts_col(ml_trtrm_hnd_sc, ml_cur_cnsl, buff, COL8_WHITE);

}

/**
 * @breif refresh screen
 */
void trtrm_refresh() {
	if (ml_editor_mode) 
		trtrm_disp_all(ml_cur_edt_cnsl);
	else
		tcnm_refresh(ml_trtrm_hnd_sc, ml_cur_cnsl);
}

/**
 * @brief switch editor mode
 * @param flg 
 */
void trtrm_switch_editor(TRTRM_CNSL* edt_cnsl) {
	ml_editor_mode = 1;
	ml_cur_edt_cnsl = edt_cnsl;
}

/**
 * @brief switch normal mode
 * @param flg 
 */
void trtrm_switch_normal() {
	ml_editor_mode = 0;
}

/**
 * @brief display all screen
 * @param cnsl editor console
 */
void trtrm_disp_all(TRTRM_CNSL* cnsl) {
	int y;
	TRTRM_CNSL_CELL* cursor_cell;
	
	for (y=0; y<CNSL_DISP_LINES; y++) {
		trtrm_disp_line(&(cnsl->line[y]), y);
	}
	tcnm_boxfill(ml_trtrm_hnd_sc, cnsl->cursor_x, cnsl->cursor_y, COL8_GREEN);
	
	cursor_cell = &(cnsl->line[cnsl->cursor_y].cell[cnsl->cursor_x]);
	
	if (cursor_cell->chara) {
		tcnm_disp_char(ml_trtrm_hnd_sc, cursor_cell->chara, 
				cnsl->cursor_x, cnsl->cursor_y, cursor_cell->forecolor);
	}
}

/**
 * @brief display line
 * @param cnsl editor console line
 * @param y line position
 */
void trtrm_disp_line(TRTRM_CNSL_LINE* cnsl_line, int y) {
	int x;
	TRTRM_CNSL_CELL* cell;
	
	for (x=0; x<TCNS_LINE_SIZE; x++) {
		cell = &(cnsl_line->cell[x]);
		if (!cell->chara)
			break;
		tcnm_boxfill(ml_trtrm_hnd_sc, x, y, cell->bgcolor);
		tcnm_disp_char(ml_trtrm_hnd_sc, cell->chara, 
				x, y, cell->forecolor);  
	}

}

void trtrm_edt_putc(TRTRM_CNSL* edt_cnsl, char c, 
		char forecolor, char bgcolor) {
	
	int x, y;
	TRTRM_CNSL_CELL* cell;
	x = edt_cnsl->cursor_x;
	y = edt_cnsl->cursor_y;
	cell = &(edt_cnsl->line[y].cell[x]);
	cell->bgcolor = bgcolor;
	cell->forecolor = forecolor;
	cell->chara = c;
	edt_cnsl->cursor_x = x + 1;
}

/**
 * @brief clear one line buffer and screen
 * @param edt_cnsl editor console
 * @param y line position
 */
void trtrm_edt_clear_line(TRTRM_CNSL* edt_cnsl) {
	int x, y;
	TRTRM_CNSL_CELL* cell;
	TRTRM_CNSL_LINE* cnsl_line;
	y = edt_cnsl->cursor_y;
	cnsl_line = &(edt_cnsl->line[y]);

	for (x=0; x<TCNS_LINE_SIZE; x++) {
		cell = &(cnsl_line->cell[x]);
		cell->chara = ' ';
		cell->bgcolor = COL8_BLACK;
		tcnm_boxfill(ml_trtrm_hnd_sc, x, y, cell->bgcolor);
	}
	
}
