/**
 * @file trinode.c
 * @brief memory-inode cache module
 */

#include <torilos/trinode.h>
#include <torilos/trlock.h>

/* static */ TR_INODE* ml_free_inode_head;

//ml_all_inode->next_hash is first blkcell.
/* static */ TR_INODE* ml_all_inode;

static TR_INODE ml_inodes[TRIND_MAX_INODE];

//static int ml_sz_inodes;

/**
 * @brief initialize memory-inode module
 * @return  0: success, 0!=: error
 */
int trind_init() {

	int i;
	int cnt;
	//unsigned char* p;

	//ml_sz_blkcells = sz_buf * (4096 / sz_block);
	//if (ml_sz_blkcells > TBLBF_MAX_BLKCELL) return -1;

	cnt = 0;
	//p = buff;
	
	//blkcells[0].prev_hash = blkcells[0].prev_free = 0;
	ml_inodes[0].prev_hash = 0;
	ml_inodes[0].prev_free = &ml_inodes[TRIND_MAX_INODE-1];
	ml_inodes[0].next_hash = ml_inodes[0].next_free = &ml_inodes[1];
	ml_inodes[0].log_dev_no = 0;
	ml_inodes[0].inono = 0;
	ml_inodes[0].free = 1;
	//ml_inodes[0].dirty = 0;
	//ml_inodes[0].blk = p;
	
	//p += sz_block;

	for (i=1; i<TRIND_MAX_INODE-1; i++) {
		ml_inodes[i].prev_hash = ml_inodes[i].prev_free = &ml_inodes[i-1];
		ml_inodes[i].next_hash = ml_inodes[i].next_free = &ml_inodes[i+1];
		ml_inodes[0].log_dev_no = 0;
		ml_inodes[i].inono = 0;
		ml_inodes[i].free = 1;
		//ml_inodes[i].dirty = 0;
		//ml_inodes[i].blk = p;
		//p += sz_block;
	}	

	ml_inodes[TRIND_MAX_INODE-1].log_dev_no = 0;
	ml_inodes[TRIND_MAX_INODE-1].inono = 0;
	ml_inodes[TRIND_MAX_INODE-1].free = 1;
	//ml_inodes[ml_sz_ml_inodes-1].dirty = 0;
	ml_inodes[TRIND_MAX_INODE-1].prev_hash = ml_inodes[TRIND_MAX_INODE-1].prev_free = &ml_inodes[TRIND_MAX_INODE-2];
	ml_inodes[TRIND_MAX_INODE-1].next_hash = 0;
	ml_inodes[TRIND_MAX_INODE-1].next_free = &ml_inodes[0];
	//ml_inodes[ml_sz_ml_inodes-1].blk = p;

	ml_all_inode = ml_free_inode_head = &ml_inodes[0];

	return 0;

}

/**
 * @brief allocate tr_inode from free-list
 * 
 * @return alocated tr_inode
 */
TR_INODE* trind_alc_free_inode() {
	TR_INODE* tr_inode;

	if (trind_empty_free_inode()) return 0;
	
	tlck_stop_intr();

	tr_inode = ml_free_inode_head->next_free;
	
	trind_remove_from_free(tr_inode);
	
	//tlck_allow_intr();

	return tr_inode;
}

/**
 * @brief remove tr_inode from free-list
 */
int trind_remove_from_free(TR_INODE* tr_inode) {
	
	tlck_stop_intr();

	tr_inode->prev_free->next_free = tr_inode->next_free;
	tr_inode->next_free->prev_free = tr_inode->prev_free;
	
	tr_inode->next_free = tr_inode->prev_free = 0;
	tr_inode->free = 0;
	
	//tlck_allow_intr();

	return 0;
}

/**
 * @brief test if free-list is empty
 */
int trind_empty_free_inode() {
	if (ml_free_inode_head->next_free == ml_free_inode_head) return 1;
	return 0;
}

/**
 * @brief return tr_inode to at the end of free-list
 * @param tr_inode return tr_inode
 */
void trind_free_inode(TR_INODE* tr_inode) {
	TR_INODE* tail;

	if (tr_inode->free) return;	//guard logic
	
	tlck_stop_intr();
	
	tail = ml_free_inode_head->prev_free;
	
	tr_inode->prev_free = tail;
	tr_inode->next_free = tail->next_free;
	tail->next_free->prev_free = tr_inode;
	tail->next_free = tr_inode;
	
	tr_inode->free = 1;

	//tlck_allow_intr();
	
}

/**
 * @brief lookup tr_inode by log_dev_no and inono
 * @param log_dev_no logical device no
 * @param inono inode-no
 * @return tr_inode. 0 if not found.
 */ 
TR_INODE* trind_lookup_inode(unsigned char log_dev_no, unsigned int inono) {
	TR_INODE* tr_inode;

	tr_inode = ml_all_inode;
	while (tr_inode->next_hash) {
		tr_inode = tr_inode->next_hash;
		if (tr_inode->log_dev_no == log_dev_no && 
				tr_inode->inono == inono) {
			return tr_inode;
		}
	}
	return 0;
}


