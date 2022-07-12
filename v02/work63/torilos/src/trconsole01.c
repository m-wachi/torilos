#include <torilos/trconsole01.h>
#include <torilos/graphic01.h>
#include <torilos/mylib01.h>

void tcns_init_console(TCNS_CONSOLE* cnsl) {
	int i;

	cnsl->idx_current_line = 0;
	cnsl->idx_top = 0;
	cnsl->idx_next_char = 0;
	cnsl->flg_show_cursor = 0;
	
	for (i=0; i<CNSL_MAX_LINES; i++) {
		my_memset(cnsl->ary_l[i].line, 0, TCNS_LINE_SIZE+2);
		cnsl->ary_l[i].color = COL8_WHITE;
	}

}

void tcns_putchar(TCNS_CONSOLE* cnsl, char c, char color) {

	int flg_newline = 0;
	
	switch (c) {
	case 0x08:	//backspace
		cnsl->ary_l[cnsl->idx_current_line].line[--cnsl->idx_next_char] = 0;
		break;
	case 0x0a:	//newline
		cnsl->ary_l[cnsl->idx_current_line].color = color;
		flg_newline = 1;
		break;
	default:
		cnsl->ary_l[cnsl->idx_current_line].color = color;
		cnsl->ary_l[cnsl->idx_current_line].line[cnsl->idx_next_char++] = c;
		cnsl->ary_l[cnsl->idx_current_line].line[cnsl->idx_next_char] = 0;
		if (cnsl->idx_next_char > TCNS_LINE_SIZE) {
			flg_newline = 1;
		}
	}
	
	
	if (flg_newline) {// new-line 
		cnsl->idx_next_char = 0;
		cnsl->idx_current_line++;
		tcns_cons_adjust_idx(cnsl);
		cnsl->ary_l[cnsl->idx_current_line].line[0] = 0;
	}
}


void tcns_put_line(TCNS_CONSOLE* cnsl, char* line, char color) {
	int i;
	
	for (i=0; i<my_strlen(line); i++) {
		tcns_putchar(cnsl, line[i], color);
	}
	tcns_putchar(cnsl, '\n', color);
}

void tcns_put_line_old(TCNS_CONSOLE* cnsl, char* line, char color) {
	char *dst;

	dst = cnsl->ary_l[cnsl->idx_current_line].line;
	
	cnsl->ary_l[cnsl->idx_current_line].color = color;

	my_strncpy(dst, line, TCNS_LINE_SIZE);

	cnsl->idx_current_line++;
	tcns_cons_adjust_idx(cnsl);
}

void tcns_cons_adjust_idx(TCNS_CONSOLE* cnsl) {
	int work_next;

	/* idx_next wrap judge */
	if (CNSL_MAX_LINES <= cnsl->idx_current_line)
		cnsl->idx_current_line = 0;
	else if (0 > cnsl->idx_current_line)
		cnsl->idx_current_line = CNSL_MAX_LINES + cnsl->idx_current_line;
		
	/* scroll judge */
	work_next = cnsl->idx_current_line;
	if (work_next < cnsl->idx_top) {
		work_next += CNSL_MAX_LINES;
	}
	if (work_next - cnsl->idx_top > CNSL_DISP_LINES - 1 ) {
		cnsl->idx_top++;
	}

	/* idx_top wrap judge */
	if (CNSL_MAX_LINES <= cnsl->idx_top) {
		cnsl->idx_top=0;
	} 
	else if (0 > cnsl->idx_top) 
		cnsl->idx_top = CNSL_MAX_LINES + cnsl->idx_top;
}

MY_CNSL_LINE* tcns_get_line(TCNS_CONSOLE* cnsl, int idx) {
	int idx_cur;
	
	if (CNSL_DISP_LINES <= idx)
		return 0;

	idx_cur = cnsl->idx_top + idx;

	/* wrap judge */
	if (CNSL_MAX_LINES <= idx_cur ) {
		idx_cur -= CNSL_MAX_LINES;
	}

	if (cnsl->idx_top <= cnsl->idx_current_line) {
		if (cnsl->idx_current_line < idx_cur) return 0;
	} else {
		if (cnsl->idx_current_line < idx_cur &&
			idx_cur < cnsl->idx_top) return 0;
	}
		
	return &(cnsl->ary_l[idx_cur]);
}	

/*
 * get current line-index on the screen
 */
int tcns_get_cur_line_idx(TCNS_CONSOLE* cnsl) {
	if (cnsl->idx_top == 0) {
		return cnsl->idx_current_line;
	} else {
		return CNSL_DISP_LINES - 1;
	}

}
