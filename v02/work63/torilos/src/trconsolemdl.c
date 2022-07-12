#include <torilos/trconsolemdl.h>
#include <torilos/graphic01.h>
#include <torilos/mylib01.h>

static int ml_flg_update = 0;


void lipt_init(HND_LIPT* hnd_lipt) {
	hnd_lipt->x = 0;
	hnd_lipt->y = 0;
	my_memset(hnd_lipt->line, 0, sizeof(hnd_lipt->line));
}

//void lipt_putc(HND_SC* hnd_sc, TCNS_CONSOLE* cnsl, 
//			   HND_LIPT* hnd_lipt, unsigned char c) {
//
//	/*
//	debug_puts("lipt_putc");
//	debug_varval01("x=", hnd_lipt->x);
//	debug_varval01("y=", hnd_lipt->y);
//	debug_varval01("c=", c);
//	*/
//	switch (c) {
//	case 0x08:
//		boxfill8(hnd_sc->vram_base, hnd_sc->width, COL8_DARKBLUE,
//				 hnd_lipt->x * 8, hnd_lipt->y*16,
//				 hnd_lipt->x * 8 + 8, hnd_lipt->y*16 + 15);
//		hnd_lipt->x--;
//		hnd_lipt->line[hnd_lipt->x] = '\0';
//		break;
//	case 0x0a:
//		my_puts(hnd_sc, cnsl, hnd_lipt->line);
//
//		boxfill8(hnd_sc->vram_base, hnd_sc->width, COL8_DARKBLUE,
//				 0, hnd_lipt->y*16,
//				 hnd_lipt->x * 8 + 8, hnd_lipt->y*16+15);
//		hnd_lipt->x = 0;
//		hnd_lipt->line[hnd_lipt->x] = '\0';
//		break;
//	default:
//		hnd_lipt->line[hnd_lipt->x] = c;
//		hnd_lipt->line[hnd_lipt->x+1] = '\0';
//		putc_pos(hnd_sc, c, hnd_lipt->x * 8, hnd_lipt->y*16);
//
//		hnd_lipt->x++;
//	}
//
//	boxfill8(hnd_sc->vram_base, hnd_sc->width, COL8_WHITE,
//			 hnd_lipt->x * 8, hnd_lipt->y*16,
// 			 hnd_lipt->x * 8 + 8, hnd_lipt->y*16 + 15);
//
//}

void lipt_putc(HND_SC* hnd_sc, TCNS_CONSOLE* cnsl, 
			   HND_LIPT* hnd_lipt, unsigned char c) {

	switch (c) {
	case 0x08:
		hnd_lipt->x--;
		hnd_lipt->line[hnd_lipt->x] = '\0';
		break;
	case 0x0a:
		hnd_lipt->x = 0;
		hnd_lipt->line[hnd_lipt->x] = '\0';
		break;
	default:
		hnd_lipt->line[hnd_lipt->x] = c;
		hnd_lipt->line[hnd_lipt->x+1] = '\0';

		hnd_lipt->x++;

	}
	tcnm_putc(hnd_sc, cnsl, c, COL8_WHITE);

}

void tcnm_init(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl) {

	tcnm_init_screen(hnd_sc);
	tcns_init_console(cnsl);
	
}

void tcnm_init_screen(HND_SC *hnd_sc) {

	boxfill8((unsigned char*)hnd_sc->vram_base, hnd_sc->width, 
			 TCNM_BACKGROUND_COLOR, 
			 0, 0, 80*8, (CNSL_DISP_LINES-1)*16+15);
	
}


void tcnm_putc(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char c, char color) {
	int idx_line, tab_left;
	char last_char;
	
	idx_line = tcns_get_cur_line_idx(cnsl);
	
	switch (c) {
	case 0x08:	//backspace
		last_char = cnsl->ary_l[cnsl->idx_current_line].line[cnsl->idx_next_char];

		tcnm_disp_char(hnd_sc, c, cnsl->idx_next_char-1, idx_line, TCNM_BACKGROUND_COLOR);

		tcns_putchar(cnsl, c, color);
		break;
	case 0x09:	//'\t'
		tab_left = 8 - cnsl->idx_next_char % 8;
		while(tab_left-- > 0)
			tcnm_putc(hnd_sc, cnsl, ' ', color);
		break;
	case 0x0a:	//'\n'
		fast_clear_console(hnd_sc, cnsl);
		tcns_putchar(cnsl, c, color);
		fast_disp_console(hnd_sc, cnsl);
		break;
	default:
		
		tcnm_disp_char(hnd_sc, c, cnsl->idx_next_char, idx_line, color);
		
		tcns_putchar(cnsl, c, color);

	}
	ml_flg_update = 1;
}

