#include <torilos/trext2fs_opr.h>
#include <torilos/trext2fsmdl.h>
#include <torilos/trmajor.h>
#include <torilos/debug01.h>
#include <torilos/trstat.h>
#include <torilos/trfilemdl.h>
#include <torilos/mylib01.h>

static TE2FM_HND_FSMDL* ml_hnd_fsmdl;

void te2fo_init(TE2FM_HND_FSMDL* hnd_fsmdl) {
	ml_hnd_fsmdl = hnd_fsmdl;
}

/**
 * @brief "open" procedure for block device
 * 
 * @param filename (absolute path)
 * @param user_fdtbl
 * @return file descriptor, <0: error
 */
int te2fo_open(char* filename, TFL_HND_FILE** user_fdtbl) {
	TFL_HND_FILE* file;
	struct ext2_inode* p_inode;
	int fd;
	int inono;
	
	if(!(p_inode = te2fm_namei(&inono, ml_hnd_fsmdl, filename))) {
		return -2;	
	}
	
	file = tfl_alloc_filetbl();
	
	file->tr_inode = trind_alc_free_inode();
	file->tr_inode->inono = inono;
	file->tr_inode->log_dev_no = HD_MAJOR;
	file->tr_inode->i_mode = p_inode->i_mode;
	
	file->ext2_inode = p_inode;
	file->offset = 0;
	file->ref_count = 1;
	
	debug_varval01("p_inode->i_block[0]", p_inode->i_block[0], 1);
	
	fd = tfl_alloc_file_usertbl(user_fdtbl, file);
	if (fd<0)
		return -2;
	
	return fd;
}

/**
 * @brief "create" procedure for block device
 * 
 * @param filename (absolute path)
 * @param user_fdtbl
 * @return 0: success, error if others
 */
int te2fo_create(char* filename, TFL_HND_FILE** user_fdtbl) {

	TFL_HND_FILE* file;
	unsigned int new_inono, parent_inono;
	struct ext2_inode* p_inode;
	int i, fd;

	unsigned int** pp_dirty_inoblk;
	unsigned int* pi_buff;
	
	pp_dirty_inoblk = &pi_buff;
	p_inode = te2fo_create_new_dirent(&new_inono, &parent_inono, 
			pp_dirty_inoblk,	filename, EXT2_FT_REG_FILE);
	if (!p_inode) {
		debug_puts("couldn't create dirent");
		return -1;
	}
	
	p_inode->i_mode = 0x8180;
	p_inode->i_uid = 0x3e8;
	p_inode->i_gid = 0x3e8;
	p_inode->i_size = 0;
	p_inode->i_blocks = 0;
	p_inode->i_links_count = 1;

	for(i=0; i<EXT2_N_BLOCKS; i++)
		p_inode->i_block[i] = 0;
	
	//ml_hnd_fsmdl->rootdir_blkcell->dirty = 1;

	file = tfl_alloc_filetbl();
	
	file->ext2_inode = p_inode;
	file->offset = 0;

	file->tr_inode = trind_alc_free_inode();
	file->tr_inode->inono = new_inono; //p_entry->inode;
	file->tr_inode->log_dev_no = HD_MAJOR;
	file->tr_inode->i_mode = p_inode->i_mode;
	file->ref_count = 1;
	file->p_dirty_inoblk = *pp_dirty_inoblk;
	*file->p_dirty_inoblk = 1;

	fd = tfl_alloc_file_usertbl(user_fdtbl, file);
	if (fd<0)
		return -2;
	
	return fd;
}

/**
 * @brief 'getdents' main function
 * @param h_file file-handle (directory)
 * @param dirp read buffer
 * @param count read buffer size
 * @return 0: end of directory, >0: read count, <0: error
 */
int te2fo_getdents(TFL_HND_FILE* h_file, struct dirent* dirp, unsigned int count) {
	int sz_read, rc=1, sz_total_read;
	unsigned int sz_rest_dirp;
	char *p_src;
	
	if (h_file->tr_inode->log_dev_no != HD_MAJOR)
		return -1;
	
	if (!(h_file->ext2_inode->i_mode & S_IFDIR)) {
		return -2;
	}
	
	sz_total_read = 0;
	sz_rest_dirp = count;
	p_src = (char*)dirp;
	while(rc) {
		rc = te2fo_getdent_one(&sz_read, h_file, (struct dirent*)p_src, sz_rest_dirp);
		if (!sz_read) {
			break;
		}
		sz_rest_dirp -= sz_read;
		sz_total_read += sz_read;
		p_src += sz_read;
	}
	
	
	return sz_total_read;
}

