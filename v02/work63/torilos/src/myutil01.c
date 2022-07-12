#include <torilos/myutil01.h>
#include <torilos/graphic01.h>
#include <torilos/fifo01.h>
#include <torilos/mylib01.h>
#include <torilos/ext2fs.h>
#include <torilos/debug01.h>
#include <torilos/trfilemdl.h>
#include <torilos/myasmfunc01.h>
#include <torilos/trutil.h>
#include <torilos/trconsolemdl.h>
#include <torilos/trtermmgr.h>
#include <torilos/mytask01.h>

extern unsigned long g_time_counter;
extern FIFO8 g_floppyque;

void puts_pos(HND_SC *hnd_sc, char *s, int x, int y) {
	putfonts8_asc(hnd_sc->vram_base, hnd_sc->width, x, y,
				  COL8_WHITE, (unsigned char*)s);

}

void putc_pos(HND_SC *hnd_sc, unsigned char c, int x, int y) {

	extern char _hankaku[4096];

	boxfill8((unsigned char*)hnd_sc->vram_base, hnd_sc->width, 
			 COL8_DARKBLUE,
			 x, y, x+8, y+15);

	putfont8(hnd_sc->vram_base, hnd_sc->width, x, y, 
			 COL8_WHITE, _hankaku + c * 16);
}


/*
 * draw mouse cursor center
 */
void draw_init_mcursor(char* mcursor, HND_SC *hnd_sc) {
	int mx, my;

	/* calculate center position */
	/* mx = (SCREEN_X_WIDTH - 16) / 2;
	   my = (SCREEN_Y_WIDTH - 16) / 2;
	*/

	mx = (hnd_sc->width - 16) / 2;
	my = (hnd_sc->height - 16) / 2;

	/* init mouse-cursor */
	init_mouse_cursor8(mcursor, COL8_DARK_SKYBLUE);

	/* draw mouse-cursor */
	putblock8_8(hnd_sc->vram_base, hnd_sc->width, 16, 16, mx, my, mcursor, 16);

}



/*
 * print variable and value on screen.
 * hnd_sc - screen-handle
 * varname - variable name string
 * value - variable value
 * mode - 0: decimal, 1: hex
 */
void print_varval01(HND_SC* hnd_sc, char* varname, int value, int mode) {
	
	int x, y;

	x = 0;
	y = hnd_sc->line_number * 16;

	print_varval02(hnd_sc, varname, value, mode, x, y);
	hnd_sc->line_number++;

}


void puts_varval01(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char* varname, 
				 int value, int mode) {
	char buff[100];
	sprint_varval(buff, varname, value, mode);
	my_puts(hnd_sc, cnsl, buff);
}

void puts_varval01_col(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char* varname, 
				 int value, int mode, char color) {
	char buff[100];
	sprint_varval(buff, varname, value, mode);
	my_puts_col(hnd_sc, cnsl, buff, color);
}


/*
 * make string of unsigned long variable and value on screen.
 * buff - string buffer
 * varname - variable name string
 * value - variable value
 * mode - 0: decimal, 1: hex
 */
char* sprint_varval_ul(char* buff, char* varname, unsigned long value, 
					   int mode) {
	char buffval[50];

	my_strcpy(buff, varname);
	my_strcat(buff, "=");

	if (mode) 
		my_ultoh(buffval, value);
	else
		my_ultoa(buffval, value);

	my_strcat(buff, buffval);
	
	return buff;
}

void puts_varval_ul(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char* varname, 
				 unsigned long value, int mode) {
	char buff[100];
	sprint_varval_ul(buff, varname, value, mode);
	my_puts(hnd_sc, cnsl, buff);
}

/*
 * print variable and value on screen.
 * hnd_sc - screen-handle
 * varname - variable name string
 * value - variable value
 * mode - 0: decimal, 1: hex
 * x, y - position
 */
void print_varval02(HND_SC* hnd_sc, char* varname, int value, int mode, int x, int y) {
	char buff[1024];
	char buffval[50];

	my_strcpy(buff, varname);
	my_strcat(buff, "=");

	if (mode) 
		my_itoh(buffval, value);
	else
		my_itoa(buffval, value);
	my_strcat(buff, buffval);

	putfonts8_asc(hnd_sc->vram_base, hnd_sc->width, x, y,
				  COL8_WHITE, (unsigned char*)buff);
	
}

/*
 * print unsigned long variable and value on screen.
 * hnd_sc - screen-handle
 * varname - variable name string
 * value - variable value
 * mode - 0: decimal, 1: hex
 * x, y - position
 */
void print_varval12(HND_SC* hnd_sc, char* varname, unsigned long value,
					int mode, int x, int y) {
	char buff[1024];
	char buffval[50];

	my_strcpy(buff, varname);
	my_strcat(buff, "=");

	if (mode) 
		my_ultoh(buffval, value);
	else
		my_ultoa(buffval, value);

	my_strcat(buff, buffval);

	putfonts8_asc(hnd_sc->vram_base, hnd_sc->width, x, y,
				  COL8_WHITE, (unsigned char*)buff);
	
}


/*
 * set timeout
 * @tmr timer handle
 * @timeout timeout (10msec)
 */
void my_set_timeout(HND_TMR* tmr, unsigned long timeout) {
	tmr->start = g_time_counter;
	tmr->end = g_time_counter + timeout;
}
	
/*
 * timeout?
 * @tmr timer handle
 */
int my_is_timeout(HND_TMR* tmr) {
	if (g_time_counter >= tmr->end)
		return -1;
	return 0;
}

/*
 * wait-loop
 * @wait wait time (10msec)
 */
void fancy_wait(unsigned long wait) {
	unsigned long end;
	unsigned long i;
	end = g_time_counter + wait;
	debug_puts("fancy_wait start..");

	//io_sti();
	for(i=0; i<0xffffffff; i++) {
		if (g_time_counter >= end) break;
		io_hlt();

	}
	//io_cli();

}

/*
 * wait-loop fdc-interrupt
 * @return que data
 */
int fancy_wait_fdcint() {

	unsigned long i;
	int data;

	//debug_puts("fancy_wait_fdcint start..");

	//io_sti();

	for(i=0; i<0xffffffff; i++) {
		if (0 < fifo8_count(&g_floppyque)) {
			data = fifo8_get(&g_floppyque);
			break;
		}
		io_hlt();
	}
	//io_cli();
	return data;

}

void put_multi(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl, char* s) {
	unsigned char buff[256];
	unsigned char *p_s;
	int i, tab_left;

	p_s = (unsigned char*)s;

	i = 0;
	while(*p_s != '\0') {
		if ('\n' == *p_s) {
			buff[i] = '\0';
			my_puts(hnd_sc, cnsl, (char*)buff);
			i = 0; p_s++;
		} 
		else if ('\t' == *p_s) {
			tab_left = 8 - i % 8;

			//for(i2=0; i2<tab_left; i2++) 
			while(tab_left-- > 0)
				buff[i++] = ' ';
			p_s++;
		}	
		else {
			buff[i++] = *p_s++;
		}
	}
	buff[i] = '\0';
	my_puts(hnd_sc, cnsl, (char*)buff);

}

void myutil_panic(char* msg) {
	io_cli();

	trtrm_puts("!!!! P A N I C !!!!");
	trtrm_puts(msg);
	tsk_set_switch_interval(0);
	for(;;) {
		debug_refresh();
		io_stihlt();
	}
	
	
}

