#ifndef TRINODE_H
#define TRINODE_H

#define TRIND_MAX_INODE		256

/** 
 * @brief memory inode struct 
 */
typedef struct tr_inode {
	/**
	 * @brief logical device no
	 * 
	 * 0: none (initial value)
	 * 1: hard-disk
	 * 2: terminal in (stdin)
	 * 3: terminal out (stdout)
	 */
	unsigned char log_dev_no;	
	unsigned int inono;	//!< inode no
	short i_mode;
	unsigned int free;	///< 1 if free
	struct tr_inode* prev_hash;
	struct tr_inode* next_hash;
	struct tr_inode* prev_free;
	struct tr_inode* next_free;

	
} TR_INODE;

int trind_init();
TR_INODE* trind_alc_free_inode();
int trind_remove_from_free(TR_INODE* tr_inode);
int trind_empty_free_inode();
void trind_free_inode(TR_INODE* tr_inode);
TR_INODE* trind_lookup_inode(unsigned char log_dev_no, unsigned int inono);
#endif
