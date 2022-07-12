/*
 * 
 * $Rev: 196 $
 * $Author: m-wachi $
 */

#ifndef IO_H
#define IO_H

#define IOMODE_MEMORY 	0
#define IOMODE_FILE		1
#define IOMODE_STDIN		2

typedef struct hnd_io {
	int mode;		
	char *src;	//memory-mode src top string pointer
	char last_read;
	char *p;		//memory-mode src current char pointer
	FILE* fp;		//file-mode src file pointer
} HND_IO;

void setup_memory_reader(char* s);
int setup_file_reader(char* filename);
int setup_stdin_reader();
char io_read_next();
char io_read_next_memory();
char io_read_next_file();
char io_peek();
void io_close();
void io_close_memory();
void io_close_file();

#endif
