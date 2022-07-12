/**
 * @file trblkbuf.c
 * @brief Block-buffer cache module
 */

#include <torilos/trblkbuf.h>
#include <torilos/trlock.h>
#include <torilos/debug01.h>
#include <torilos/mylib01.h>
#include <torilos/trutil.h>

/* static */ TBLBF_BLKCELL* ml_free_blk_head;

//ml_all_blk->next_hash is first blkcell.
/* static */ TBLBF_BLKCELL* ml_all_blk;

static TBLBF_BLKCELL blkcells[TBLBF_MAX_BLKCELL];

static int ml_sz_blkcells;

/**
 * @brief initialize block-buffer module
 * @param buff    buffer memory
 * @param sz_buf  buffer memory size(/4kb)
 * @param sz_block buffer block size(byte)
 * @return  0: success, 0!=: error
 */
int tblbf_init(unsigned char* buff, int sz_buf, int sz_block) {

	int i;
	int cnt;
	unsigned char* p;

	//ml_sz_blkcells = sz_buf * 4;
	ml_sz_blkcells = sz_buf * (4096 / sz_block);
	
	if (ml_sz_blkcells > TBLBF_MAX_BLKCELL) return -1;

	cnt = 0;
	p = buff;
	
	blkcells[0].prev_hash = 0;
	blkcells[0].prev_free = &blkcells[ml_sz_blkcells-1];
	blkcells[0].next_hash = blkcells[0].next_free = &blkcells[1];
	blkcells[0].blkno = 0;
	blkcells[0].free = 1;
	blkcells[0].dirty = 0;
	blkcells[0].blk = p;

	p += sz_block;

	for (i=1; i<ml_sz_blkcells-1; i++) {
		blkcells[i].prev_hash = blkcells[i].prev_free = &blkcells[i-1];
		blkcells[i].next_hash = blkcells[i].next_free = &blkcells[i+1];
		blkcells[i].blkno = 0;
		blkcells[i].free = 1;
		blkcells[i].dirty = 0;
		blkcells[i].blk = p;
		p += sz_block;
	}	

	blkcells[ml_sz_blkcells-1].free = 1;
	blkcells[ml_sz_blkcells-1].dirty = 0;
	blkcells[ml_sz_blkcells-1].blkno = 0;
	blkcells[ml_sz_blkcells-1].prev_hash = blkcells[ml_sz_blkcells-1].prev_free = &blkcells[ml_sz_blkcells-2];
	blkcells[ml_sz_blkcells-1].next_hash = 0;
	blkcells[ml_sz_blkcells-1].next_free = &blkcells[0];
	blkcells[ml_sz_blkcells-1].blk = p;

	ml_all_blk = ml_free_blk_head = &blkcells[0];
	
	return 0;

}

/**
 * @brief allocate blockcell from free-list
 * 
 * @return alocated blockcell
 */
TBLBF_BLKCELL* tblbf_alc_free_blkcell() {
	TBLBF_BLKCELL* blkcell;

	if (tblbf_empty_free_blkcell()) return 0;
	
	//tlck_stop_intr();

	blkcell = ml_free_blk_head->next_free;
	
	tblbf_remove_from_free(blkcell);
	
	//tlck_allow_intr();

	return blkcell;
}

/**
 * @brief remove blockcell from free-list
 * @param blkcell blockcell to remove
 * @return 0: success
 */
int tblbf_remove_from_free(TBLBF_BLKCELL* blkcell) {
	
	//tlck_stop_intr();
	

	blkcell->prev_free->next_free = blkcell->next_free;
	blkcell->next_free->prev_free = blkcell->prev_free;
	
	blkcell->next_free = blkcell->prev_free = 0;
	blkcell->free = 0;
	
	//tlck_allow_intr();

	return 0;
}



/**
 * @brief test if free-list is empty
 */
int tblbf_empty_free_blkcell() {
	if (ml_free_blk_head->next_free == ml_free_blk_head) return 1;
	return 0;
}

/**
 * @brief return blockcell to at the end of free-list
 */
