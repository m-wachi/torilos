#ifndef TRFILE_H
#define TRFILE_H

#include <torilos/trext2fs.h>
#include <torilos/trblkbuf.h>

#define TFL_MAX_FILETBL   128
#define TFL_MAX_USERFD    16

/* from newlib sys/fcntl.h */
#define	O_RDONLY	0		/* +1 == FREAD */
#define	O_WRONLY	1		/* +1 == FWRITE */
#define	O_RDWR		2		/* +1 == FREAD|FWRITE */
#define	O_CREAT	0x0200

typedef struct tfl_filetbl_item {
	struct ext2_inode* inode;
	long offset;
} TFL_HND_FILE;

int tfl_open(char* filename, unsigned char* dirent_blk, 
			 struct ext2_inode* inodetbl, TFL_HND_FILE** user_fdtbl);
void tfl_init_userfd(TFL_HND_FILE** user_fdtbl);
TFL_HND_FILE* tfl_alloc_filetbl();
void tfl_free_filetbl(TFL_HND_FILE* filetbl);
TBLBF_BLKCELL* tfl_append_newblk(struct ext2_inode* p_inode, int blk_index,
								 TE2FS_HND_FS* hnd_fs);
void tfl_init_filetbl(TE2FS_HND_FS* hnd_fs);
void tfl_close(int fd, TFL_HND_FILE** user_fdtbl);

#endif
