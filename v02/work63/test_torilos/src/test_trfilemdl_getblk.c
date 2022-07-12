#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <torilos/trblkbuf.h>
#include <torilos/trfilemdl.h>
#include <torilos/trblkbufmdl.h>
#include <torilos/trext2fs_opr.h>

#include <torilos_stab/trataiomdl_stdapp.h>


int main(int argc, char** argv) {
	int errcnt=0;
	char* blkbuf;
	char target_fname[] = "/test_getbk.txt";
	char* diskimage_fname;
	int fd, sz_bufpage;
	unsigned int blkno1, blkno_indirect;
	unsigned int *p_indirect;
	struct ext2_inode* p_inode;
	
	TE2FM_HND_FSMDL hnd_fsmdl;	// FileSystem Handle (wrapper version)

	TFL_HND_FILE* user_fdtbl[16];  // user fd table

	TBLBF_BLKCELL* blkcell0;
	TBLBF_BLKCELL* blkcell1;
	TBLBF_BLKCELL* blkcell12;
	TBLBF_BLKCELL* blkcell13;
	TBLBF_BLKCELL* blkcell_indirect;
	
	
	/*
	 * checking args
	 */
	if (argc < 2) {
		puts("usage: test_trfilemdl_getblk (diskimage)");
		exit(-1);
	}

	diskimage_fname = argv[1];
	printf("diskimage=[%s], target_fname=[%s]\n", diskimage_fname, target_fname);
	

	/*
	 * test preparation
	 */
	tatam_stdapp_init(diskimage_fname, 256);
	
	sz_bufpage = 16;
	
	blkbuf = malloc(4096 * sz_bufpage);

	tflm_init_filetbl(&hnd_fsmdl);
	
	tblbf_init((unsigned char*)blkbuf, sz_bufpage, 1024);
	
	trind_init();

	tflm_prepare_filesystem(&hnd_fsmdl);
	te2fo_init(&hnd_fsmdl);
	
	//fd = tflm_create2(target_fname, user_fdtbl);
	fd = te2fo_create(target_fname, user_fdtbl);
	
	if (fd < 3) {
		fprintf(stderr, "Error - Couldn't tflm_open3(%s). fd=%d\n", target_fname, fd);
		exit(-1);
	}
	
	p_inode = user_fdtbl[fd]->ext2_inode;

	/*
	 *  test mode=0 and error
	 */
	blkcell0 = tflm_getblk_by_inode_index(p_inode, 0, 0);
	if (0 != blkcell0) {
		fprintf(stderr, "Failed - test mode=0 and error - tflm_getblk_by_inode_index()");
		errcnt++;
	}
	
	/*
	 *  test mode=1, idx_block=0
	 */
	blkcell0 = tflm_getblk_by_inode_index(p_inode, 0, 1);
	if (0 == blkcell0) {
		fprintf(stderr, "Failed - test mode=1, idx_block=0 - tflm_getblk_by_inode_index()\n");
		errcnt++;
	}
	
	/*
	 *  test re-fetch allocated block.
	 */
	blkcell1 = tflm_getblk_by_inode_index(p_inode, 1, 1);
	if (0 == blkcell1) {
		fprintf(stderr, "Failed - test mode=1, idx_block=0 - tflm_getblk_by_inode_index()\n");
		errcnt++;
	}
	blkno1 = blkcell1->blkno;
	sprintf((char*)blkcell1->blk, "idx_block=1, blkno=%#x", blkno1);
	blkcell1 = tflm_getblk_by_inode_index(p_inode, 1, 0);
	if (0 == blkcell1) {
		fprintf(stderr, "Failed - test mode=1, idx_block=1 - tflm_getblk_by_inode_index()\n");
		errcnt++;
	}
	if (blkno1 != blkcell1->blkno) {
		fprintf(stderr, "Failed - test mode=1, idx_block=1 - tflm_getblk_by_inode_index()\n");
		fprintf(stderr, "\tExpected: %#x, Actual: %#x\n", blkno1, blkcell1->blkno);
		fprintf(stderr, "\tblk=[%s]\n", blkcell1->blk);
		errcnt++;
	}

	blkcell12 = tflm_getblk_by_inode_index(p_inode, 12, 1);
	if (0 == blkcell12) {
		fprintf(stderr, "Failed - test mode=1, idx_block=12 - tflm_getblk_by_inode_index()\n");
		errcnt++;
	}
	
	blkcell13 = tflm_getblk_by_inode_index(p_inode, 13, 1);
	if (0 == blkcell13) {
		fprintf(stderr, "Failed - test mode=1, idx_block=12 - tflm_getblk_by_inode_index()\n");
		errcnt++;
	}
	
	blkno_indirect = p_inode->i_block[12];

	blkcell_indirect = tblbm_get_blkcell(blkno_indirect);

	if (0 == blkcell_indirect->dirty) {
		fprintf(stderr, "Failed - indirect block - tflm_getblk_by_inode_index()\n");
		fprintf(stderr, "\tblkcell_indirect->dirty=%d\n", 
				blkcell_indirect->dirty);
		errcnt++;
	}
	
	p_indirect = (unsigned int*)blkcell_indirect->blk;
	
	if (p_indirect[0] != blkcell12->blkno) {
		fprintf(stderr, "Failed - indirect block data - tflm_getblk_by_inode_index()\n");
		fprintf(stderr, "\tp_indirect[0]=%#x, blkcell12->blkno=%#x\n", 
				p_indirect[0], blkcell12->blkno);
		errcnt++;
	}
	
	if (p_indirect[1] != blkcell13->blkno) {
		fprintf(stderr, "Failed - indirect block data - tflm_getblk_by_inode_index()\n");
		fprintf(stderr, "\tp_indirect[0]=%#x, blkcell13->blkno=%#x\n", 
				p_indirect[0], blkcell13->blkno);
		errcnt++;
	}
	
	tflm_close(fd, user_fdtbl);
	
	//actual_sync();
	
	free(blkbuf);
	
	if (errcnt)
		exit(1);
	
	puts("OK - test passed.");
	exit(0);
	
}

