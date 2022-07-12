#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trconsole01.h"

static TCNS_CONSOLE my_cnsl;

void disp_console(TCNS_CONSOLE* cnsl);

int main(int argc, char** argv) {
	int i;
	char buff[100];

	puts("Please set following constants for testing.");
	puts("\t#define CNSL_MAX_LINES 10");
	puts("\t#define CNSL_DISP_LINES 4");
	
	tcns_init_console(&my_cnsl);

	tcns_putchar(&my_cnsl, 'T', 0);
	
	disp_console(&my_cnsl);
	
	tcns_putchar(&my_cnsl, 'O', 0);
	
	disp_console(&my_cnsl);
	
	for (i=0; i<CNSL_DISP_LINES-1; i++) {
		sprintf(buff, "Hello: %03d", i);
		tcns_put_line(&my_cnsl, buff, 0);
	}
	disp_console(&my_cnsl);

	tcns_put_line(&my_cnsl, "hello world.", 0);
	disp_console(&my_cnsl);

	tcns_putchar(&my_cnsl, 'Z', 0);
	disp_console(&my_cnsl);
	
	tcns_put_line(&my_cnsl, "hey hey hey.", 0);
	disp_console(&my_cnsl);

	tcns_put_line(&my_cnsl, "hey hey hey2.", 0);
	disp_console(&my_cnsl);

	for (i=0; i<3; i++) {
		sprintf(buff, "Hello: %03d", i);
		tcns_put_line(&my_cnsl, buff, 0);
	}
	disp_console(&my_cnsl);


	tcns_put_line(&my_cnsl, "hey hey hey3.", 0);
	disp_console(&my_cnsl);


	tcns_put_line(&my_cnsl, "hey hey hey4.", 0);
	disp_console(&my_cnsl);

	tcns_put_line(&my_cnsl, "hey hey hey5.", 0);
	disp_console(&my_cnsl);

	tcns_put_line(&my_cnsl, "hey hey hey6.", 0);
	disp_console(&my_cnsl);

	tcns_putchar(&my_cnsl, 'T', 0);
	disp_console(&my_cnsl);

	return 0;
}

void disp_console(TCNS_CONSOLE* cnsl) {
	int i=0;
	MY_CNSL_LINE *p;

	puts("----------------------");
	while(p = tcns_get_line(cnsl, i++)){;
		puts(p->line);
	}
	printf("\ttop=%d, line=%d, next_char=%d\n", cnsl->idx_top, cnsl->idx_current_line,
			cnsl->idx_next_char);
}
