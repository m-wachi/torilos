#include <torilos/debug01.h>
#include <torilos/trconsole01.h>
#include <torilos/myutil01.h>
#include <torilos/trconsolemdl.h>

static HND_SC* p_deb_hnd_sc;

static TCNS_CONSOLE* p_deb_cnsl;

static int ml_debug_flg;	//0: OFF, 1: ON

void debug_init(HND_SC* hnd_sc, TCNS_CONSOLE* cnsl) {
	p_deb_hnd_sc = hnd_sc;
	p_deb_cnsl = cnsl;
	ml_debug_flg = 1;

}

void debug_switch(int flg) {
	ml_debug_flg = flg;
}

int get_debug_switch() {
	return ml_debug_flg;
}


void debug_puts(char* s) {
	if (ml_debug_flg)
		my_puts(p_deb_hnd_sc, p_deb_cnsl, s);
	
}

void debug_varval01(char* varname, int value, int mode) {
	if (ml_debug_flg)
		puts_varval01(p_deb_hnd_sc, p_deb_cnsl, varname, value, mode);
}

void debug_refresh() {
	tcnm_refresh(p_deb_hnd_sc, p_deb_cnsl);
}

void debug_putmulti(char* s) {
	char buff[256];
	unsigned char *p_s;
	int i, tab_left;

	if (!ml_debug_flg) return;
	
	p_s = (unsigned char*)s;

	i = 0;
	while(*p_s != '\0') {
		if ('\n' == *p_s) {
			buff[i] = '\0';
			my_puts(p_deb_hnd_sc, p_deb_cnsl, buff);
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
	my_puts(p_deb_hnd_sc, p_deb_cnsl, buff);

}
