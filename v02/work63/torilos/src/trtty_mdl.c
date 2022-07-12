#include <torilos/trtty_mdl.h>
#include <torilos/trtty.h>
#include <torilos/mytask01.h>
#include <torilos/trtermmgr.h>
#include <torilos/graphic01.h>
#include <torilos/myutil01.h>

static TRTYM_TTY_CNSL ml_tty_cnsl[TRTTY_TTY_CNT];

/**
 * @brief initialize trtty_mdl module
 */
void trtym_init() {
	int i;
	
	for (i=0; i<TRTTY_TTY_CNT; i++) {
		ml_tty_cnsl[i].nml_cnsl = 0;
		ml_tty_cnsl[i].edt_cnsl= 0;
		ml_tty_cnsl[i].flg_editor = 0;
		ml_tty_cnsl[i].read_mode = TRTTY_READ_CHAR;
	}
	
	trtty_init();
}

/**
 * @brief "read" procedure for tty
 * @param tty_no tty no
 * @param buf
 * @param count 
 * @return actual read size
 */
unsigned int trtym_read(int tty_no, char* buf, unsigned int count) {
	unsigned int sz_actual_read;
	unsigned char kbdchar;
	
	//sz_actual_read = 0;
	for (sz_actual_read=0; sz_actual_read<count; sz_actual_read++) {
		kbdchar = trtym_getc(tty_no);
		buf[sz_actual_read] = kbdchar;
		//if (!kbdchar && TRTTY_BLKIO_OFF == ml_tty[tty_no].flg_blockio) {
		if (!kbdchar && TRTTY_BLKIO_OFF == trtty_get_blkio_flg(tty_no)) {
			break; 
		}
	}
	return sz_actual_read;
}

/**
 * @brief get one character
 * 
 * wait if non-blocking I/O mode. 
 * @param tty_no tty no
 * @return char-data, 0 if nothing in que
 */
unsigned char trtym_getc(int tty_no) {
	unsigned char kbdchar;
	int read_mode;
	
	read_mode = ml_tty_cnsl[tty_no].read_mode;
	
	if (TRTTY_BLKIO_OFF == trtty_get_blkio_flg(tty_no)) {
		//kbdchar = trtty_getc_nonblk(tty_no);
		//kbdchar = trtty_get_byte_nonblk(tty_no, TRTTY_READ_CHAR);
		kbdchar = trtty_get_byte_nonblk(tty_no, read_mode);
	} 
	else {
		while(1) {
			//kbdchar = trtty_getc_nonblk(tty_no);
			//kbdchar = trtty_get_byte_nonblk(tty_no, TRTTY_READ_CHAR);
			kbdchar = trtty_get_byte_nonblk(tty_no, read_mode);
			if (kbdchar) break;
			tsk_waitkbd_curtask();
		}
	}
	
	return kbdchar;
		
}

/**
 * @brief switch EDITOR mode
 * @param tty_no tty no
 * @param flg_blockio  0: normal mode, 1: EDITOR mode
 */
void trtym_editor(int tty_no, int flg_editor) {
	TRTRM_CNSL* new_edt_cnsl;
	ml_tty_cnsl[tty_no].flg_editor = flg_editor;
	if (flg_editor) {
		ml_tty_cnsl[tty_no].read_mode = TRTTY_READ_RAW;
		ml_tty_cnsl[tty_no].nml_cnsl = trtrm_get_cur_console();
		new_edt_cnsl = trtrm_new_edt_console();
		if (!new_edt_cnsl)
			myutil_panic("Couldn't allocate new editor console.");
		ml_tty_cnsl[tty_no].edt_cnsl = new_edt_cnsl;
		trtrm_switch_editor(new_edt_cnsl);
	} else {
		ml_tty_cnsl[tty_no].read_mode = TRTTY_READ_CHAR;
		trtrm_switch_normal();
	}
}

/**
 * @brief write tty
 * @param tty_no tty no
 * @param buf 
 * @param sz
 */
void trtym_write(int tty_no, char* buf, unsigned int sz) {
	TRTRM_CNSL* cnsl;
	
	if (0 == ml_tty_cnsl[tty_no].flg_editor) {
		trtty_write(tty_no, buf, sz);
		return;
	}

	cnsl = ml_tty_cnsl[tty_no].edt_cnsl;
	
	switch (buf[0]) {
	case 'p':
		trtrm_edt_putc(cnsl, buf[1], buf[2], buf[3]);
		break;
	case 'r':
		cnsl->cursor_x += (int)buf[1];
		cnsl->cursor_y += (int)buf[2];
		break;
	case 'a':
		cnsl->cursor_x = (int)buf[1];
		cnsl->cursor_y = (int)buf[2];
		break;
	case 'c':
		cnsl->cursor_y = (int)buf[1];
		trtrm_edt_clear_line(cnsl);
		break;
	}
	//trtrm_write(buf, sz);
}

void trtym_close(int tty_no) {
	if (ml_tty_cnsl[tty_no].edt_cnsl) {
		trtrm_close_edt_console();
		ml_tty_cnsl[tty_no].edt_cnsl = 0;
	}
	trtty_close(tty_no);
}
