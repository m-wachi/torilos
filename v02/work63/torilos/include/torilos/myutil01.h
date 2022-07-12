#ifndef MYUTIL01_H
#define MYUTIL01_H
#include "trscreen.h"
#include "trconsole01.h"


/* timer handle */
typedef struct my_timer_handle {
	unsigned long start;
	unsigned long end;
} HND_TMR;



void print_varval01(HND_SC* hnd_sc, char* varname, int value, int mode);
void print_varval02(HND_SC* hnd_sc, char* varname, int value, int mode,
					int x, int y);
void print_varval12(HND_SC* hnd_sc, char* varname, unsigned long value,
					int mode, int x, int y);
void puts_pos(HND_SC *hnd_sc, char *s, int x, int y);
void putc_pos(HND_SC *hnd_sc, unsigned char c, int x, int y);

char* sprint_varval_ul(char* buff, char* varname, unsigned long value, 
					   int mode);

void puts_varval_ul(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char* varname, 
					unsigned long value, int mode);


void puts_varval01(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char* varname, 
				   int value, int mode);
void puts_varval01_col(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char* varname, 
				 int value, int mode, char color);

void put_multi(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char* s);

void my_set_timeout(HND_TMR* tmr, unsigned long timeout);
int my_is_timeout(HND_TMR* tmr);
void fancy_wait(unsigned long wait);
int fancy_wait_fdcint();

void myutil_panic(char* msg);

#endif
