#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <torilos/trtty.h>
#include <torilos/trtty_opr.h>
#include <torilos/trmajor.h>
#include <torilos/trconsole01.h>


void trtrm_write(char* s, unsigned int sz);
TCNS_CONSOLE* trtrm_new_console();

int test_get_byte_nonblk();
int test_get_byte_nonblk2();
int test_getc_blkio();
int test_read();
int test_trtyo_namei();
int test_opr();


int main(int argc, char** argv) {
	int errcnt = 0;
	
	trtty_init();

	errcnt += test_get_byte_nonblk();
	
	errcnt += test_get_byte_nonblk2();
	//trind_init();
	
	//errcnt += test_trtyo_namei();
	
	//errcnt += test_opr();
	
	if (errcnt)
		return -1;
		
	puts("OK - test passed.");
	return 0;
}

// read_mode = READ_CHAR test
int test_get_byte_nonblk() {
	int errcnt = 0;
	char buff[80];

	trtty_open(0);
	
	trtty_fifoput(0, 0x1e);	// press 'a'
	trtty_fifoput(0, 0x9e);	// release 'a'
	trtty_fifoput(0, 0x30);	// press 'b'
	trtty_fifoput(0, 0xb0);	// release 'a'

	//buff[0] = trtty_getc_nonblk(0);
	buff[0] = trtty_get_byte_nonblk(0, TRTTY_READ_CHAR);
	buff[1] = 0;
	
	if (buff[0] != 'a') {
		fprintf(stderr, "Error - trtty_getc_nonblk()=%#x:[%s]\n", buff[0], buff);
		errcnt++;
	}
	
	//buff[0] = trtty_getc_nonblk(0);
	buff[0] = trtty_get_byte_nonblk(0, TRTTY_READ_CHAR);
	
	if (buff[0] != 'b') {
		fprintf(stderr, "Error - trtty_getc_nonblk()=%#x:[%s]\n", buff[0], buff);
		errcnt++;
	}
	
	//buff[0] = trtty_getc_nonblk(0);
	buff[0] = trtty_get_byte_nonblk(0, TRTTY_READ_CHAR);
	if (buff[0] != 0) {
		fprintf(stderr, "Error - trtty_getc_nonblk()=%#x:[%s]\n", buff[0], buff);
		errcnt++;
	}
	trtty_close(0);
	
	return errcnt;
}

// read_mode = READ_RAW test
int test_get_byte_nonblk2() {
	int errcnt = 0;
	unsigned char buff[80];

	trtty_open(0);
	
	trtty_fifoput(0, 0x1e);	// press 'a'
	trtty_fifoput(0, 0x9e);	// release 'a'
	trtty_fifoput(0, 0x30);	// press 'b'

	buff[0] = trtty_get_byte_nonblk(0, TRTTY_READ_RAW);
	buff[1] = 0;
	
	if (buff[0] != 0x1e) {
		fprintf(stderr, "Error - trtty_getc_nonblk()=%#x:\n", buff[0]);
		errcnt++;
	}
	
	buff[0] = trtty_get_byte_nonblk(0, TRTTY_READ_RAW);
	
	if (buff[0] != 0x9e) {
		fprintf(stderr, "Error - trtty_getc_nonblk()=%#x\n", buff[0]);
		errcnt++;
	}
	
	buff[0] = trtty_get_byte_nonblk(0, TRTTY_READ_RAW);
	if (buff[0] != 0x30) {
		fprintf(stderr, "Error - trtty_getc_nonblk()=%#x\n", buff[0]);
		errcnt++;
	}
	
	buff[0] = trtty_get_byte_nonblk(0, TRTTY_READ_RAW);
	if (buff[0] != 0) {
		fprintf(stderr, "Error - trtty_getc_nonblk()=%#x\n", buff[0]);
		errcnt++;
	}

	trtty_close(0);
	
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
