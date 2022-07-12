#ifndef TRFILE_OPR_H
#define TRFILE_OPR_H
#include <torilos/trfile.h>
#include <torilos/trstat.h>

int tflop_open(char* filename, int flags, TFL_HND_FILE** user_fdtbl, char* cwd);
int tflop_read(int fd, char* buf, unsigned int count, 
		TFL_HND_FILE** user_fdtbl);
int tflop_write(int fd, char* buf, unsigned int count, 
			  TFL_HND_FILE** user_fdtbl);
int tflop_close(int fd, TFL_HND_FILE** user_fdtbl);
int tflop_lseek(int fd, long offset, int whence, 
		TFL_HND_FILE** user_fdtbl);
int tflop_fstat(int fd, struct stat *statbuf, TFL_HND_FILE** user_fdtbl);
int tflop_ioctl(int fd, int cmd, int param1, TFL_HND_FILE** user_fdtbl);

#endif

