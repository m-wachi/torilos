/**
 * @file trext2fsmdl.c
 * @brief block-bitmap, inode-bitmap and directory-entry operation module (middle layer)
 */
#include <torilos/trext2fsmdl.h>
#include <torilos/trext2fs.h>
#include <torilos/trblkbufmdl.h>
#include <torilos/trutil.h>
#include <torilos/mylib01.h>
#include <torilos/trfilemdl.h>

/**
 * @brief allocate new inode-no
 * 
 * allocate new inode-no and mark dirty on inode bitmap block.
 * @param hnd_fsmdl
 * @return new inode-no
 */
unsigned int te2fm_alc_new_inono(TE2FM_HND_FSMDL* hnd_fsmdl) {
	unsigned int inono;

	inono = te2fs_alc_new_inono(&hnd_fsmdl->hnd_fs);
	hnd_fsmdl->inobmp_blkcell->dirty = 1;
	
	return inono;
}

/**
 * @brief free inode-no
 * 
 * free inode-no and mark dirty on inode bitmap block.
 * @param hnd_fsmdl
 * @param inono inode-no
 */
void te2fm_free_inono(TE2FM_HND_FSMDL* hnd_fsmdl, unsigned int inono) {
	te2fs_free_inono(&hnd_fsmdl->hnd_fs, inono);
	hnd_fsmdl->inobmp_blkcell->dirty = 1;
}

/**
 * @brief allocate new block-no on block-bitmap
 * 
 * allocate new block-no on block-bitmap and
 * mark dirty on inode bitmap block.
 * @param hnd_fsmdl
 * @return allocated new block-no
 */
unsigned int te2fm_alc_new_blkno(TE2FM_HND_FSMDL* hnd_fsmdl) {
	unsigned int blkno;

	blkno = te2fs_alc_new_blkno(&hnd_fsmdl->hnd_fs);
	hnd_fsmdl->blkbmp_blkcell->dirty = 1;
	return blkno;
}

/**
 * @brief free block-no
 * 
 * free block-no and mark dirty on inode bitmap block.
 * @param hnd_fsmdl
 * @param blkno block-no
 */
void te2fm_free_blkno(TE2FM_HND_FSMDL* hnd_fsmdl, unsigned int blkno) {

	te2fs_free_blkno(&hnd_fsmdl->hnd_fs, blkno);
	hnd_fsmdl->blkbmp_blkcell->dirty = 1;
}

/**
 * @brief get i_node from filepath
 * @param inono inode no (out parameter)
 * @param hnd_fsmdl
 * @param s_path filepath
 * @return i_node
 */
struct ext2_inode* te2fm_namei(int* inono, TE2FM_HND_FSMDL* hnd_fsmdl, 
		char* s_path) {
	char* s_cur_path;
	char s_path_buf[TFL_MAX_PATHSTR+1];
	TBLBF_BLKCELL* blkcell;
	struct ext2_dir_entry_2* p_dirent;
	struct ext2_inode* p_inode;
	int idx_slash;
	char name[128];
	unsigned int dir_blkno;
	
	my_strcpy(s_path_buf, s_path);
	
	s_cur_path = s_path_buf;
	
	if (!my_strcmp(s_cur_path, "/")) {
		*inono = 2;
		return hnd_fsmdl->root_inode;
	}
	
	s_cur_path++;	// skip first '/'
	
	//TODO search all block. only block[0] searching at the moment.
	dir_blkno = hnd_fsmdl->root_inode->i_block[0];
	
	idx_slash = my_stridx(s_cur_path, '/');

	while(idx_slash >= 0) {
		my_strncpy(name, s_cur_path, idx_slash);
		name[idx_slash] = 0;
		s_cur_path += idx_slash + 1;
		
		blkcell = tblbm_get_blkcell(dir_blkno);
		
		p_dirent = te2fs_lookup_dirent(name, blkcell->blk, hnd_fsmdl->hnd_fs.sz_block);
		if (!p_dirent)
			return 0;
		
		p_inode = tflm_get_inode_by_dirent(p_dirent);
		
		dir_blkno = p_inode->i_block[0];
		
		idx_slash = my_stridx(s_cur_path, '/');
	}
	
	blkcell = tblbm_get_blkcell(dir_blkno);

	p_dirent = te2fs_lookup_dirent(s_cur_path, blkcell->blk, hnd_fsmdl->hnd_fs.sz_block);
	if (!p_dirent)
		return 0;
	*inono = p_dirent->inode;
	
	p_inode = tflm_get_inode_by_dirent(p_dirent);
	
	return p_inode;
}
