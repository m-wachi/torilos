/**
 * @file trext2fs.c
 * @brief block-bitmap, inode-bitmap and directory-entry operation module
 * 
 * !!NOTICE!!
 * block-bitmap must be started with block-no '1', and inode-bitmap too.
 * Then block-no (and inode-no) '0' is not represented in the bitmap. 
 */

#include <torilos/trext2fs.h>
#include <torilos/mylib01.h>
#include <torilos/trstat.h>

/**
 * @brief get bitmap byte-index
 * @param no block-no or inode-no (expected starting with 1)
 * @return bitmap byte-index (starting with 0)
 */
unsigned int te2fs_bindex_bmp(unsigned int no) {
	//return no / 8;
	return (no-1) / 8;
}

/**
 * @brief get bitmap bit-index
 * @param no block-no or inode-no (expected starting with 1)
 * @return bitmap bit-index (starting with 0)
 */
int te2fs_blshft_bmp(unsigned int no) {
	//return no % 8;
	return (no-1) % 8;
}

unsigned int te2fs_inode_block(unsigned int inono, 
							   unsigned int start_inode_blkno) {
	return (inono-1) / 8 + start_inode_blkno;
}

unsigned int te2fs_inode_block_offset(unsigned int inono) {
	return ((inono-1) % 8) * 128;
}


/**
 * @brief get not-allocated bitmap bit-index.
 * @param bitmap	bitmap
 * @param sz_block block-size
 * @return no (block-no or inode-no) 
 */
unsigned int te2fs_notalc_bmp(unsigned char *bitmap, int sz_block) {
	int i, j;
	unsigned char c, mask;
	unsigned int no;

	no = 0;
	for (i=0; i<sz_block; i++) {
		c = bitmap[i];
		if(0xff != c) {
			no = i*8;
			mask = 1;
			for (j=0; j<8; j++) {
				if(!(c & mask)) {
					no += j;
					break;
				}
				mask <<= 1;
			}
			break;
		}
	}
	/* we assume bitmap not to be represented no '0' */ 
	return no + 1;

}

/**
 * @brief set bit on the bitmap.
 * @param bitmap   bitmap
 * @param no       no (start with 0)
 * @param bit      1: ON, 0: OFF
 */
void te2fs_setbit_bmp(unsigned char *bitmap, unsigned int no, int bit) {
	unsigned char* p;
	int idx;
	int lsht;
	unsigned char mask;

	idx = te2fs_bindex_bmp(no);

	lsht = te2fs_blshft_bmp(no);
	
	p = bitmap + idx;
	
	
	if(bit) {
		mask = 1 << lsht;
		*p |= mask;
	} else {
		mask = 1 << lsht;
		mask = ~mask;
		*p &= mask;
	}
}


/**
 * @brief get not-allocated inode no.
 */
unsigned int te2fs_notalc_inono(TE2FS_HND_FS* hnd_fs) {
	//return te2fs_notalc_bmp(hnd_fs->inobmp, hnd_fs->sz_block) + 1;
	return te2fs_notalc_bmp(hnd_fs->inobmp, hnd_fs->sz_block);
}

/*
 * get not-allocated block no.
 */
unsigned int te2fs_notalc_blkno(TE2FS_HND_FS* hnd_fs) {
	return te2fs_notalc_bmp(hnd_fs->blkbmp, hnd_fs->sz_block);
}

/**
 * @brief allocate new inode-no
 * @param hnd_fs
 * @return new inode-no
 */
unsigned int te2fs_alc_new_inono(TE2FS_HND_FS* hnd_fs) {
	unsigned int inono;

	inono = te2fs_notalc_inono(hnd_fs);
	//te2fs_setbit_bmp(hnd_fs->inobmp, inono-1, 1);
	te2fs_setbit_bmp(hnd_fs->inobmp, inono, 1);
	return inono;
}

/*
 * free inode-no
 */
void te2fs_free_inono(TE2FS_HND_FS* hnd_fs, unsigned int inono) {

	te2fs_setbit_bmp(hnd_fs->inobmp, inono, 0);
}

/**
 * @brief allocate new block-no on block-bitmap
 * 
 * @param hnd_fs
 * @return allocated new block-no
 */
unsigned int te2fs_alc_new_blkno(TE2FS_HND_FS* hnd_fs) {
	unsigned int blkno;

	blkno = te2fs_notalc_blkno(hnd_fs);
	te2fs_setbit_bmp(hnd_fs->blkbmp, blkno, 1);
	return blkno;
}

/*
 * free block-no
 */
void te2fs_free_blkno(TE2FS_HND_FS* hnd_fs, unsigned int blkno) {

	te2fs_setbit_bmp(hnd_fs->blkbmp, blkno, 0);
}


/*
 * get required blocksize from filesize.
 */
int te2fs_block_size(TE2FS_HND_FS* hnd_fs, int sz_file) {
	int ret;

	ret = sz_file / hnd_fs->sz_block;
	if(sz_file % hnd_fs->sz_block)
		ret++;

	return ret;
}

