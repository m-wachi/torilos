/**
 * @file trfilemdl.c
 * @brief file operation module (middle layer)
 */
#include <torilos/trfilemdl.h>
#include <torilos/mylib01.h>
#include <torilos/trutil.h>
#include <torilos/debug01.h>
#include <torilos/trblkbufmdl.h>
#include <torilos/trataiomdl.h>
#include <torilos/trext2fs.h>
#include <torilos/trext2fsmdl.h>
#include <torilos/trinode.h>
#include <torilos/trmajor.h>

static TE2FM_HND_FSMDL* ml_hnd_fsmdl;

void tflm_init_filetbl(TE2FM_HND_FSMDL* hnd_fsmdl) {

	//ml_hnd_fs = hnd_fs;
	ml_hnd_fsmdl = hnd_fsmdl;
	
	tfl_init_filetbl(&hnd_fsmdl->hnd_fs);
	
}

/**
 * @brief "create" procedure for block device
 * 
 * @param filename
 * @param dirent_blk
 * @param user_fdtbl
 * @return 0: success, error if others
 */
/*
int tflm_create(char* filename, unsigned char* dirent_blk, 
			 TFL_HND_FILE** user_fdtbl) {

	TFL_HND_FILE* file;
	unsigned int new_inono;
	struct ext2_inode* p_inode;
	struct ext2_dir_entry_2* p_entry;
	int i;
	unsigned int inode_blkno, inode_blkoff;
	TBLBF_BLKCELL* inode_blkcell;

	p_entry = te2fs_lookup_dirent(filename, dirent_blk, 
			ml_hnd_fsmdl->hnd_fs.sz_block);
	if(p_entry) return -1;
	
	file = tfl_alloc_filetbl();

	//new_inono = te2fs_alc_new_inono(&ml_hnd_fsmdl->hnd_fs);
	new_inono = te2fm_alc_new_inono(ml_hnd_fsmdl);

	p_entry = te2fs_alloc_dirent(&ml_hnd_fsmdl->hnd_fs, dirent_blk, new_inono, 
								 EXT2_FT_REG_FILE, filename);

	if (!p_entry) {
		return -3;
	}
	
	//TODO use tflm_get_inode_by_dirent()
	inode_blkno = te2fs_inode_block(p_entry->inode, 
			ml_hnd_fsmdl->hnd_fs.start_inode_blkno); 
	inode_blkoff = te2fs_inode_block_offset(p_entry->inode);

	inode_blkcell = tblbm_get_blkcell(inode_blkno);
	if (!inode_blkcell) {
		return -2;
	}
	inode_blkcell->dirty = 1;
	
	p_inode = (struct ext2_inode*) (inode_blkcell->blk + inode_blkoff);

	
	p_inode->i_mode = 0x8180;
	p_inode->i_uid = 0x3e8;
	p_inode->i_gid = 0x3e8;
	p_inode->i_size = 0;
	p_inode->i_blocks = 0;
	p_inode->i_links_count = 1;

	
	for(i=0; i<EXT2_N_BLOCKS; i++)
		p_inode->i_block[i] = 0;
	
	ml_hnd_fsmdl->rootdir_blkcell->dirty = 1;
	
	file->ext2_inode = p_inode;
	file->offset = 0;

	file->tr_inode = trind_alc_free_inode();
	file->tr_inode->inono = p_entry->inode;
	file->tr_inode->log_dev_no = HD_MAJOR;
	file->tr_inode->i_mode = p_inode->i_mode;
	file->ref_count = 1;
	
	for (i = 3; i < 10; i++) {
		if (!user_fdtbl[i]) {
			user_fdtbl[i] = file;
			return i;
		}
	}
	
	return -2;
}

int tflm_create2(char* filename, TFL_HND_FILE** user_fdtbl) {
	return tflm_create(filename, ml_hnd_fsmdl->rootdir_blkcell->blk, user_fdtbl);
}
*/
/**
 * @brief "read" procedure for block device
 * @param fd
 * @param buff
 * @param count
 * @param user_fdtbl
 * @return read size
 */
