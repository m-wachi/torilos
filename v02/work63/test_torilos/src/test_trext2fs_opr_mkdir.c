#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <torilos/trblkbuf.h>
#include <torilos/trfilemdl.h>
#include <torilos/trblkbufmdl.h>
#include <torilos/debug01.h>
#include <torilos/trext2fs_opr.h>
#include <torilos_stab/trataiomdl_stdapp.h>

int fancy_diff(char* file1, char* file2);

int main(int argc, char** argv) {
	int rc, fd, errcnt=0;
	char* blkbuf;
	char *dir_path, *p_char;
	char cwd[] = "/";
	char dst_dir_path[256];
	char* diskimage_fname;
	int sz_bufpage;
	char buff[1024];
	TE2FM_HND_FSMDL hnd_fsmdl;	// FileSystem Handle (wrapper version)
	TFL_HND_FILE* user_fdtbl[16];  // user fd table
	struct dirent* p_dirent;
	
	/*
	 * checking args
	 */
	if (argc < 3) {
		puts("usage: test_trext2fs_opr_mkdir (diskimage) (dir-path(without first '/')).");
		exit(-1);
	}

	diskimage_fname = argv[1];
	dir_path = argv[2];

	/*
	sz_writebuf = atoi(argv[3]);
	if (sz_writebuf > 4096) {
		fprintf(stderr, "too much write-buffer size. %s\n", argv[2]);
		exit(-1);
	}
	*/
	printf("diskimage=[%s], dir-path=[%s]\n", diskimage_fname, dir_path);
	
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
	
	//
	// do mkdir()
	//
	rc = te2fo_mkdir(dir_path, 0, cwd);
	if (rc != 0) {
		fprintf(stderr, "Error - Couldn't do te2fo_mkdir(). rc=%d\n", rc);
		exit(-1);
	}
	
	//
	// checking..
	//
	strcpy(dst_dir_path, "/");
	strcat(dst_dir_path, dir_path);
	
	fd = te2fo_open(dst_dir_path, user_fdtbl);
	if (fd < 0) {
		fprintf(stderr, "Error - te2fo_open(%s). fd=%d\n", dst_dir_path, fd);
		exit(-1);
	}

	p_dirent = (struct dirent*)buff;
	memset(buff, 0, sizeof(buff));
	rc = te2fo_getdents(user_fdtbl[fd], p_dirent, sizeof(buff));
	if (rc == 0) {
		fprintf(stderr, "Error - te2fo_getdents(). rc=%d\n", rc);
		errcnt++;
	}
	if (strcmp(p_dirent->d_name, ".")) {
		fprintf(stderr, "Error - te2fo_getdents(). d_name=%s\n", p_dirent->d_name);
		errcnt++;
	}

	p_char = (char*)p_dirent;
	p_char += p_dirent->d_reclen;
	p_dirent = (struct dirent*)p_char;

	if (strcmp(p_dirent->d_name, "..")) {
		fprintf(stderr, "Error - te2fo_getdents(). d_name=%s\n", p_dirent->d_name);
		errcnt++;
	}
	
	tflm_sync();
	
	free(blkbuf);

	if (errcnt)
		return -1;
		
	puts("OK - test passed.");
	return 0;
}