/**
 * @brief read one directory entry
 * @param sz_actual_read actual read size
 * @param h_file file handle (directory)
 * @param dirp read buffer
 * @param sz_rest_dirp the size of available read buffer
 * @return 0: the end of directory, 1: not the end of directory
 */  
int te2fo_getdent_one(int* sz_actual_read, TFL_HND_FILE* h_file, 
		struct dirent* dirp, unsigned int sz_rest_dirp) {
	int sz_1st_read, sz_2nd_read;
	int sz_try_1st_read = 8;
	TFL_HND_FILE* fdtbl[1];
	unsigned char buff[1024];	//TODO fix to enable if disk-block-size is over 1024
	struct ext2_dir_entry_2* p_dirent;
	unsigned char *p_char;
	
	fdtbl[0] = h_file;

	p_char = buff;
	p_dirent = (struct ext2_dir_entry_2*)p_char;
	
	sz_1st_read = tflm_read(0, p_char, sz_try_1st_read, fdtbl);  
	if (!sz_1st_read) {
		*sz_actual_read = 0;
		return 0;
	}
	
	p_char += sz_1st_read;
	
	sz_2nd_read = tflm_read(0, p_char, p_dirent->rec_len - sz_1st_read, fdtbl);

	// 10 means sizeof(d_ino)(=4) + sizeof(d_off)(=4)
	//              + sizeof(d_reclen)(=2)
	//'+1' means counting '\0'.
	*sz_actual_read = 10 + p_dirent->name_len + 1; 
	
	//
	// align 4 byte that newlib 'readdir()' expecting
	//  
	*sz_actual_read = (*sz_actual_read + 3) & 0xfffffffc;
	
	// rollback offset if insufficient reading
	if (sz_rest_dirp < *sz_actual_read) {
		h_file->offset -= sz_1st_read + sz_2nd_read;
		*sz_actual_read = 0;
		return 1;
	}
	
	dirp->d_ino = p_dirent->inode;
	dirp->d_reclen = *sz_actual_read;
	my_strncpy(dirp->d_name, p_dirent->name, p_dirent->name_len);
	dirp->d_name[p_dirent->name_len] = 0;
	return 1;
}

struct ext2_inode* te2fo_namei(int* inono, char* s_path) {

	return te2fm_namei(inono, ml_hnd_fsmdl, s_path);

}

/**
 * @brief "mkdir" procedure for block device
 * 
 * @param path
 * @param mode 
 * @param cwd current working directory
 * @return 0: success, error if others
 */
int te2fo_mkdir(char* path, int mode, char* cwd) {
	//XXX enable 'mode' parameter
	
	unsigned int new_inono, parent_inono;
	struct ext2_inode* p_inode;
	int i;
	TBLBF_BLKCELL* blkcell;
	struct ext2_dir_entry_2* p_dirent_c;
	char* p_char;
	unsigned int** pp_dirty_inoblk;
	unsigned int* p_dirty_inoblk;
	char abs_path[TFL_MAX_PATHSTR];
	
	if (tfl_make_abspath(abs_path, path, cwd)) {
		return -1;
	}
	
	// create new entry and get inode itself
	pp_dirty_inoblk = &p_dirty_inoblk;
	p_inode = te2fo_create_new_dirent(&new_inono, &parent_inono, 
			pp_dirty_inoblk,	abs_path, EXT2_FT_DIR);
	if (!p_inode) {
		return -1;
	}
	
	// set up inode-attribute
	p_inode->i_mode = 0x41ed;
	p_inode->i_uid = 0x3e8;
	p_inode->i_gid = 0x3e8;
	p_inode->i_size = ml_hnd_fsmdl->hnd_fs.sz_block;
	p_inode->i_blocks = 0;
	p_inode->i_links_count = 1;

	for(i=0; i<EXT2_N_BLOCKS; i++)
		p_inode->i_block[i] = 0;
	
	// get new blk
	blkcell = tflm_getblk_by_inode_index(p_inode, 0, 1);
	if (!blkcell) {
		return -1;
	}
	// make "." entry on new directory block
	p_dirent_c = (struct ext2_dir_entry_2*)(p_char = (char*)blkcell->blk);
	p_dirent_c->inode = new_inono;
	p_dirent_c->file_type = EXT2_FT_DIR;
	p_dirent_c->name[0] = '.';
	p_dirent_c->name_len = 1;
	p_dirent_c->rec_len = ml_hnd_fsmdl->hnd_fs.sz_block;
	
	// make ".." entry on new directory block
	te2fs_alloc_dirent(&ml_hnd_fsmdl->hnd_fs, blkcell->blk, parent_inono, 
			EXT2_FT_DIR, "..");
	
	blkcell->dirty = 1;
	
	*p_dirty_inoblk = 1;

	return 0;
}