void fast_clear_console(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl) {
	int i=0;
	//char *p;
	MY_CNSL_LINE* cnsl_line;

	//while(p = get_line_old(cnsl, i)){
	while((cnsl_line = tcns_get_line(cnsl, i))) {
		putfonts8_asc(hnd_sc->vram_base, hnd_sc->width, 0, i*16,
				  TCNM_BACKGROUND_COLOR, (unsigned char*)cnsl_line->line);
		i++;
	}
}

void fast_disp_console(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl) {
	int i=0;
	MY_CNSL_LINE* cnsl_line;

	while((cnsl_line = tcns_get_line(cnsl, i))) {
		putfonts8_asc(hnd_sc->vram_base, hnd_sc->width, 0, i*16,
				  cnsl_line->color, (unsigned char*)cnsl_line->line);
		i++;
	}
}


/*
void disp_console_line(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl) {
	int i=0;
	MY_CNSL_LINE* cnsl_line;

	if (cnsl->idx_top == 0) {
		i = cnsl->idx_current_line;
	} else {
		i = CNSL_DISP_LINES - 1;
	}
	
	cnsl_line = tcns_get_line(cnsl, i);
	boxfill8(hnd_sc->vram_base, hnd_sc->width, 
				 COL8_BLACK, 
				 0, i*16, 80*8, i*16+15);
		
	putfonts8_asc(hnd_sc->vram_base, hnd_sc->width, 0, i*16,
				  cnsl_line->color, cnsl_line->line);
	//putfonts8_asc(hnd_sc->vram_base, hnd_sc->width, 0, i*16,
	//			  cnsl_line->color, cnsl_line->line);
}
*/

void tcnm_refresh(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl) {
	if (ml_flg_update) {
		disp_console(hnd_sc, cnsl);
	}
	ml_flg_update = 0;
}


/*
 * refresh display console data
 * (slow and sure version)
 */
void disp_console(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl) {
	int i=0, y=0; 
	MY_CNSL_LINE* cnsl_line;

	while((cnsl_line = tcns_get_line(cnsl, i))) {
		boxfill8((unsigned char*)hnd_sc->vram_base, hnd_sc->width, 
				 COL8_BLACK, 
				 0, i*16, 80*8, i*16+15);
		
		putfonts8_asc(hnd_sc->vram_base, hnd_sc->width, 0, i*16,
				  cnsl_line->color, (unsigned char*)cnsl_line->line);
		i++;
	}
	
	if (cnsl->flg_show_cursor) {

		if (cnsl->idx_top == 0)
			y = cnsl->idx_current_line;
		else
			y = CNSL_DISP_LINES - 1;

		tcnm_boxfill(hnd_sc, cnsl->idx_next_char, y, COL8_YELLOW);
	}
}


void redisp_console(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, int pos) {
	int bkup_next, bkup_top;
	
	bkup_next = cnsl->idx_current_line;
	bkup_top = cnsl->idx_top;

	cnsl->idx_top += pos;
	cnsl->idx_current_line += pos;

	tcns_cons_adjust_idx(cnsl);

	disp_console(hnd_sc, cnsl);

	cnsl->idx_current_line = bkup_next;
	cnsl->idx_top = bkup_top;
}

void tcnm_disp_char(HND_SC *hnd_sc, unsigned char c, int idx_char, int idx_line,
		char color) {

	extern char _hankaku[4096];

	putfont8(hnd_sc->vram_base, hnd_sc->width, idx_char*8, idx_line*16, 
			color, _hankaku + c * 16);
}



void my_puts(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char* s) {
	fast_clear_console(hnd_sc, cnsl);
	tcns_put_line(cnsl, s, COL8_GREEN);
	fast_disp_console(hnd_sc, cnsl);
	//disp_console(hnd_sc, cnsl);
}

void my_puts_col(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char* s, char color) {
	fast_clear_console(hnd_sc, cnsl);
	tcns_put_line(cnsl, s, color);
	fast_disp_console(hnd_sc, cnsl);
	//disp_console(hnd_sc, cnsl);
}

void tcnm_boxfill(HND_SC* hnd_sc, int idx_char, int idx_line, char color) {
	boxfill8((unsigned char*)hnd_sc->vram_base, hnd_sc->width, 
			 color, 
			 idx_char*8, idx_line*16, (idx_char+1)*8, (idx_line)*16+15);
	
}