/**
 * @brief alloc new directory-entry
 * @param hnd_fs
 * @param dirent_blk directory-entry block
 * @param inono      new entry inode-no
 * @param file_type  new entry filetype
 * @param filename   new entry filename
 * @return allocated new directory-entry, 0 if no availabel entry
 */
struct ext2_dir_entry_2*
te2fs_alloc_dirent(TE2FS_HND_FS* hnd_fs, unsigned char* dirent_blk,
				   unsigned int inono, 
				   unsigned char file_type,
				   char* filename) {

	struct ext2_dir_entry_2* p_entry;
	struct ext2_dir_entry_2* p_new_entry;
	unsigned int sz_new_fn;
	int sz_old_rec_len;
	int sz_min_cur_ent;
	int sz_new_ent;

	p_entry = (struct ext2_dir_entry_2*)dirent_blk;
	
	sz_new_fn = my_strlen(filename);
	sz_new_ent = te2fs_minsize_dirent(sz_new_fn);
	while (p_entry) {

		sz_min_cur_ent = te2fs_minsize_dirent(p_entry->name_len);

		if (p_entry->rec_len >= (sz_min_cur_ent + sz_new_ent)) {
			sz_old_rec_len = p_entry->rec_len;
			p_entry->rec_len = sz_min_cur_ent;
			p_new_entry = te2fs_next_dirent(p_entry, dirent_blk, 
											hnd_fs->sz_block);
			p_new_entry->inode = inono;
			p_new_entry->rec_len = sz_old_rec_len - sz_min_cur_ent;
			p_new_entry->name_len = sz_new_fn;
			p_new_entry->file_type = file_type;
			my_strcpy(p_new_entry->name, filename);
			return p_new_entry;
		}
			
		p_entry = te2fs_next_dirent(p_entry, dirent_blk, hnd_fs->sz_block);


	}
	return 0;
}

/*
 * free directory-entry
 * 
 * @hnd_fs
 * @filename filename to set free
 * @return   inode-no set free
 */
unsigned int te2fs_free_dirent(TE2FS_HND_FS* hnd_fs, 
							   unsigned char* dirent_blk, char* filename) {
	struct ext2_dir_entry_2* p_cur_ent;
	struct ext2_dir_entry_2* p_prev_ent;

	p_prev_ent = 0;
	p_cur_ent = (struct ext2_dir_entry_2*)dirent_blk;

	while (p_cur_ent) {
		if (!my_strcmp(filename, p_cur_ent->name)) {
			p_prev_ent->rec_len += p_cur_ent->rec_len;
			return p_cur_ent->inode;
		}		
		p_prev_ent = p_cur_ent;
		p_cur_ent = te2fs_next_dirent(p_cur_ent, dirent_blk, hnd_fs->sz_block);
	}
	return 0;

}


/*
 * get minimum directory-entry size
 * (must be a multiple of 4)
 */
int te2fs_minsize_dirent(int sz_filename) {
	
	int min, min4;
	
	// 4: inode size
	// 2: rec_len size
	// 2: name_len + file_type
	// 1: string terminator ('\0')
	min = sz_filename + 9;

	min4 = (min + 3) & 0xfffffffc;

	return min4;
	
}

/**
 * @brief lookup directory-entry
 * @param filename entry name looking up 
 * @param dirent_blk directory-entry block
 * @param sz_block block size
 * @return directory-entry. 0 if filename coundn't be found.
 */
struct ext2_dir_entry_2* 
te2fs_lookup_dirent(char* filename, unsigned char* dirent_block, 
			  unsigned int sz_block) {

	struct ext2_dir_entry_2* p_entry;
	int file_name_len;
	
	p_entry = (struct ext2_dir_entry_2*)dirent_block;
	file_name_len = my_strlen(filename);
	while(p_entry) {
		if (file_name_len == p_entry->name_len) {
			if (!my_strncmp(p_entry->name, filename, p_entry->name_len))
				return p_entry;
		}
		p_entry = te2fs_next_dirent(p_entry, dirent_block, sz_block);
	}		
	return 0;
}


/**
 * @brief get next directory-entry 
 * @param p_entry		current directory-entry
 * @param dirent_block	directory-entry block
 * @param sz_block		block size
 * @return		!=0: next directory-entry, =0: the end of entry
 */
struct ext2_dir_entry_2* 
te2fs_next_dirent(struct ext2_dir_entry_2* p_entry, 
				  unsigned char* dirent_block, unsigned int sz_block) {
	
	unsigned char* p;

	p = (unsigned char*)p_entry;
	p += p_entry->rec_len;
	if (p >= dirent_block + sz_block)
		return 0;

	return (struct ext2_dir_entry_2*)p;

}

/**
 * @brief check if file is directory
 * @p_inode ext2-inode
 * @return 0: not director, 1: directory
 */
int te2fs_isdir(struct ext2_inode* p_inode) {
	
	if (p_inode->i_mode & S_IFDIR) {
		return 1;
	}
	return 0;
	
}
