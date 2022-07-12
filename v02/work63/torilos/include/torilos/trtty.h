#ifndef TRTTY_H
#include <torilos/fifo01.h>

#define TRTTY_H

#define TRTTY_TTY_CNT		4
#define TRTTY_SZ_TTY_BUFF	32

#define TRTTY_BLKIO_OFF		0
#define TRTTY_BLKIO_ON		1

#define TRTTY_READ_RAW		0
#define TRTTY_READ_CHAR		1

/**
 * @brief TTY I/O Buffer and mode
 */
typedef struct trtty_buffer {
	FIFO8 fifo;
	/**
	 * @brief count of opened
	 */
	int refcount;
	/**
	 * @brief flg if blocking I/O mode
	 * 
	 * 0: non-blocking I/O mode, 1: blocking I/O mode
	 */
	int flg_blockio;
	
} TRTTY_BUFFER;


void trtty_init();
void trtty_fifoput(int tty_no, unsigned char data);
void trtty_open(int tty_no);
void trtty_close(int tty_no);
void trtty_write(int tty_no, char* buf, unsigned int sz);
int trtty_get_blkio_flg(int tty_no);
//unsigned char trtty_getc_nonblk(int tty_no);
unsigned char trtty_get_byte_nonblk(int tty_no, int mode);
void trtty_blockio(int tty_no, int flg_blockio);

#endif
