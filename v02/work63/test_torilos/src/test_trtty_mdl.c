#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <torilos/trtty.h>
#include <torilos/trtty_opr.h>
#include <torilos/trtty_mdl.h>
#include <torilos/trmajor.h>

int test_getc_blkio();
int test_read();
int test_read2();
int test_opr();

int main(int argc, char** argv) {
	int errcnt = 0;
	
	trtym_init();

	//errcnt += test_getc_nonblk();
	
	errcnt += test_getc_blkio();
	
	errcnt += test_read();
	
	errcnt += test_read2();
	
	trind_init();
	
	//errcnt += test_trtyo_namei();
	
	errcnt += test_opr();
	
	if (errcnt)
		return -1;
		
	puts("OK - test passed.");
	return 0;
}

int test_getc_blkio() {
	char buff[80];
	int errcnt = 0;
	
	trtty_open(0);

	buff[1] = 0;

	trtty_fifoput(0, 0x1e);	// press 'a'
	trtty_fifoput(0, 0x9e);	// release 'a'
	trtty_fifoput(0, 0x30);	// press 'b'

	trtty_blockio(0, TRTTY_BLKIO_ON);
	
	buff[0] = trtym_getc(0);
	if (buff[0] != 'a') {
		fprintf(stderr, "Error - trtty_getc()=%#x:[%s]\n", buff[0], buff);
		errcnt++;
	}

	buff[0] = trtym_getc(0);
	if (buff[0] != 'b') {
		fprintf(stderr, "Error - trtty_getc()=%#x:[%s]\n", buff[0], buff);
		errcnt++;
	}
	
	
	trtty_blockio(0, TRTTY_BLKIO_OFF);
	trtty_close(0);
	
	return errcnt;
}

int test_read() {
	char buff[80];
	int errcnt = 0;
	int sz_act_read;
	
	trtty_open(0);

	buff[1] = 0;

	trtty_fifoput(0, 0x1e);	// press 'a'
	trtty_fifoput(0, 0x9e);	// release 'a'
	trtty_fifoput(0, 0x30);	// press 'b'

	
	memset(buff, 0, sizeof(buff));
	
	sz_act_read = trtym_read(0, buff, 3);
	
	if (strcmp("ab", buff)) {
		fprintf(stderr, "Error - trtty_read() buff=%s, sz_act_read=%d\n", buff, sz_act_read);
		errcnt++;
	}
	
	trtty_fifoput(0, 0x1e);	// press 'a'

	trtty_blockio(0, TRTTY_BLKIO_ON);

	buff[1] = 0;
	
	sz_act_read = trtym_read(0, buff, 1);
	
	if (buff[0] != 'a') {
		fprintf(stderr, "Error - trtty_read() buff=%s, sz_act_read=%d\n", buff, sz_act_read);
		errcnt++;
	}

	trtty_blockio(0, TRTTY_BLKIO_OFF);
	
	trtty_close(0);
	
	return errcnt;

}

int test_read2() {
	char buff[80];
	int errcnt = 0;
	int sz_act_read;
	
	trtty_open(0);
	
	trtym_editor(0, 1);

	buff[1] = 0;

	trtty_fifoput(0, 0x1e);	// press 'a'
	trtty_fifoput(0, 0x9e);	// release 'a'
	trtty_fifoput(0, 0x30);	// press 'b'

	
	memset(buff, 0, sizeof(buff));
	
	sz_act_read = trtym_read(0, buff, 2);
	
	if (2 != sz_act_read) {
		fprintf(stderr, "Error - trtty_read() sz_act_read=%d\n", sz_act_read);
		errcnt++;
	}
	
	if (0x1e != (unsigned char)buff[0]) {
		fprintf(stderr, "Error - trtty_read() buff[0]=%#x\n", (unsigned char)buff[0]);
		errcnt++;
	}
	
	if (0x9e != (unsigned char)buff[1]) {
		fprintf(stderr, "Error - trtty_read() buff[1]=%#x\n", (unsigned char)buff[1]);
		errcnt++;
	}
	
	sz_act_read = trtym_read(0, buff, 1);
	
	if (1 != sz_act_read) {
		fprintf(stderr, "Error - 2nd trtty_read() sz_act_read=%d\n", sz_act_read);
		errcnt++;
	}
	
	if (0x30 != (unsigned char)buff[0]) {
		fprintf(stderr, "Error - 2nd trtty_read() buff[0]=%#x\n", (unsigned char)buff[0]);
		errcnt++;
	}
	
	sz_act_read = trtym_read(0, buff, 1);
	
	if (0 != sz_act_read) {
		fprintf(stderr, "Error - 3rd trtty_read() sz_act_read=%d\n", sz_act_read);
		errcnt++;
	}

	trtty_fifoput(0, 0x1e);	// press 'a'

	trtty_blockio(0, TRTTY_BLKIO_ON);

	buff[1] = 0;
	
	sz_act_read = trtym_read(0, buff, 1);
	
	if (buff[0] != 0x1e) {
		fprintf(stderr, "Error - trtty_read() buff[0]=%#x, sz_act_read=%d\n", buff[0], sz_act_read);
		errcnt++;
	}

	trtty_blockio(0, TRTTY_BLKIO_OFF);
	
	trtym_editor(0, 0);
	
	trtty_close(0);
	
	return errcnt;

}