unsigned int tflm_read(int fd, unsigned char* buff, unsigned int count, 
		TFL_HND_FILE** user_fdtbl) {

	TFL_HND_FILE* file;
	TBLBF_BLKCELL* blkcell;
	TBLBF_BLKCELL* indirect_blkcell;
	//unsigned int* p_indirect_block;
	unsigned int sz_read, sz_rest, blk_off, sz_total_read;
	int idx_block;
	unsigned char* p_start_src;
	unsigned char* p_start_dst;
	
	//char buff2[200], buff3[128];
	
	indirect_blkcell = 0;
	
	file = user_fdtbl[fd];

	if (file->ext2_inode->i_size <= file->offset) {
		return 0;
	}
	
	idx_block = file->offset / ml_hnd_fsmdl->hnd_fs.sz_block;
	blk_off = file->offset % ml_hnd_fsmdl->hnd_fs.sz_block;

	/*
	sprint_varval(buff2, "idx_block", idx_block, 0);
	sprint_varval(buff3, ", blk_off", blk_off, 1);
	my_strcat(buff2, buff3);
	debug_puts(buff2);
	
	sprint_varval(buff2, "i_size", file->ext2_inode->i_size, 1);
	sprint_varval(buff3, ", offset", file->offset, 1);
	my_strcat(buff2, buff3);
	sprint_varval(buff3, ", count", count, 1);
	my_strcat(buff2, buff3);
	debug_puts(buff2);
	*/
	
	sz_total_read = 0;
	
	if (file->ext2_inode->i_size < (file->offset + count)) {
		sz_rest = file->ext2_inode->i_size - file->offset;
	} else {
		sz_rest = count;
	}
	
	p_start_dst = buff;
	
	while(sz_rest) {
		blkcell = tflm_getblk_by_inode_index(file->ext2_inode, idx_block, 0);
		
		p_start_src = blkcell->blk;
		p_start_src += blk_off;
		
		if (blk_off + sz_rest > ml_hnd_fsmdl->hnd_fs.sz_block) {
			sz_read = ml_hnd_fsmdl->hnd_fs.sz_block - blk_off;
			sz_rest -= sz_read;
			blk_off = 0; idx_block++;
		} else {
			sz_read = sz_rest;
			sz_rest = 0;
		}
		
		my_memcpy(p_start_dst, p_start_src, sz_read);
		p_start_dst += sz_read;
		p_start_src += sz_read;

		file->offset += sz_read;
		
		sz_total_read += sz_read;

	}
	
	return sz_total_read;
	
}

int tflm_unlink(char* filename, unsigned char* dirent_blk) {

	unsigned int inono;
	struct ext2_inode* p_inode;
	int i;
	
	p_inode = tflm_get_inode(filename, dirent_blk);
	
	//TODO clear over 10 blocks
	for (i=0; i<10; i++) {
		if (p_inode->i_block[i]) {
			//te2fs_free_blkno(&ml_hnd_fsmdl->hnd_fs, p_inode->i_block[i]);
			te2fm_free_blkno(ml_hnd_fsmdl, p_inode->i_block[i]);
			p_inode->i_block[i] = 0;
		}
	}
	
	p_inode->i_mode = 0;
	p_inode->i_uid = 0;
	p_inode->i_gid = 0;
	p_inode->i_size = 0;
	p_inode->i_blocks = 0;
	p_inode->i_links_count = 0;
	
	// free directory-entry
	inono = te2fs_free_dirent(&ml_hnd_fsmdl->hnd_fs, dirent_blk, filename);

	// free inode-no from inode-bitmap
	te2fm_free_inono(ml_hnd_fsmdl, inono);

	ml_hnd_fsmdl->rootdir_blkcell->dirty = 1;
	
	return 0;
}

/**
 * @brief "close" procedure for block device
 * @param fd
 * @param user_fdtbl 
 * @return 0: success
 */
