#ifndef TRFILE_H
#define TRFILE_H

#include "trext2fs.h"
#include "trblkbuf.h"
#include "trinode.h"

#define TFL_MAX_FILETBL		128
#define TFL_MAX_USERFD		16
#define TFL_MAX_PATHSTR		256

//These lines are defined in fcntl.h
///* from newlib sys/fcntl.h */
//#define	O_RDONLY	0		/* +1 == FREAD */
//#define	O_WRONLY	1		/* +1 == FWRITE */
//#define	O_RDWR		2		/* +1 == FREAD|FWRITE */
//#define	O_CREAT	0x0200
//#define	O_NONBLOCK	0x4000	/* non blocking I/O (POSIX style) */
//
//#define	O_ACCMODE	(O_RDONLY|O_WRONLY|O_RDWR)

/* from newlib sys/unistd.h */
# define	SEEK_SET	0
# define	SEEK_CUR	1
# define	SEEK_END	2


typedef struct tfl_filetbl_item {
	TR_INODE* tr_inode;
	struct ext2_inode* ext2_inode;
	long offset;
	int ref_count;	//!< reference count
	unsigned int* p_dirty_inoblk;	//!< inode block dirty flag pointer 
} TFL_HND_FILE;

void tfl_close(int fd, TFL_HND_FILE** user_fdtbl);
void tfl_init_filetbl(TE2FS_HND_FS* hnd_fs);
void tfl_init_userfd(TFL_HND_FILE** user_fdtbl);
TFL_HND_FILE* tfl_alloc_filetbl();
void tfl_free_filetbl(TFL_HND_FILE* filetbl);
int tfl_alloc_file_usertbl(TFL_HND_FILE** user_fdtbl, TFL_HND_FILE* hnd_file);
int tfl_dup(TFL_HND_FILE** user_fdtbl, int oldfd);
int tfl_make_abspath(char* abspath_buf, char* filepath, char* cwdpath);
void tfl_remove_period_exp(char* path);
int tfl_is_period_exp(char* s);
int tfl_copy_until_slash(char* dst, char*src);
char* tfl_forward_after_slash(char* p);
char* tfl_backward_prev_slash(char* p, char* p_head);
void tfl_strip_tail_slash(char* filepath);
void tfl_dirpath_file(char* dirpath, char* file, char* abs_path);
#endif
