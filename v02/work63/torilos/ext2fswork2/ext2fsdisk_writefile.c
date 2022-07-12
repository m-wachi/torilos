#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <torilos/trblkbuf.h>
#include <torilos/trfilemdl.h>
#include <torilos/trblkbufmdl.h>
#include <torilos/trext2fsmdl.h>
#include <torilos/debug01.h>
#include <torilos/trinode.h>
#include <torilos/trext2fs_opr.h>

#include <torilos_stab/trataiomdl_stdapp.h>


int main(int argc, char** argv) {
	int sz_writebuf;
	char* blkbuf;
	char buff[4096];
	char* target_fname;
	char dst_target_fname[256];
	char* diskimage_fname;
	int fd, sz_bufpage;
	unsigned int sz_read, sz_write;
	FILE* fpin1;
	
	TE2FM_HND_FSMDL hnd_fsmdl;	// FileSystem Handle (wrapper version)

	TFL_HND_FILE* user_fdtbl[16];  // user fd table
	
	/*
	 * checking args
	 */
	if (argc < 3) {
		puts("usage: ext2fsdisk_writefile (diskimage) (test data file).");
		exit(-1);
	}

	diskimage_fname = argv[1];
	target_fname = argv[2];

	sz_writebuf = 1024;
	
	printf("diskimage=[%s], target_fname=[%s]\n", diskimage_fname, target_fname);
	
	//sprintf(infile, "%s.out", target_fname);
	
	/*
	 * preparation
	 */
	tatam_stdapp_init(diskimage_fname, 256);
	
	sz_bufpage = 32;
	
	blkbuf = malloc(4096 * sz_bufpage);

	tflm_init_filetbl(&hnd_fsmdl);
	
	tblbf_init((unsigned char*)blkbuf, sz_bufpage, 1024);
	
	tflm_prepare_filesystem(&hnd_fsmdl);
	
	trind_init();
	te2fo_init(&hnd_fsmdl);
	
	debug_switch(1);
	
	
	strcpy(dst_target_fname, "/");
	strcat(dst_target_fname, target_fname);
	//fd = tflm_create2(target_fname, user_fdtbl);
	fd = te2fo_create(dst_target_fname, user_fdtbl);
	if (fd < 0) {
		fprintf(stderr, "Error - Couldn't tflm_open3(%s). fd=%d\n", target_fname, fd);
		exit(-1);
	}
	
	fpin1 = fopen(target_fname, "r");
	

	while((sz_read = fread(buff, 1, sz_writebuf, fpin1))) {
		sz_write = tflm_write(fd, buff, sz_read, user_fdtbl); 
	}
	
	fclose(fpin1);
	tflm_close(fd, user_fdtbl);
	
	tflm_sync();
	
	actual_sync();
	
	free(blkbuf);
	
	exit(0);
}