int tflm_close(int fd, TFL_HND_FILE** user_fdtbl) {
	int rc, i;
	struct ext2_inode* p_inode;
	TBLBF_BLKCELL* blkcell;
	TBLBF_BLKCELL* blkcell12;
	unsigned int* p_indirect;
	TFL_HND_FILE* file;
	
	file = user_fdtbl[fd];
	p_inode = file->ext2_inode;
	
	for (i=0; i<12; i++) {
		if (p_inode->i_block[i]) {
			blkcell = tblbf_lookup_blkcell(p_inode->i_block[i]);
			if (blkcell) tblbf_free_blkcell(blkcell);
		}
	}
	
	if (p_inode->i_block[12]) {
		blkcell12 = tblbf_lookup_blkcell(p_inode->i_block[12]);
		if (blkcell12) {
			p_indirect = (unsigned int*)blkcell12->blk;
			for (i=0; i<256; i++) {
				blkcell = tblbf_lookup_blkcell(*p_indirect);
				if (blkcell) tblbf_free_blkcell(blkcell);
				p_indirect++;
			}
			tblbf_free_blkcell(blkcell12);
		}
	}

	//TODO free double indirect blocks

	trind_free_inode(file->tr_inode);
	file->tr_inode = 0;
	
	rc = 0;
	tfl_close(fd, user_fdtbl);
	return rc;
}

struct ext2_inode* tflm_get_inode(char* filename, unsigned char* dirent_blk) {

	struct ext2_dir_entry_2* p_entry;
	unsigned int inode_blkno;
	unsigned int inode_blkoff;
	TBLBF_BLKCELL* blkcell;
	
	p_entry = te2fs_lookup_dirent(filename, dirent_blk, 
			ml_hnd_fsmdl->hnd_fs.sz_block);

	if (!p_entry)
		return 0;	//file not found.

	inode_blkno = te2fs_inode_block(p_entry->inode, 
			ml_hnd_fsmdl->hnd_fs.start_inode_blkno); 
	
	inode_blkoff = te2fs_inode_block_offset(p_entry->inode);

	blkcell = tblbm_get_blkcell(inode_blkno);

	if (!blkcell) {
		return 0;	
	}
	
	return (struct ext2_inode*) (blkcell->blk + inode_blkoff);
	
}

//TODO replace tflm_get_inode_by_no()
/**
 * @brief get inode by directory-entry
 * @param p_entry directory-entry
 * @return inode
 */
struct ext2_inode* tflm_get_inode_by_dirent(struct ext2_dir_entry_2* p_entry) {

	unsigned int inode_blkno;
	unsigned int inode_blkoff;
	TBLBF_BLKCELL* blkcell;
	
	if (!p_entry)
		return 0;	//file not found.

	inode_blkno = te2fs_inode_block(p_entry->inode, 
			ml_hnd_fsmdl->hnd_fs.start_inode_blkno); 
	
	inode_blkoff = te2fs_inode_block_offset(p_entry->inode);

	blkcell = tblbm_get_blkcell(inode_blkno);

	if (!blkcell) {
		return 0;	
	}
	
	return (struct ext2_inode*) (blkcell->blk + inode_blkoff);
	
}

/**
 * @brief get inode by inode no
 * @param pp_dirty_inoblk dirty flag double pointer of inode block (OUT parameter)
 * @param inono inode no
 * @return ext2_inode
 */
struct ext2_inode* tflm_get_inode_by_no(unsigned int** pp_dirty_inoblk, 
		unsigned int inono) {

	unsigned int inode_blkno;
	unsigned int inode_blkoff;
	TBLBF_BLKCELL* blkcell;
	
	if (!inono)
		return 0;	//file not found.

	inode_blkno = te2fs_inode_block(inono, 
			ml_hnd_fsmdl->hnd_fs.start_inode_blkno); 
	
	inode_blkoff = te2fs_inode_block_offset(inono);

	blkcell = tblbm_get_blkcell(inode_blkno);

	if (!blkcell) {
		return 0;	
	}
	*pp_dirty_inoblk = &blkcell->dirty;
	return (struct ext2_inode*) (blkcell->blk + inode_blkoff);
	
}

unsigned int tflm_get_size(int fd, TFL_HND_FILE** user_fdtbl) {

	return user_fdtbl[fd]->ext2_inode->i_size;

}

void tflm_sync() {
	TBLBF_BLKCELL* blkcell;
	
	blkcell = tblbf_get_blkcell_dirty();
	while(blkcell) {
		//printf("dirty: %#x\n", blkcell->blkno);
		debug_varval01("dirty blkno", blkcell->blkno, 1);
		tatam_writeblock(blkcell->blk, blkcell->blkno * 2);
		blkcell->dirty = 0;
		blkcell = tblbf_get_blkcell_dirty();
	}

}

