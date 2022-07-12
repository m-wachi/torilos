#ifndef MYCONSOLE_H
#define MYCONSOLE_H

#define CNSL_MAX_LINES 400
//#define CNSL_MAX_LINES 10

#define TCNS_LINE_SIZE 80
#define CNSL_DISP_LINES 25
//#define CNSL_DISP_LINES 4

typedef struct my_cnsl_line {
	char line[TCNS_LINE_SIZE+2];
	char color;
} MY_CNSL_LINE;

typedef struct tcns_console {
	MY_CNSL_LINE ary_l[CNSL_MAX_LINES];
	int idx_current_line;	//line index
	int idx_top;
	int idx_next_char;	//char position
	int flg_show_cursor;	//!< 1: show cursor block
} TCNS_CONSOLE;

void tcns_init_console(TCNS_CONSOLE* cnsl);
void tcns_putchar(TCNS_CONSOLE* cnsl, char c, char color);
void tcns_put_line2(TCNS_CONSOLE* cnsl, char* line, char color);
void tcns_put_line(TCNS_CONSOLE* cnsl, char* line, char color);
void tcns_cons_adjust_idx(TCNS_CONSOLE* cnsl);
MY_CNSL_LINE* tcns_get_line(TCNS_CONSOLE* cnsl, int idx);

int tcns_get_cur_line_idx(TCNS_CONSOLE* cnsl);

#endif
