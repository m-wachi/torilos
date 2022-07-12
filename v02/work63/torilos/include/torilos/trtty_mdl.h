#ifndef TRTTY_MDL_H
#define TRTTY_MDL_H

#include <torilos/trconsole01.h>
#include <torilos/trtermmgr.h>

typedef struct trtym_tty_cnsl {
	TCNS_CONSOLE* nml_cnsl;
	TRTRM_CNSL* edt_cnsl;
	/**
	 * @brief flg if EDITOR mode
	 * 
	 * 0: normal mode, 1: EDITOR mode
	 */
	int flg_editor;
	
	/**
	 * @brief read mode
	 * 
	 *  TRTTY_READ_RAW, TRTTY_READ_CHAR
	 */
	int read_mode;
} TRTYM_TTY_CNSL;

void trtym_init();
unsigned int trtym_read(int tty_no, char* buf, unsigned int sz);
unsigned char trtym_getc(int tty_no);
void trtym_editor(int tty_no, int flg_editor);
void trtym_write(int tty_no, char* buf, unsigned int sz);
void trtym_close(int tty_no);

#endif