/**
 * @brief "write" procedure for block device
 * 
 * @param fd
 * @param buf
 * @param size
 * @param user_fdtbl
 * @return written size
 */
int tflm_write(int fd, char* buf, unsigned int size, 
			  TFL_HND_FILE** user_fdtbl) {

	TFL_HND_FILE* file;
	struct ext2_inode* p_inode;
	TBLBF_BLKCELL* blkcell;
	TBLBF_BLKCELL* indirect_blkcell;
	unsigned int sz_write, sz_rest, blk_off;
	int idx_block;
	char* p_start_src;
	unsigned char* p_start_dst;
	char buff2[128];
	
	if (fd < 0) {
		sprint_varval(buff2, "Error: Invalid fd. fd", fd, 0);
		debug_puts(buff2);
		return 0;
	}
	
	file = user_fdtbl[fd];
	p_inode = file->ext2_inode;
	sz_rest = size;
	p_start_src = buf;
	indirect_blkcell = 0;
	
	while(sz_rest) {
		idx_block = file->offset / ml_hnd_fsmdl->hnd_fs.sz_block;
		blk_off = file->offset % ml_hnd_fsmdl->hnd_fs.sz_block;

		// allocate new block if needed
		
		//blkcell = tflm_append_newblk(p_inode, idx_block, &ml_hnd_fsmdl->hnd_fs);
		blkcell = tflm_getblk_by_inode_index(p_inode, idx_block, 1);
		if(!blkcell) {
			return 0;
		}
		
		// calculate write size
		if (blk_off + sz_rest > ml_hnd_fsmdl->hnd_fs.sz_block) {
			sz_write = ml_hnd_fsmdl->hnd_fs.sz_block - blk_off;  
		} else {
			sz_write = sz_rest;
		}
		
		// write block-buffer
		p_start_dst = blkcell->blk;
		p_start_dst += blk_off;

		my_memcpy(p_start_dst, p_start_src, sz_write);
		p_start_src += sz_write;
		p_inode->i_size += sz_write;
		blkcell->dirty = 1;
		file->offset += sz_write;
		//*file->p_dirty_inoblk = 1;
		sz_rest -= sz_write;

	}

	return size;

}

/**
 * @brief get blockcell by inode index
 * 
 * @param p_inode inode
 * @param idx_block  index of inode.i_block[]
 * @param mode 1: allocate new block if i_block[idx_block] == 0, 0: never allocate new block
 * @return allocated block, 0: block not allocated
 */
TBLBF_BLKCELL* tflm_getblk_by_inode_index(struct ext2_inode* p_inode,	
		unsigned int idx_block, int mode) {

	TBLBF_BLKCELL* blkcell;
	TBLBF_BLKCELL* indirect_blkcell;
	unsigned int* p_indirect_block;
	unsigned int blkno, indirect_blkno;
	int i;
	char buff[128];
	
	indirect_blkcell = 0;

	//sprint_varval(buff, "getblk_by_inode_index - idx_block", idx_block, 1);
	//debug_puts(buff);

	
	if (idx_block < 12) {
		if (0 == p_inode->i_block[idx_block]) {	// not allocated i_block
			
			if (!mode) return 0;		//error -  not allocated i_block

			blkno = te2fm_alc_new_blkno(ml_hnd_fsmdl);

			sprint_varval(buff, "new blkno", blkno, 1);
			debug_puts(buff);
			
			p_inode->i_block[idx_block] = blkno;
			p_inode->i_blocks += 2;
			
		} else {
			blkno = p_inode->i_block[idx_block];
		}		
		blkcell = tblbm_get_blkcell(blkno);
	} 
	else if (12 <= idx_block && idx_block < 256+12) {

		if (0 == p_inode->i_block[12]) {

			if (!mode) return 0; // not allocated indirect block

			//allocate and initialize indirect block
			indirect_blkno = te2fm_alc_new_blkno(ml_hnd_fsmdl);
			p_inode->i_block[12] = indirect_blkno;
			indirect_blkcell = tblbm_get_blkcell(p_inode->i_block[12]);
			p_indirect_block = (unsigned int*) indirect_blkcell->blk;
			for (i=0; i<256; i++) p_indirect_block[i] = 0;
			indirect_blkcell->dirty = 1;
			p_inode->i_blocks += 2;

		}
		indirect_blkcell = tblbm_get_blkcell(p_inode->i_block[12]);
		
		p_indirect_block = (unsigned int*) indirect_blkcell->blk;
		blkno = p_indirect_block[idx_block - 12];
		if (!blkno) {
			if (!mode) return 0;		//error -  not allocated block-no in indirect block
			
			blkno = te2fm_alc_new_blkno(ml_hnd_fsmdl);
			p_indirect_block[idx_block - 12] = blkno;
			indirect_blkcell->dirty = 1;
			p_inode->i_blocks += 2;
		}
		
		blkcell = tblbm_get_blkcell(blkno);
	} else {
		//TODO reading the file using double indirect block.
		debug_puts("Error - the file using double indirect block reading is not implemented yet.");
		return 0;
	}
		
	return blkcell;
	
}


