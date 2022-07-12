#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <torilos/trblkbuf.h>
#include <torilos/trfilemdl.h>
#include <torilos/trblkbufmdl.h>
#include <torilos/debug01.h>
#include <torilos/trext2fs_opr.h>
#include <torilos_stab/trataiomdl_stdapp.h>
#include <torilos/ext2fs.h>

unsigned char getbit_bmp(unsigned char *bitmap, unsigned int no);

int main(int argc, char** argv) {
	int i, sz_writebuf, last_blkno, indirect_blkno, first_blkno;
	int errcnt = 0;
	char* blkbuf;
	char buff[4096];
	char* target_fname;
	char dst_target_fname[256];
	char* diskimage_fname;
	int fd, sz_bufpage;
	unsigned int sz_read, sz_write;
	FILE* fpin1;
	unsigned char bytedata;
	
	TE2FM_HND_FSMDL hnd_fsmdl;	// FileSystem Handle (wrapper version)
	TFL_HND_FILE* user_fdtbl[16];  // user fd table
	TBLBF_BLKCELL* blkcell;

	/*
	 * checking args
	 */
	if (argc < 4) {
		puts("usage: test_trext2fs_opr_truncate (diskimage) (test data file) (write-buffer size).");
		exit(-1);
	}

	diskimage_fname = argv[1];
	target_fname = argv[2];

	sz_writebuf = atoi(argv[3]);
	if (sz_writebuf > 4096) {
		fprintf(stderr, "too much write-buffer size. %s\n", argv[2]);
		exit(-1);
	}
	
	printf("diskimage=[%s], target_fname=[%s], write-buffer size=%d\n", diskimage_fname, target_fname, sz_writebuf);
	
	//sprintf(infile, "%s.out", target_fname);
	
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
	
	debug_switch(0);
	
	strcpy(dst_target_fname, "/");
	strcat(dst_target_fname, target_fname);
	
	fd = te2fo_create(dst_target_fname, user_fdtbl);
	if (fd < 0) {
		fprintf(stderr, "Error - Couldn't te2fo_create(%s). fd=%d\n", dst_target_fname, fd);
		exit(-1);
	}
	
	fpin1 = fopen(target_fname, "r");
	
	/*
	 * write data to truncate later.
	 */
	i = 0; 

	while((sz_read = fread(buff, 1, sz_writebuf, fpin1))) {
		sz_write = tflm_write(fd, buff, sz_read, user_fdtbl); 
		if (sz_write != sz_read) {
			fprintf(stderr, "Warning - couldn't write expected size. %dth read, expect=%d, actual=%d\n",
					i, sz_read, sz_write);
		}
		i++;
	}
	
	fclose(fpin1);
	tflm_close(fd, user_fdtbl);
	
	
	fd = te2fo_open(dst_target_fname, user_fdtbl);
	
	i = 0;
	
	//printf("=== used block-no info ===\n");
	blkcell = tflm_getblk_by_inode_index(user_fdtbl[fd]->ext2_inode, i, 0);
	while(blkcell) {
		last_blkno = blkcell->blkno;
		//printf("%d: blkno=%#x\n", i, blkcell->blkno);
		blkcell = tflm_getblk_by_inode_index(user_fdtbl[fd]->ext2_inode, ++i, 0);
	}
	first_blkno = user_fdtbl[fd]->ext2_inode->i_block[0];
	indirect_blkno = user_fdtbl[fd]->ext2_inode->i_block[12];
	
	//do truncate!
	te2fo_truncate(user_fdtbl[fd]->ext2_inode);
	
	//check all i_block[i] is 0
	for (i=0; i<EXT2_N_BLOCKS; i++) {
		if (user_fdtbl[fd]->ext2_inode->i_block[i]) {
			fprintf(stderr, "Error - i_block[%d] is %#x\n", i, 
					user_fdtbl[fd]->ext2_inode->i_block[i]);
			errcnt++;
		}
	}
	
	tflm_close(fd, user_fdtbl);
	
	/* 
	 * check first_blkno, indirect_blkno and last_blkno
	 * block-bitmap is really 'OFF'!
	 */
	//printf("first_blkno=%#x, indirect_blkno=%#x, last_blkno=%#x\n", 
	//		first_blkno, indirect_blkno, last_blkno);
	
	bytedata = getbit_bmp(hnd_fsmdl.blkbmp_blkcell->blk, first_blkno); 
	//printf("first_blkno blkbitmap=%#x\n", bytedata);
	if (bytedata) {
		fprintf(stderr, "Error - block-bitmap of first_blkno is not OFF\n");
		errcnt++;
	}
	
	bytedata = getbit_bmp(hnd_fsmdl.blkbmp_blkcell->blk, indirect_blkno); 
	//printf("indirect_blkno blkbitmap=%#x\n", bytedata);
	if (bytedata) {
		fprintf(stderr, "Error - block-bitmap of indirect_blkno is not OFF\n");
		errcnt++;
	}

	bytedata = getbit_bmp(hnd_fsmdl.blkbmp_blkcell->blk, last_blkno); 
	//printf("last_blkno blkbitmap=%#x\n", bytedata);
	if (bytedata) {
		fprintf(stderr, "Error - block-bitmap of last_blkno is not OFF\n");
		errcnt++;
	}
	
	tflm_sync();
	
	actual_sync();
	
	free(blkbuf);
	
	if (errcnt)
		return -1;
		
	puts("OK - test passed.");
	exit(0);
}

unsigned char getbit_bmp(unsigned char *bitmap, unsigned int no) {
	unsigned char* p;
	unsigned char bytedata;
	int idx;
	int lsht;
	unsigned char mask;

	idx = te2fs_bindex_bmp(no);

	lsht = te2fs_blshft_bmp(no);
	
	p = bitmap + idx;
	
	bytedata = *p;
	mask = 1 << lsht;
	bytedata &= mask;
	
	return bytedata;
}
