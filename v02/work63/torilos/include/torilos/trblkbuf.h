#ifndef TRBLKBUF_H
#define TRBLKBUF_H

#define TBLBF_MAX_BLKCELL   256

/**
 * @brief block-cell
 */ 
typedef struct tblbf_blkcell {
	unsigned int blkno;	///< block no
	unsigned char* blk;	///< block data
	unsigned int free;	///< 1 if free
	unsigned int dirty;	///< 1 if dirty (updated)
	struct tblbf_blkcell* prev_hash;
	struct tblbf_blkcell* next_hash;
	struct tblbf_blkcell* prev_free;
	struct tblbf_blkcell* next_free;
} TBLBF_BLKCELL;


int tblbf_init_old(unsigned char* buff, int sz_buf);
int tblbf_init(unsigned char* buff, int sz_buf, int sz_block);
TBLBF_BLKCELL* tblbf_alc_free_blkcell();
int tblbf_remove_from_free(TBLBF_BLKCELL* blkcell);
int tblbf_empty_free_blkcell();
void tblbf_free_blkcell(TBLBF_BLKCELL* blkcell);
TBLBF_BLKCELL* tblbf_lookup_blkcell(unsigned int blkno);
TBLBF_BLKCELL* tblbf_get_blkcell_dirty();
int tblbf_get_freeblk_count();
int tblbf_get_blkcells_count();
int tblbf_get_unalc_count();
int tblbf_dump_used_block();
#endif