void tblbf_free_blkcell(TBLBF_BLKCELL* blkcell) {
	TBLBF_BLKCELL* tail;

	if (blkcell->free) return;	//guard logic
	
	//tlck_stop_intr();
	
	tail = ml_free_blk_head->prev_free;
	
	blkcell->prev_free = tail;
	blkcell->next_free = tail->next_free;
	tail->next_free->prev_free = blkcell;
	tail->next_free = blkcell;
	
	blkcell->free = 1;

	//tlck_allow_intr();
	
}

/**
 * @brief lookup blockcell by block-no
 * @param blkno block-no
 * @return blockcell. 0 if not found.
 */ 
TBLBF_BLKCELL* tblbf_lookup_blkcell(unsigned int blkno) {
	TBLBF_BLKCELL* blkcell;

	blkcell = ml_all_blk;
	while (blkcell->next_hash) {
		blkcell = blkcell->next_hash;
		if (blkcell->blkno == blkno) {
			return blkcell;
		}
	}
	return 0;
}

TBLBF_BLKCELL* tblbf_get_blkcell_dirty() {

	TBLBF_BLKCELL* blkcell;

	blkcell = ml_all_blk;
	while (blkcell->next_hash) {
		blkcell = blkcell->next_hash;
		if (blkcell->dirty) {
			return blkcell;
		}
	}
	return 0;
	
}

int tblbf_get_freeblk_count() {
	TBLBF_BLKCELL* next;
	int cnt = 0;
	
	next = ml_free_blk_head->next_free;
	
	while (ml_free_blk_head != next) {
		next = next->next_free;
		cnt++;
	}
	
	return cnt;
}

int tblbf_get_blkcells_count() {
	return ml_sz_blkcells;
}

int tblbf_get_unalc_count() {
	TBLBF_BLKCELL* next;
	int cnt = 0;
	
	next = ml_free_blk_head->next_free;
	
	while (ml_free_blk_head != next) {
		if (!next->blkno) cnt++;
		next = next->next_free;
	}
	
	return cnt;
}

int tblbf_dump_used_block() {
	TBLBF_BLKCELL* blkcell;

	blkcell = ml_all_blk;
	debug_puts("dump-used-block");
	
	while (blkcell->next_hash) {
		blkcell = blkcell->next_hash;
		if (!blkcell->free) {
			debug_varval01("used-block", blkcell->blkno, 1);
		}
	}
	return 0;
	
}

int tblbf_dump_all_block() {
	TBLBF_BLKCELL* blkcell;
	char buff[128], buff2[128];
	int i=0;
	
	blkcell = ml_all_blk;
	debug_puts("dump-all-block");

	debug_varval01("&ml_all_blk", (unsigned int)ml_all_blk, 1);
	sprint_varval(buff, "&ml_free_blk_head", (unsigned int)ml_free_blk_head, 1);
	sprint_varval(buff2, ", next", (unsigned int)ml_free_blk_head->next_free, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", prev", (unsigned int)ml_free_blk_head->prev_free, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);
	
	i++;
	while (blkcell->next_hash) {
		blkcell = blkcell->next_hash;
		sprint_varval(buff, "", i++, 0);
		sprint_varval(buff2, ": &blkcell", (unsigned int)blkcell, 1);
		my_strcat(buff, buff2);
		sprint_varval(buff2, ", free", (unsigned int)blkcell->free, 0);
		my_strcat(buff, buff2);
		sprint_varval(buff2, ", blkno", (unsigned int)blkcell->blkno, 1);
		my_strcat(buff, buff2);
		sprint_varval(buff2, ", dirty", (unsigned int)blkcell->dirty, 1);
		my_strcat(buff, buff2);
		debug_puts(buff);

		my_strcpy(buff, "    ");
		sprint_varval(buff2, "next_hash", (unsigned int)blkcell->next_hash, 1);
		my_strcat(buff, buff2);
		sprint_varval(buff2, ", prev_hash", (unsigned int)blkcell->prev_hash, 1);
		my_strcat(buff, buff2);
		debug_puts(buff);

		my_strcpy(buff, "    ");
		sprint_varval(buff2, "next_free", (unsigned int)blkcell->next_free, 1);
		my_strcat(buff, buff2);
		sprint_varval(buff2, ", prev_free", (unsigned int)blkcell->prev_free, 1);
		my_strcat(buff, buff2);
		debug_puts(buff);
	}
	return 0;
	
}

