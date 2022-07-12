#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <torilos/trblkbuf.h>
#include <torilos/trfilemdl.h>
#include <torilos/trblkbufmdl.h>
#include <torilos/trext2fs_opr.h>

#include <torilos_stab/trataiomdl_stdapp.h>

int fancy_diff(char* file1, char* file2);

int main(int argc, char** argv) {
	int i, sz_readbuf;
	char* blkbuf;
	char buff[4096], outfile[256];
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
	if (argc < 4) {
		puts("usage: test_trfilemdl_read (diskimage) (test data file) (read-buffer size).");
		exit(-1);
	}

	diskimage_fname = argv[1];
	target_fname = argv[2];

	sz_readbuf = atoi(argv[3]);
	if (sz_readbuf > 4096) {
		fprintf(stderr, "too much read-buffer size. %s\n", argv[2]);
		exit(-1);
	}
	
	printf("diskimage=[%s], target_fname=[%s], read-buffer size=%d\n", diskimage_fname, target_fname, sz_readbuf);
	
	sprintf(outfile, "%s.out", target_fname);
	
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

	
	strcpy(dst_target_fname, "/");
	strcat(dst_target_fname, target_fname);

	//fd = tflm_open3(target_fname, user_fdtbl);
	fd = te2fo_open(dst_target_fname, user_fdtbl);
	if (fd < 0) {
		fprintf(stderr, "Error - Couldn't te2fo_open(%s). fd=%d\n", target_fname, fd);
		exit(-1);
	}
	
	fpout1 = fopen(outfile, "w");
	
	/*
	 * testing tflm_read
	 */
	i = 0; 
	while((sz_read = tflm_read(fd, (unsigned char*)buff, sz_readbuf, user_fdtbl))) {
//		if (sz_read != sz_readbuf) {
//			fprintf(stderr, "Warning - couldn't read expected size. %dth read, expect=%d, actual=%d\n",
//					i, sz_readbuf, sz_read);
//		}
		fwrite(buff, 1, sz_read, fpout1);
		i++;
	}
	

	fclose(fpout1);
	tflm_close(fd, user_fdtbl);
	
	//actual_sync();
	
	free(blkbuf);
	
	/*
	 * checking result.
	 */
	if (fancy_diff(target_fname, outfile))
		exit(1);
	
	puts("OK - test passed.");
	exit(0);

}

int fancy_diff(char* file1, char* file2) {
	FILE* fp1;
	FILE* fp2;
	char buff1[1024], buff2[1024];
	size_t sz_read1, sz_read2, sz_total;
	int rc = 0;
	
	if (!(fp1 = fopen(file1, "r"))) {
		fprintf(stderr, "Error - couldn't open file %s\n", file1);
	}

	if (!(fp2 = fopen(file2, "r"))) {
		fprintf(stderr, "Error - couldn't open file %s\n", file1);
	}
	
	sz_total = 0;
	while((sz_read1=fread(buff1, 1, 1024, fp1))) {
		sz_read2=fread(buff2, 1, 1024, fp2);
		if (sz_read1 != sz_read2) {
			fprintf(stderr, "warning - different size read. sz_read1=%d, sz_read2=%d\n",
					sz_read1, sz_read2);
			rc = -1;
			break;
		}
		if (memcmp(buff1, buff2, sz_read1)) {
			fprintf(stderr, "Error - detected different data.\n offset: from=%d, to=%d\n",
					sz_total, sz_total + sz_read1);
			rc = -2;
			break;
		}
		sz_total += sz_read1;
	}
	
	fclose(fp1);
	fclose(fp2);
	
	return rc;
}
