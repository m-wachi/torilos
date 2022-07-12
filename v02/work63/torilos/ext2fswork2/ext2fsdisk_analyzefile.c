#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <torilos/trblkbuf.h>
#include <torilos/trfilemdl.h>
#include <torilos/trblkbufmdl.h>
#include <torilos/debug01.h>
#include <torilos/trext2fs_opr.h>

#include <torilos_stab/trataiomdl_stdapp.h>

int main(int argc, char** argv) {
	int sz_readbuf;
	char* blkbuf;
	char* target_fname;
	char* diskimage_fname;
	int fd, sz_bufpage;
	int i;
	
	TE2FM_HND_FSMDL hnd_fsmdl;	// FileSystem Handle (wrapper version)

	TFL_HND_FILE* user_fdtbl[16];  // user fd table

	TBLBF_BLKCELL* blkcell;
	
	/*
	 * checking args
	 */
	if (argc < 3) {
		puts("usage: ext2fsdisk_analyzefile (diskimage) (target-file(full-path))");
		exit(-1);
	}

	diskimage_fname = argv[1];
	target_fname = argv[2];

	//sz_readbuf = atoi(argv[3]);
	sz_readbuf = 1024;
	
	printf("diskimage=[%s], target_fname=[%s]\n", diskimage_fname, target_fname);
	
	//sprintf(outfile, "%s.out", target_fname);
	
	/*
	 * preparation
	 */
	tatam_stdapp_init(diskimage_fname, 256);
	
	sz_bufpage = 16;
	
	blkbuf = malloc(4096 * sz_bufpage);

	tflm_init_filetbl(&hnd_fsmdl);
	
	tblbf_init((unsigned char*)blkbuf, sz_bufpage, 1024);
	
	trind_init();

	tflm_prepare_filesystem(&hnd_fsmdl);
	
	te2fo_init(&hnd_fsmdl);

	//strcpy(dst_target_fname, "/");
	//strcat(dst_target_fname, target_fname);

	/*
	 * get file
	 */
	//fd = tflm_open3(target_fname, user_fdtbl);
	fd = te2fo_open(target_fname, user_fdtbl);
	if (fd < 0) {
		fprintf(stderr, "Error - Couldn't tflm_open3(%s). fd=%d\n", target_fname, fd);
		exit(-1);
	}
	
	debug_switch(1);
	
	dump_inode(user_fdtbl[fd]->ext2_inode, 0xaa);
	
	//fpout1 = fopen(target_fname, "w");
	/*
	while(sz_read = tflm_read(fd, buff, sz_readbuf, user_fdtbl)) {
		fwrite(buff, 1, sz_read, fpout1);
	}
	*/
	debug_switch(0);
	i = 0;
	
	printf("=== used block-no info ===\n");
	blkcell = tflm_getblk_by_inode_index(user_fdtbl[fd]->ext2_inode, i, 0);
	while(blkcell) {
		printf("%d: blkno=%#x\n", i, blkcell->blkno);
		blkcell = tflm_getblk_by_inode_index(user_fdtbl[fd]->ext2_inode, ++i, 0);
	}

	//fclose(fpout1);
	tflm_close(fd, user_fdtbl);
	
	free(blkbuf);
	
	exit(0);
	
}

