#ifndef TRFILEMDL_H
#define TRFILEMDL_H

#include "trext2fs.h"
#include "trfile.h"
#include "trblkbuf.h"
#include "trext2fsmdl.h"


void tflm_init_filetbl(TE2FM_HND_FSMDL* hnd_fs);

/*
int tflm_create(char* filename, unsigned char* dirent_blk, 
			 TFL_HND_FILE** user_fdtbl);
int tflm_create2(char* filename, TFL_HND_FILE** user_fdtbl);
*/
unsigned int tflm_read(int fd, unsigned char* buff, unsigned int count, 
		TFL_HND_FILE** user_fdtbl);

int tflm_unlink(char* filename, unsigned char* dirent_blk);
int tflm_close(int fd, TFL_HND_FILE** user_fdtbl);
struct ext2_inode* tflm_get_inode(char* filename, unsigned char* dirent_blk);
struct ext2_inode* tflm_get_inode_by_dirent(struct ext2_dir_entry_2* p_entry);
struct ext2_inode* tflm_get_inode_by_no(unsigned int** pp_dirty_inoblk, 
		unsigned int inono);
unsigned int tflm_get_size(int fd, TFL_HND_FILE** user_fdtbl);

void tflm_sync();
int tflm_write(int fd, char* buf, unsigned int size, 
			  TFL_HND_FILE** user_fdtbl);
TBLBF_BLKCELL* tflm_getblk_by_inode_index(struct ext2_inode* p_inode,	
		unsigned int idx_block, int mode);

unsigned char* tflm_prepare_filesystem(TE2FM_HND_FSMDL* hnd_fsmdl);

void print_direntry(unsigned char* block);
void dump_direntry(struct ext2_dir_entry_2* p_entry);
void dump_inode(struct ext2_inode* p_inode, unsigned int inode_no);
#endif