int test_trtyo_namei() {
	int errcnt = 0;
	TR_INODE* tr_inode;
	
	tr_inode = trtyo_namei("/dev/tty1");
	
	if (TTY_MAJOR != tr_inode->log_dev_no
			|| 2 != tr_inode->inono) {

		fprintf(stderr, "Error - tr_inode->log_dev_no=%d, tr_inode->inono=%d\n", tr_inode->log_dev_no, tr_inode->inono);
		errcnt++;
	}
	
	return errcnt;
}

int test_opr() {
	int errcnt = 0;
	TFL_HND_FILE* user_fdtbl[16];
	int fd;
	TR_INODE* tr_inode;
	char buff[80];
	
	tfl_init_userfd(user_fdtbl);
	
	fd = trtyo_open("/dev/tty0", 0, user_fdtbl);

	tr_inode = user_fdtbl[fd]->tr_inode;
	
	if (TTY_MAJOR != tr_inode->log_dev_no
			|| 1 != tr_inode->inono) {

		fprintf(stderr, "Error - trtyo_open() - tr_inode->log_dev_no=%d, tr_inode->inono=%d\n", 
				tr_inode->log_dev_no, tr_inode->inono);
		errcnt++;
	}	
	trtty_fifoput(0, 0x1e);	// press 'a'
	trtty_fifoput(0, 0x9e);	// release 'a'
	trtty_fifoput(0, 0x30);	// press 'b'
	
	memset(buff, 0, sizeof(buff));
	
	trtyo_read(fd, buff, 1, user_fdtbl);

	if ('a' != buff[0]) {
		fprintf(stderr, "Error - trtyo_read() - buff=%#x:[%s]\n", 
				buff[0], buff);
		errcnt++;
	}

	trtyo_read(fd, buff, 1, user_fdtbl);
	if ('b' != buff[0]) {
		fprintf(stderr, "Error - trtyo_read() - buff=%#x:[%s]\n", 
				buff[0], buff);
		errcnt++;
	}

	
	trtyo_close(fd, user_fdtbl);
	if (0 != user_fdtbl[fd]) {
		fprintf(stderr, "Error - trtyo_close() - user_fdtbl[fd] != 0\n");
		errcnt++;
	}
	
	return errcnt;
}
// stab function
void trtrm_write(char* s, unsigned int sz) {
	fwrite(s, 1, sz, stdout);
}

TCNS_CONSOLE* trtrm_new_console() {
	return 0;
}

void trtrm_set_cur_console(TCNS_CONSOLE* cnsl) {
	return;
}

TCNS_CONSOLE* trtrm_get_cur_console() {
	return 0;
}

void trtrm_edt_putc(TRTRM_CNSL* edt_cnsl, char c, 
		char forecolor, char bgcolor) {
	
}

void trtrm_switch_editor(TRTRM_CNSL* edt_cnsl) {
}

/**
 * @brief switch normal mode
 * @param flg 
 */
void trtrm_switch_normal() {
}

TRTRM_CNSL* trtrm_new_edt_console() {
	return 0;
}

void trtrm_edt_clear_line(TRTRM_CNSL* edt_cnsl) {
}

void trtrm_close_edt_console() {
}


