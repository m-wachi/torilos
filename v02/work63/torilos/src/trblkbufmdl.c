/**
 * @file trblkbufmdl.c
 * @brief Buffer-cache Middle-layer Module.
 * 
 * This file is the higher-level-module of trblkbuf.
 */  
#include <torilos/trblkbufmdl.h>
#include <torilos/trataiomdl.h>
#include <torilos/debug01.h>
#include <torilos/trlock.h>
#include <torilos/myutil01.h>

static int ml_blkcell_opr_lock;
static int ml_readblk_opr_lock;

int tblbm_init(unsigned char* buff, int sz_buf, int sz_block) {
	ml_blkcell_opr_lock = 0;
	ml_readblk_opr_lock = 0;
	return tblbf_init(buff, sz_buf, sz_block);
}



/**
 * @brief get block by block-no
 * @param blkno   block-no (not lba)
 * @return  !=0: success, ==0: failed.
 */
TBLBF_BLKCELL* tblbm_get_blkcell(unsigned long blkno) {
	int rc;
	//int dev = 0;
	TBLBF_BLKCELL* blkcell;
	
	tlck_lock(&ml_blkcell_opr_lock, TLCK_LOCK_MEM);
	
	
	//lookup block-buffer
	blkcell = tblbf_lookup_blkcell(blkno);
	
	//return block if it's found.
	if (blkcell) {
		if (blkcell->free) {
			tblbf_remove_from_free(blkcell);
		}
		tlck_release(&ml_blkcell_opr_lock, TLCK_LOCK_MEM);
		return blkcell;
	}

	//allocated new free block
	blkcell = tblbf_alc_free_blkcell();
	
	if (!blkcell) {
		myutil_panic("fatal error: NO FREE BLOCKS!! - tblbm_get_blkcell");
		return 0;
	}
	
	if (blkcell->dirty) {
		myutil_panic("fatal error: dirty block flushing is not implemented yet.");
		return 0;
	}
	
	blkcell->blkno = blkno;
	tlck_release(&ml_blkcell_opr_lock, TLCK_LOCK_MEM);
	
	tlck_lock(&ml_readblk_opr_lock, TLCK_LOCK_IO);
	// read block from device
	rc = tatam_readblock(blkcell->blk, blkno * 2);
	tlck_release(&ml_readblk_opr_lock, TLCK_LOCK_IO);
	
	if (rc) return 0;
	
	return blkcell;
	
}