//TODO - need refactoring, Temporary Implemantation
unsigned char* tflm_prepare_filesystem(TE2FM_HND_FSMDL* hnd_fsmdl) {
	
	//int rc;

	TBLBF_BLKCELL* blkcell_01;
	TBLBF_BLKCELL* blkcell_02;
	TBLBF_BLKCELL* blkcell_08;
	TBLBF_BLKCELL* blkcell_09;
	TBLBF_BLKCELL* blkcell_0a;
	TBLBF_BLKCELL* blkcell_21;

	
	unsigned char* rootdir_ent;		//root-directory-entry

	struct ext2_inode* p_inode;
	struct ext2_super_block* p_ext2_sb;	// super block
	struct ext2_group_desc* p_ext2_gd;	// blockgroup descriptor 

	/**************************
	 * read super block
	 **************************/

//			rc = tatam_readblock((char*)&ext2_sb, 0x2);
//			if (rc) {
//				puts_varval01(&hnd_sc, &my_cnsl, 
//						"error: tatam_readblock: rc", rc, 0);
//			}

	blkcell_01 = tblbm_get_blkcell(0x1);
	if (!blkcell_01) {
		debug_puts("error: tatam_get_blkcell");
	}
	p_ext2_sb = (struct ext2_super_block*) blkcell_01->blk;
	
	//puts_varval01(&hnd_sc, &my_cnsl, 
	//		"&ext2_sb", (unsigned int)&ext2_sb, 1);

	debug_varval01("ext2_sb.s_blocks_count", p_ext2_sb->s_blocks_count, 1);
	
	debug_varval01("ext2_sb.s_log_block_size", p_ext2_sb->s_log_block_size, 1);
	
	
	//hnd_fs->sz_block = 1024 << p_ext2_sb->s_log_block_size;
	hnd_fsmdl->hnd_fs.sz_block = 1024 << p_ext2_sb->s_log_block_size;
	
	debug_varval01("hnd_fs.sz_block", hnd_fsmdl->hnd_fs.sz_block, 1);

	/**************************
	 * read group descriptor
	 **************************/
	blkcell_02 = tblbm_get_blkcell(0x2);
	if (!blkcell_02) {
		debug_puts("error: tatam_get_blkcell");
	}

	p_ext2_gd = (struct ext2_group_desc*) blkcell_02->blk; 
	
	debug_varval01("blkcell_02->blkno", blkcell_02->blkno, 1);
	
	debug_varval01("ext2_gd.bg_block_bitmap", p_ext2_gd->bg_block_bitmap, 1);
	
	debug_varval01("ext2_gd.bg_block_bitmap", p_ext2_gd->bg_inode_bitmap, 1);

	debug_varval01("ext2_gd.bg_inode_table", p_ext2_gd->bg_inode_table, 1);

	hnd_fsmdl->hnd_fs.start_inode_blkno = p_ext2_gd->bg_inode_table;

	blkcell_08 = tblbm_get_blkcell(p_ext2_gd->bg_block_bitmap);
	if (!blkcell_08) {
		debug_puts("error: tatam_get_blkcell");
	}
	hnd_fsmdl->blkbmp_blkcell = blkcell_08;
	
	hnd_fsmdl->hnd_fs.blkbmp = blkcell_08->blk;
	
	//blkcell_08->dirty = 1;
	
	blkcell_09 = tblbm_get_blkcell(p_ext2_gd->bg_inode_bitmap);
	if (!blkcell_09) {
		debug_puts("error: tatam_get_blkcell");
	}
	hnd_fsmdl->inobmp_blkcell = blkcell_09;
	
	hnd_fsmdl->hnd_fs.inobmp = blkcell_09->blk;
	
	//blkcell_09->dirty = 1;
	
	tflm_init_filetbl(hnd_fsmdl);

	
	/**************************
	 * read root-directory-entry
	 **************************/
	blkcell_0a = tblbm_get_blkcell(0xa);

	p_inode = (struct ext2_inode*) blkcell_0a->blk;

	p_inode++;	// inode-no=2 is root-inode
	hnd_fsmdl->root_inode = p_inode;
	
	debug_varval01("p_inode->i_blocks", p_inode->i_blocks, 1);

	debug_varval01("p_inode->i_block[0]", p_inode->i_block[0], 1);
	
	blkcell_21 = tblbm_get_blkcell(p_inode->i_block[0]);
	rootdir_ent = blkcell_21->blk;
	//blkcell_21->dirty = 1;
	hnd_fsmdl->rootdir_blkcell = blkcell_21;
	
	debug_puts("print root directory-entry.");
	print_direntry(rootdir_ent);

	//tflm_set_rootdir_ent(rootdir_ent);
	
	return rootdir_ent;
	
}

