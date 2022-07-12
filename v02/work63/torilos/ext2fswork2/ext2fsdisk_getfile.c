#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <torilos/trblkbuf.h>
#include <torilos/trfilemdl.h>
#include <torilos/trblkbufmdl.h>
#include <torilos/trext2fs_opr.h>

#include <torilos_stab/trataiomdl_stdapp.h>

int main(int argc, char** argv) {
	int sz_readbuf;
	char* blkbuf;
	char buff[4096];
	char* target_fname;
	char dst_target_fname[256];
	char* diskimage_fname;
	int fd, sz_bufpage;
	unsigned int sz_read;
	FILE* fpout1;
	
	TE2FM_HND_FSMDL hnd_fsmdl;	// FileSystem Handle (wrapper version)

	TFL_HND_FILE* user_fdtbl[16];  // user fd table

	
	/*
	 * checking args
	 */
	if (argc < 3) {
		puts("usage: ext2fsdisk_getfile (diskimage) (target-file)");
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
	
	/*
	 * get file
	 */
	strcpy(dst_target_fname, "/");
	strcat(dst_target_fname, target_fname);
	//fd = tflm_open3(target_fname, user_fdtbl);
	fd = te2fo_open(dst_target_fname, user_fdtbl);
	if (fd < 0) {
		fprintf(stderr, "Error - Couldn't tflm_open3(%s). fd=%d\n", target_fname, fd);
		exit(-1);
	}

	fpout1 = fopen(target_fname, "w");
	
	while((sz_read = tflm_read(fd, (unsigned char*)buff, sz_readbuf, user_fdtbl))) {
		fwrite(buff, 1, sz_read, fpout1);
	}

	fclose(fpout1);
	tflm_close(fd, user_fdtbl);
	
	free(blkbuf);
	
	exit(0);
	
}

