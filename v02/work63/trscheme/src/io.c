/*
 * $Author: m-wachi $
 * $Rev: 204 $
 */
//aaaa
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trscheme_config.h"
#include "objects.h"

#include "io.h"

static HND_IO ml_hnd_io;

void setup_memory_reader(char* s) {
	ml_hnd_io.mode = IOMODE_MEMORY;
	ml_hnd_io.src = malloc(strlen(s));
	ml_hnd_io.last_read = 0;
	strcpy(ml_hnd_io.src, s);
	ml_hnd_io.p = ml_hnd_io.src;
	//read forward
	io_read_next();
}
/**
 * @brief file-reader setup
 * @param filename 
 * @return 0: success, -1: error
 */
int setup_file_reader(char* filename) {
	ml_hnd_io.mode = IOMODE_FILE;			
	ml_hnd_io.fp = fopen(filename, "r");
	if (!ml_hnd_io.fp)
		return -1;
	ml_hnd_io.last_read = 0;	
	//read forward
	io_read_next();
	return 0;
}

/**
 * @brief stdin-reader setup
 * @return 0: success, -1: error
 */
int setup_stdin_reader() {
	ml_hnd_io.mode = IOMODE_STDIN;			
	ml_hnd_io.fp = stdin;
	if (!ml_hnd_io.fp)
		return -1;
	ml_hnd_io.last_read = 0;	
	//read forward
	io_read_next();
	return 0;
}

char io_read_next() {
	char rc = 0;
	switch (ml_hnd_io.mode) {
	case IOMODE_MEMORY:
		rc = io_read_next_memory();
		break;
	case IOMODE_FILE:
	case IOMODE_STDIN:
		rc = io_read_next_file();
		break;
	}
	ml_hnd_io.last_read = rc;
	
	return rc;
}

char io_read_next_memory() {
	char rc;
	if (!*ml_hnd_io.p)
		return 0;
	rc = *ml_hnd_io.p++; 
	return rc;
}

char io_read_next_file() {
	char rc;
	
#ifdef TORILOS
	if (ml_hnd_io.fp == stdin) {
		read(0, &rc, 1);
		write(1, &rc, 1);
	} else { 
		rc = fgetc(ml_hnd_io.fp);
	}
#else
	rc = fgetc(ml_hnd_io.fp);
#endif
	if (EOF == rc) 
		rc = 0;
	
	return rc;
}

char io_peek() {
	return ml_hnd_io.last_read;
}

void io_close() {
	switch (ml_hnd_io.mode) {
	case IOMODE_MEMORY:
		io_close_memory();
		break;
	case IOMODE_FILE:
		io_close_file();
		break;
	}
}

void io_close_memory() {
	free(ml_hnd_io.src);
	ml_hnd_io.src = 0;
	ml_hnd_io.p = 0;
}

void io_close_file() {
	fclose(ml_hnd_io.fp);
}
