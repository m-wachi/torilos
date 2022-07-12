/*
 * $Author: m-wachi $
 * $Rev: 204 $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trscheme_config.h"
#include "objects.h"
#include "io.h"
#include "test_io.h"

int test_io_main() {
	int errcnt=0;
	
	errcnt += test_io_memory();
	
	errcnt += test_io_file();
	
	return errcnt; 
}

int test_io_memory() {
	int errcnt=0;
	
	setup_memory_reader("hello");

	errcnt = test_io_core("memory");
	
	io_close();
	
	return errcnt;
	
}

int test_io_file() {
	char filename1[] = "autotest.scm";  
	FILE* fptest;
	int errcnt=0;
	int rc;
	
	fptest = fopen(filename1, "w");
	
	fputs("hello", fptest);
	
	fclose(fptest);
	
	rc = setup_file_reader(filename1);
	if (rc != 0) {
		fprintf(stderr, "Failed - setup_file_reader(): rc=%d, expect 0\n", rc);
		errcnt++; 
	}
	errcnt += test_io_core("file");
	
	io_close();
	
	rc = setup_file_reader("not-exist-file.scm");
	if (rc != -1) {
		fprintf(stderr, "Failed - setup_file_reader(): rc=%d, expect -1\n", rc);
		errcnt++; 
	}
	return errcnt;
}

int test_io_core(char* type) {
	int errcnt=0, i;
	char c;
	
	c = io_peek();
	if (c != 'h') {
		fprintf(stderr, "Failed - %s/io_peek(): 1st read rc=%c\n", type, c);
		errcnt++;
	}
	
	c = io_read_next();
	if (c != 'e') {
		fprintf(stderr, "Failed - %s/io_read_char(): 2nd read rc=%c\n", type, c);
		errcnt++;
	}

	for (i=0; i<4; i++) {
		c = io_read_next();
	}
	if (c) {
		fprintf(stderr, "Failed - %s/io_read_char(): 6th read rc=%c\n", type, c);
		errcnt++;
	}
	
	c = io_read_next();
	if (c) {
		fprintf(stderr, "Failed - %s/io_read_char(): 7th read rc=%c\n", type, c);
		errcnt++;
	}
	
	return errcnt;
	
}
