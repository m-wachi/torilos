#include <torilos/trtty.h>
#include <torilos/trtermmgr.h>
#include <torilos/trkbd.h>

static TRTTY_BUFFER ml_tty[TRTTY_TTY_CNT];
 
static unsigned char ml_keyquebuf[TRTTY_SZ_TTY_BUFF * TRTTY_TTY_CNT];

/**
 * @brief initialize trtty module
 */
void trtty_init() {
	unsigned char* p_quebuf;
	int i;
	
	p_quebuf = ml_keyquebuf;

	for (i=0; i<TRTTY_TTY_CNT; i++) {
		fifo8_init(&ml_tty[i].fifo, TRTTY_SZ_TTY_BUFF, p_quebuf);
		ml_tty[i].refcount = 0;
		ml_tty[i].flg_blockio = 0;
		p_quebuf += TRTTY_SZ_TTY_BUFF;
	}	
	
}

void trtty_fifoput(int tty_no, unsigned char data) {

	if (ml_tty[tty_no].refcount) {
		fifo8_put(&ml_tty[tty_no].fifo, data);
	}
}

void trtty_open(int tty_no) {
	ml_tty[tty_no].refcount++;
}

void trtty_close(int tty_no) {
	if (ml_tty[tty_no].refcount > 0)
		ml_tty[tty_no].refcount--;
}

void trtty_write(int tty_no, char* buf, unsigned int sz) {
	trtrm_write(buf, sz);
}

/**
 * @brief get tty blocking I/O mode flag
 * @param tty_no tty no
 * @return tty's flg_blockio
 */  
int trtty_get_blkio_flg(int tty_no) {
	return ml_tty[tty_no].flg_blockio;
}

/**
 * @brief get one character (non-blocking read)
 * @param tty_no tty no
 * @return char-data, 0 if nothing in que
 */
/*
unsigned char trtty_getc_nonblk(int tty_no) {
	unsigned char kbdchar;
	int data;
	
	if (!ml_tty[tty_no].refcount) return 0;
	
	kbdchar = 0;
	while (0 < fifo8_count(&ml_tty[tty_no].fifo)) {
		data = fifo8_get(&ml_tty[tty_no].fifo);
		if ((kbdchar = tkbd_kbdkey2char(data)))
			break;
	}
	return kbdchar;
}
*/

/**
 * @breif read one byte from fifo
 * @param tty_no tty no
 * @param mode TRTTY_READ_RAW: raw-mode, TRTTY_READ_CHAR: character mode
 * @return read data, 0 if nothing in que
 */
unsigned char trtty_get_byte_nonblk(int tty_no, int mode) {
	unsigned char read_data;
	
	if (!ml_tty[tty_no].refcount) return 0;
	
	read_data = 0;
	while (0 < fifo8_count(&ml_tty[tty_no].fifo)) {
		read_data = fifo8_get(&ml_tty[tty_no].fifo);
		if (TRTTY_READ_CHAR == mode) { 
			if ((read_data = tkbd_kbdkey2char(read_data)))
				break;
		} 
		else
			break;
	}
	return read_data;
	
}

/**
 * @brief change blocking I/O mode
 * @param tty_no tty no
 * @param flg_blockio  0: non-blocking I/O, 1: blocking I/O mode
 */
void trtty_blockio(int tty_no, int flg_blockio) {
	ml_tty[tty_no].flg_blockio = flg_blockio;
}



