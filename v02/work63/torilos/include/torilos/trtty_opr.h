#ifndef TRTTY_OPR_H
#define TRTTY_OPR_H

#include <torilos/trinode.h>
#include <torilos/trfile.h>

#define TRTYO_NORMAL_MODE 6
#define TRTYO_EDITOR_MODE 7

int trtyo_open(char* filename, int flags, TFL_HND_FILE** user_fdtbl);
TR_INODE* trtyo_namei(char* s_path);
int trtyo_read(int fd, char* buf, unsigned int sz, 
		TFL_HND_FILE** user_fdtbl);
int trtyo_write(int fd, char* buf, unsigned int size, 
			  TFL_HND_FILE** user_fdtbl);
void trtyo_close(int fd, TFL_HND_FILE** user_fdtbl);
int trtyo_inono2ttyno(unsigned int inono);
int trtyo_ioctl(TFL_HND_FILE* hnd_file, int cmd, int param1);

#endif