/**
 * @brief create new directory-entry
 * @param p_new_inono allocated new inode no (OUT parameter)
 * @param p_parent_inono parent inode no (OUT parameter)
 * @param p_dirty_inoblk dirty flag address of inode block (OUT parameter)
 * @param entry_abs_path entry absolute path to create
 * @param file_type new entry filetype
 * @return new entry inode, 0 if error
 * 
 * create new directory-entry and return it's i_node 
 */	
struct ext2_inode* te2fo_create_new_dirent(unsigned int* p_new_inono,
		unsigned int* p_parent_inono,
		unsigned int** pp_dirty_inoblk,
		char* entry_abs_path, unsigned char file_type) {

	char /*abs_path[TFL_MAX_PATHSTR],*/ dirpath[TFL_MAX_PATHSTR],
		filename[TFL_MAX_PATHSTR];
	
	struct ext2_inode* parent_inode;
	
	int idx_block, dir_inono;
	TBLBF_BLKCELL* blkcell;
	struct ext2_dir_entry_2* p_dirent;
	
	// get parent-directory abs-path and directory name
	tfl_dirpath_file(dirpath, filename, entry_abs_path);
	
	// get parent directory inode
	if(!(parent_inode = te2fm_namei(&dir_inono, ml_hnd_fsmdl, dirpath))) {
		debug_puts("couldn't get parent directory inode.");
		return 0;	
	}
	
	idx_block = 0;
	// get parent directory 1st block
	blkcell = tflm_getblk_by_inode_index(parent_inode, idx_block++, 0);

	// alloc new directory i-node no
	*p_new_inono = te2fm_alc_new_inono(ml_hnd_fsmdl);

	//-- LOOP Until all block exausted
	while(blkcell) {
		// existence check
		p_dirent = te2fs_lookup_dirent(filename, blkcell->blk, 
				ml_hnd_fsmdl->hnd_fs.sz_block);
	
		// if entry alreay exists, free i-node no and error return
		if (p_dirent) {
			te2fm_free_inono(ml_hnd_fsmdl, *p_new_inono);
			debug_puts("te2fo_create_new_dirent - entry already exists.");
			return 0;
		}
		
		// make one entry on the parent directory block
		p_dirent = te2fs_alloc_dirent(&ml_hnd_fsmdl->hnd_fs, blkcell->blk, 
				*p_new_inono, file_type, filename);
	
		// if block entry is not available, read next block
		if (p_dirent) {
			blkcell->dirty = 1;
			break;
		} 
		blkcell = tflm_getblk_by_inode_index(parent_inode, idx_block++, 1);
	}
	//-- LOOP_END
	
	// if all block entry is not available, error return
	if (!p_dirent) {
		te2fm_free_inono(ml_hnd_fsmdl, *p_new_inono);
		return 0;
	}
	
	// get new directory i_node 
	return tflm_get_inode_by_no(pp_dirty_inoblk, p_dirent->inode);
	
}

void te2fo_truncate(struct ext2_inode* p_inode) {
	int i = 0;
	TBLBF_BLKCELL* blkcell;
	
	// free blockcell and turn-off block bitmap
	blkcell = tflm_getblk_by_inode_index(p_inode, i, 0);
	while(blkcell) {
		te2fm_free_blkno(ml_hnd_fsmdl, blkcell->blkno);
		blkcell = tflm_getblk_by_inode_index(p_inode, ++i, 0);
	}
	
	// clear i_block
	for (i=0; i<EXT2_N_BLOCKS; i++) {
		if (p_inode->i_block[i]) {
			if (i == 12)
				te2fm_free_blkno(ml_hnd_fsmdl, p_inode->i_block[i]);
			p_inode->i_block[i] = 0;
		}
	}
	
	// clear other attributes
	p_inode->i_size = 0;
	p_inode->i_blocks = 0;
	
}
