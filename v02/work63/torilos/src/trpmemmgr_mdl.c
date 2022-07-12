/**
 * @file trpmemmgr_mdl.c
 * @brief Phisical Memory management module(middle layer).
 * 
 * Memory management functions are defined.
 * Mainly Phisical memory management.
 * This module combined locking operation
 */ 

#include <torilos/mymm01.h>
#include <torilos/trlock.h>


static int ml_memopr_lock;

void tpmmm_init() {
	ml_memopr_lock = 0;
}

/**
 * @brief malloc from application memory area with locking
 * @param size allocation memory size
 * @return memory area start address(phisical address)
 */
unsigned int tpmmm_malloc_pg_app(int size) {
	unsigned int rc;
	tlck_lock(&ml_memopr_lock, TLCK_LOCK_MEM);
	
	rc = mm_malloc_pg_app(size);
	
	tlck_release(&ml_memopr_lock, TLCK_LOCK_MEM);
	
	return rc;
}

/**
 * @brief malloc from kernel memory area with locking
 * @param size allocation memory size
 * @return memory area start address(phisical address)
 */ 
unsigned int tpmmm_malloc_pg_knl(int size) {
	unsigned int rc;
	tlck_lock(&ml_memopr_lock, TLCK_LOCK_MEM);
	
	rc = mm_malloc_pg_knl(size);
	
	tlck_release(&ml_memopr_lock, TLCK_LOCK_MEM);
	
	return rc;
	
}


/**
 * @brief free memory with locking
 * @param phisical memory addres to free
 */
void tpmmm_free(unsigned int addr) {
	tlck_lock(&ml_memopr_lock, TLCK_LOCK_MEM);
	
	mm_free(addr);
	
	tlck_release(&ml_memopr_lock, TLCK_LOCK_MEM);
}
