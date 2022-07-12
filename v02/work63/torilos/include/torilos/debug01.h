#ifndef DEBUG01_H
#define DEBUG01_H

#include "trscreen.h"
#include "trconsole01.h"

void debug_init(HND_SC* hnd_sc, TCNS_CONSOLE* cnsl);
void debug_switch(int flg);
int get_debug_switch();
void debug_puts(char* s);
void debug_varval01(char* varname, int value, int mode);
void debug_refresh();
void debug_putmulti(char* s);

#endif