void print_direntry(unsigned char* block) {
	struct ext2_dir_entry_2* p_entry;
	unsigned char* p;
	int pos = 0;
	
	p = block;

	while(pos < 1024) {
		p_entry = (struct ext2_dir_entry_2*)p;
		dump_direntry(p_entry);
		p+=p_entry->rec_len;
		pos+=p_entry->rec_len;
	}

}

void dump_direntry(struct ext2_dir_entry_2* p_entry) {
	char buff[256];
	
	debug_varval01("inode_no", p_entry->inode, 1);
	debug_varval01("rec_len", p_entry->rec_len, 1);
	debug_varval01("name_len", p_entry->name_len, 1);
	debug_varval01("file_type", p_entry->file_type, 1);
	my_memset(buff, 0, sizeof(buff));
	my_strcpy(buff, "name=");
	my_strncat(buff, p_entry->name, p_entry->name_len);
	debug_puts(buff);
}

void dump_inode(struct ext2_inode* p_inode, unsigned int inode_no) {
	int j;
	char buff[128], buff2[128];
	
	//printf("=== inode %d info  ===\n", inode_no);
	debug_puts("=== inode info  ===");
	debug_varval01("inode no", inode_no, 1);
	debug_varval01("inode: i_mode", (unsigned short)p_inode->i_mode, 1);
	//printf("inode: i_uid=%#x\n", p_inode->i_uid);
	
	//printf("inode: i_size=%#x\n", p_inode->i_size);
	debug_varval01("inode: i_size", p_inode->i_size, 1);
	//printf("inode: i_blocks=%#x\n", p_inode->i_blocks);
	debug_varval01("inode: i_blocks", p_inode->i_blocks, 1);
	/*
	printf("inode: i_flags=%#x\n", p_inode->i_flags);
	printf("inode: i_links_count=%#x\n", p_inode->i_links_count);
	printf("inode: i_file_acl=%#x\n", p_inode->i_file_acl);
	printf("inode: i_dir_acl=%#x\n", p_inode->i_dir_acl);
	printf("inode: i_gid=%#x\n", p_inode->i_gid);
	*/
	for(j=0; j<EXT2_N_BLOCKS; j++) {
		//printf("inode: i_block[%d]=%#x\n", j, p_inode->i_block[j]);
		sprint_varval(buff, "i_block index", j, 0);
		sprint_varval(buff2, ", i_block", p_inode->i_block[j], 1);
		my_strcat(buff, buff2);
		debug_puts(buff);
		if (j > (p_inode->i_blocks / 2)) break;
	}

}

