#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <torilos/debug01.h>

static int ml_debug_flg;


void debug_varval01(char* varname, int value, int mode) {

	if (!ml_debug_flg) return;
	
	if (mode)
		printf("%s=%#x\n", varname, value);
	else
		printf("%s=%d\n", varname, value);
		
}

void debug_puts(char* s) {
	if (ml_debug_flg)
		puts(s);
}

void debug_switch(int flg) {
	ml_debug_flg = flg;
}

void myutil_panic(char* msg) {
	puts(msg);
}
