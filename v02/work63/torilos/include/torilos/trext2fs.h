#ifndef TREXT2FS_H
#define TREXT2FS_H

#include "ext2fs.h"

typedef struct te2fs_hnd {
	int sz_block;
	unsigned char* blkbmp;
	unsigned char* inobmp;
	unsigned int start_inode_blkno;
} TE2FS_HND_FS;

typedef struct te2fs_file {
	struct ext2_dir_entry_2* p_dirent;
	struct ext2_inode* p_inode;
	unsigned char* blk_content[16];
} TE2FS_FILE;

unsigned int te2fs_bindex_bmp(unsigned int no);
int te2fs_blshft_bmp(unsigned int no);
unsigned int te2fs_inode_block(unsigned int inono, 
							   unsigned int start_inode_blkno);
unsigned int te2fs_inode_block_offset(unsigned int inono);
void te2fs_setbit_bmp(unsigned char *bitmap, unsigned int no, int bit);
unsigned int te2fs_notalc_bmp(unsigned char *bitmap, int sz_block);
unsigned int te2fs_notalc_inono(TE2FS_HND_FS* hnd_fs);
unsigned int te2fs_notalc_blkno(TE2FS_HND_FS* hnd_fs);
unsigned int te2fs_alc_new_inono(TE2FS_HND_FS* hnd_fs);
void te2fs_free_inono(TE2FS_HND_FS* hnd_fs, unsigned int inono);
int te2fs_block_size(TE2FS_HND_FS* hnd_fs, int sz_file);
unsigned int te2fs_alc_new_blkno(TE2FS_HND_FS* hnd_fs);
void te2fs_free_blkno(TE2FS_HND_FS* hnd_fs, unsigned int blkno);
struct ext2_dir_entry_2* 
te2fs_lookup_dirent(char* filename, 
					  unsigned char* dirent_block, 
					  unsigned int sz_block);

struct ext2_dir_entry_2* 
te2fs_next_dirent(struct ext2_dir_entry_2* p_entry, 
				  unsigned char* dirent_block, 
				  unsigned int sz_block);
	
int te2fs_minsize_dirent(int sz_filename);

struct ext2_dir_entry_2*
te2fs_alloc_dirent(TE2FS_HND_FS* hnd_fs, unsigned char* dirent_blk,
				   unsigned int inono, unsigned char file_type,
				   char* filename);

unsigned int te2fs_free_dirent(TE2FS_HND_FS* hnd_fs, 
							   unsigned char* dirent_blk, char* filename);

int te2fs_isdir(struct ext2_inode* p_inode);
#endif
