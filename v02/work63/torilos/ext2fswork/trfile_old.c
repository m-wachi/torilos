#include "trfile_old.h"

static TFL_HND_FILE ml_filetbl[TFL_MAX_FILETBL];
static TE2FS_HND_FS* ml_hnd_fs;

static TBLBF_BLKCELL* fancy_get_blkcell(unsigned int blkno);

int tfl_open(char* filename, unsigned char* dirent_blk, 
			 struct ext2_inode* inodetbl, TFL_HND_FILE** user_fdtbl) {

	TFL_HND_FILE* file;
	unsigned int new_inono;
	struct ext2_inode* p_inode;
	struct ext2_dir_entry_2* p_entry;
	int i;
	
	//puts("tfl_open start.");

	file = tfl_alloc_filetbl();

	//puts("tfl_open: tfl_alloc_filetbl done.");

	//printf("ml_hnd_fs=%#x\n", ml_hnd_fs);

	new_inono = te2fs_alc_new_inono(ml_hnd_fs);

	//puts("tfl_open: te2fs_alc_new_inono(ml_hnd_fs) done.");

	p_entry = te2fs_alloc_dirent(ml_hnd_fs, dirent_blk, new_inono, 
								 EXT2_FT_REG_FILE, filename);

	p_inode = &inodetbl[new_inono-1];
	p_inode->i_mode = 0x8180;
	p_inode->i_uid = 0x3e8;
	p_inode->i_gid = 0x3e8;
	//p_inode->i_size = sz_file;
	p_inode->i_size = 0;
	//p_inode->i_blocks = sz_block * 2;
	p_inode->i_blocks = 0;
	p_inode->i_links_count = 1;

	file->inode = p_inode;
	file->offset = 0;
	
	for (i = 3; i < 10; i++) {
		if (!user_fdtbl[i]) {
			user_fdtbl[i] = file;
			return i;
		}
	}
	
	return -2;
	//user_fdtbl[3] = file;

	//puts("tfl_open done.");
	//return 3;
}

void tfl_close(int fd, TFL_HND_FILE** user_fdtbl) {

	tfl_free_filetbl(user_fdtbl[fd]);
	user_fdtbl[fd] = 0;

}

/*
 * WARNING cannot write data over block size.
 * cannot write data which start in the middle of block.
 */
int tfl_write(int fd, char* buf, unsigned int size, 
			  TFL_HND_FILE** user_fdtbl) {

	TFL_HND_FILE* file;
	struct ext2_inode* p_inode;
	int blocks;
	TBLBF_BLKCELL* blkcell;

	//puts("tfl_write start.");

	if (size > 1024) {
		return 0;
	}

	file = user_fdtbl[fd];
	//blocks = file->offset / 1024;
	blocks = file->offset / ml_hnd_fs->sz_block;
	p_inode = file->inode;
	blkcell = tfl_append_newblk(p_inode, blocks, ml_hnd_fs);
	if(!blkcell) {
		return 0;
	}
	p_inode->i_blocks += 2;

	p_inode->i_size += size;

	my_memcpy(blkcell->blk, buf, size);
	blkcell->dirty = 1;
	
	file->offset += size;

	//puts("tfl_write done.");
	return size;

}
//TODO This function should be replaced with tflm_append_newblk
TBLBF_BLKCELL* tfl_append_newblk(struct ext2_inode* p_inode, int blk_index,
								   TE2FS_HND_FS* hnd_fs) {
	unsigned int blkno;
	/*
	TBLBF_BLKCELL_OLD* blkcell;
	TBLBF_BLKCELL_OLD* blkcell12;
	*/
	TBLBF_BLKCELL* blkcell;
	TBLBF_BLKCELL* blkcell12;
	
	unsigned int* p_indirect_block;
	int idx_blk12, i;

	blkcell = 0;

	if (blk_index < 12) {
		blkno = te2fs_alc_new_blkno(hnd_fs);
		debug_varval01("new blkno", blkno, 1);
		p_inode->i_block[blk_index] = blkno;
		//blkcell = tblbf_get_blkcell_old(blkno);
		blkcell = fancy_get_blkcell(blkno);
	}
	else if (12 <= blk_index < 256+12) {

		if (0 == p_inode->i_block[12]) {
			// get new block for indirect block
			blkno = te2fs_alc_new_blkno(hnd_fs);
			p_inode->i_block[12] = blkno;
			//blkcell12 = tblbf_get_blkcell_old(blkno);
			blkcell12 = fancy_get_blkcell(blkno);
			p_indirect_block = (unsigned int*)blkcell12->blk;
			for (i=0; i<256; i++) p_indirect_block[i] = 0;
		} else {
			// get indirect block 
			//blkcell12 = tblbf_get_blkcell_old(p_inode->i_block[12]);
			blkcell12 = fancy_get_blkcell(p_inode->i_block[12]);
			blkcell12->dirty = 1;
			p_indirect_block = (unsigned int*)blkcell12->blk;
		}
		
		blkno = te2fs_alc_new_blkno(hnd_fs);

		// set block-no to indirect block
		p_indirect_block[blk_index - 12] = blkno;

		//blkcell = tblbf_get_blkcell_old(blkno);
		blkcell = fancy_get_blkcell(blkno);
	}
	else {
		return 0;
	}


	return blkcell;

}

static TBLBF_BLKCELL* fancy_get_blkcell(unsigned int blkno) {
	TBLBF_BLKCELL* blkcell;

	blkcell = tblbf_lookup_blkcell(blkno);
	if (blkcell) return blkcell;

	//puts("get_blkcell: alc_free_blkcell();");

	blkcell = tblbf_alc_free_blkcell();
	
	blkcell->blkno = blkno;

	//puts("get_blkcell done.");

	return blkcell;
}


void tfl_init_filetbl(TE2FS_HND_FS* hnd_fs) {
	int i;

	for (i=0; i<TFL_MAX_FILETBL; i++) {
		ml_filetbl[i].inode = 0;
		ml_filetbl[i].offset = 0;
	}

	ml_hnd_fs = hnd_fs;

}

void tfl_init_userfd(TFL_HND_FILE** user_fdtbl) {
	int i;

	for (i=0; i<TFL_MAX_USERFD; i++) {
		user_fdtbl[i] = 0;
	}

}


TFL_HND_FILE* tfl_alloc_filetbl() {
	int i;
	
	for (i=0; i<TFL_MAX_FILETBL; i++) {
		if (0 == ml_filetbl[i].inode)
			return &ml_filetbl[i];
	}
	

	return 0;
}


void tfl_free_filetbl(TFL_HND_FILE* hnd_file) {
	
	hnd_file->inode = 0;

}


