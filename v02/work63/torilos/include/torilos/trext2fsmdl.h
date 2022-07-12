#ifndef TREXT2FSMDL_H
#define TREXT2FSMDL_H

#include "trext2fs.h"
#include "trblkbuf.h"

/**
 * @brief ext2fs filesystem handle (middle layer)
 */
typedef struct te2fm_hnd_fsmdl {
	TE2FS_HND_FS hnd_fs;
	TBLBF_BLKCELL* rootdir_blkcell;
	TBLBF_BLKCELL* blkbmp_blkcell;
	TBLBF_BLKCELL* inobmp_blkcell;
	struct ext2_inode* root_inode;
}TE2FM_HND_FSMDL;

unsigned int te2fm_alc_new_inono(TE2FM_HND_FSMDL* hnd_fsmdl);
void te2fm_free_inono(TE2FM_HND_FSMDL* hnd_fsmdl, unsigned int inono);
unsigned int te2fm_alc_new_blkno(TE2FM_HND_FSMDL* hnd_fsmdl);
void te2fm_free_blkno(TE2FM_HND_FSMDL* hnd_fsmdl, unsigned int blkno);
struct ext2_inode* te2fm_namei(int* inono, TE2FM_HND_FSMDL* hnd_fsmdl, 
		char* s_path);

#endif
