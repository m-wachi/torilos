#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <torilos/trblkbuf.h>
#include <torilos/trfilemdl.h>
#include <torilos/trblkbufmdl.h>
#include <torilos/trext2fs_opr.h>
#include <torilos/dirent.h>
#include <torilos_stab/trataiomdl_stdapp.h>

int test_getdent_one();
int test_getdents();
int check_dirent(struct dirent* p_dirent, unsigned int exp_ino, char* exp_name);

int main(int argc, char** argv) {
	int errcnt=0;
	char* blkbuf;
	char diskimage_fname[] = "test_namei.img";
	int sz_bufpage;
	TE2FM_HND_FSMDL hnd_fsmdl;	// FileSystem Handle (wrapper version)


	printf("%s start\n", argv[0]);
	printf("diskimage=[%s]\n", diskimage_fname);
	
	
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

	errcnt += test_getdent_one();
	errcnt += test_getdents();
	
	free(blkbuf);
	
	if (errcnt) 
		exit(-1);

	puts("OK - test passed.");
	exit(0);

}

int test_getdent_one() {
	int errcnt=0;
	char buff[4096];
	char target_dir[512];
	int fd, sz_actual_read, rc;
	struct dirent* p_dirent;
	TFL_HND_FILE* user_fdtbl[16];  // user fd table

	strcpy(target_dir, "/");
	
	fd = te2fo_open(target_dir, user_fdtbl);
	if (fd < 0) {
		fprintf(stderr, "Error - te2fo_open(%s). fd=%d\n", target_dir, fd);
		exit(-1);
	}

	//
	// testing te2fo_getdent_one() 1st
	//
	p_dirent = (struct dirent*)buff;
	memset(buff, 0, sizeof(buff));
	rc = te2fo_getdent_one(&sz_actual_read, user_fdtbl[fd], p_dirent, 12);
	if (rc != 1) {
		fprintf(stderr, "Error - te2fo_getdent_one(). rc=%d\n", rc);
		errcnt++;
	}
	if (p_dirent->d_ino != 2) {
		fprintf(stderr, "Error - te2fo_getdent_one(). p_dirent->d_ino=%#x\n", (unsigned int)p_dirent->d_ino);
		errcnt++;
	}
	if (strcmp(p_dirent->d_name, ".")) {
		fprintf(stderr, "Error - te2fo_getdent_one(). p_dirent->d_name=%s\n", p_dirent->d_name);
		errcnt++;
	}
	if (sz_actual_read != 12) {
		fprintf(stderr, "Error - te2fo_getdent_one(). sz_actual_read=%d\n", sz_actual_read);
		errcnt++;
	}
	
	//
	// testing te2fo_getdent_one() 2nd
	//
	memset(buff, 0, sizeof(buff));
	rc = te2fo_getdent_one(&sz_actual_read, user_fdtbl[fd], p_dirent, 16);
	if (rc != 1) {
		fprintf(stderr, "Error - te2fo_getdent_one(). rc=%d\n", rc);
		errcnt++;
	}
	if (p_dirent->d_ino != 2) {
		fprintf(stderr, "Error - te2fo_getdent_one(). p_dirent->d_ino=%#x\n", (unsigned int)p_dirent->d_ino);
		errcnt++;
	}
	if (strcmp(p_dirent->d_name, "..")) {
		fprintf(stderr, "Error - te2fo_getdent_one(). p_dirent->d_name=%s\n", p_dirent->d_name);
		errcnt++;
	}
	if (sz_actual_read != 16) {
		fprintf(stderr, "Error - te2fo_getdent_one(). sz_actual_read=%d\n", sz_actual_read);
		errcnt++;
	}
	
	// re-open
	tflm_close(fd, user_fdtbl);
	
	fd = te2fo_open(target_dir, user_fdtbl);
	if (fd < 0) {
		fprintf(stderr, "Error - te2fo_open(%s). fd=%d\n", target_dir, fd);
		exit(-1);
	}

	//
	// testing te2fo_getdent_one() 3rd
	//
	memset(buff, 0, sizeof(buff));
	p_dirent = (struct dirent*)buff;
	rc = te2fo_getdent_one(&sz_actual_read, user_fdtbl[fd], p_dirent, 11);
	if (rc != 1) {
		fprintf(stderr, "Error - te2fo_getdent_one(). rc=%d\n", rc);
		errcnt++;
	}
	if (sz_actual_read != 0) {
		fprintf(stderr, "Error - te2fo_getdent_one(). sz_actual_read=%d\n", sz_actual_read);
		errcnt++;
	}
	
	//
	// testing te2fo_getdent_one() 4th
	//
	rc = te2fo_getdent_one(&sz_actual_read, user_fdtbl[fd], p_dirent, 12);
	if (rc != 1) {
		fprintf(stderr, "Error - te2fo_getdent_one(). rc=%d\n", rc);
		errcnt++;
	}
	if (sz_actual_read != 12) {
		fprintf(stderr, "Error - te2fo_getdent_one(). sz_actual_read=%d\n", sz_actual_read);
		errcnt++;
	}
	if (p_dirent->d_ino != 2) {
		fprintf(stderr, "Error - te2fo_getdent_one(). p_dirent->d_ino=%#x\n", (unsigned int)p_dirent->d_ino);
		errcnt++;
	}
	if (strcmp(p_dirent->d_name, ".")) {
		fprintf(stderr, "Error - te2fo_getdent_one(). p_dirent->d_name=%s\n", p_dirent->d_name);
		errcnt++;
	}

	tflm_close(fd, user_fdtbl);

	
	return errcnt;
}

int test_getdents() {
	
	int errcnt=0, i;
	char buff[4096];
	char target_dir[512];
	int fd, rc;
	struct dirent* p_dirent;
	TFL_HND_FILE* user_fdtbl[16];  // user fd table
	char* p_char;
	strcpy(target_dir, "/");
	
	fd = te2fo_open(target_dir, user_fdtbl);
	if (fd < 0) {
		fprintf(stderr, "Error - te2fo_open(%s). fd=%d\n", target_dir, fd);
		exit(-1);
	}

	//
	// testing te2fo_getdents() 1st
	//
	p_dirent = (struct dirent*)buff;
	memset(buff, 0, sizeof(buff));
	rc = te2fo_getdents(user_fdtbl[fd], p_dirent, 28);
	if (rc != 28) {
		fprintf(stderr, "Error - te2fo_getdents(). rc=%d\n", rc);
		errcnt++;
	}
	errcnt += check_dirent(p_dirent, 2, ".");

	p_char = (char*)p_dirent;
	p_char += p_dirent->d_reclen;
	p_dirent = (struct dirent*)p_char;
	
	errcnt += check_dirent(p_dirent, 2, "..");

	//re-open
	tflm_close(fd, user_fdtbl);

	fd = te2fo_open(target_dir, user_fdtbl);
	if (fd < 0) {
		fprintf(stderr, "Error - te2fo_open(%s). fd=%d\n", target_dir, fd);
		exit(-1);
	}


	//
	// testing te2fo_getdents() 2st
	//
	p_dirent = (struct dirent*)buff;
	memset(buff, 0, sizeof(buff));
	rc = te2fo_getdents(user_fdtbl[fd], p_dirent, 27);
	if (rc != 12) {
		fprintf(stderr, "Error - te2fo_getdents(). rc=%d\n", rc);
		errcnt++;
	}
	errcnt += check_dirent(p_dirent, 2, ".");

	//
	// testing te2fo_getdents() 3rd
	//
	memset(buff, 0, sizeof(buff));
	rc = te2fo_getdents(user_fdtbl[fd], p_dirent, 2048);
	if (rc != 116) {
		fprintf(stderr, "Error - te2fo_getdents(). rc=%d\n", rc);
		errcnt++;
	}
	
	i = 1;
	
	while ((unsigned int)p_dirent < (unsigned int)(buff + rc)) {
		
		switch (i) {
		case 1:
			errcnt += check_dirent(p_dirent, 2, "..");
			break;
		case 3:
			errcnt += check_dirent(p_dirent, 12, "testdir");
			break;
		case 6:
			errcnt += check_dirent(p_dirent, 16, "cat");
			break;
		}
		
		p_char = (char*)p_dirent;
		p_char += p_dirent->d_reclen;
		p_dirent = (struct dirent*)p_char;
		i++;
	}

	if (i != 7) {
		fprintf(stderr, "Error - te2fo_getdents(). i=%d\n", i);
		errcnt++;
	}
	
	//
	// testing te2fo_getdents() 3rd
	//
	memset(buff, 0, sizeof(buff));
	rc = te2fo_getdents(user_fdtbl[fd], p_dirent, 2048);
	if (rc != 0) {
		fprintf(stderr, "Error - te2fo_getdents(). rc=%d\n", rc);
		errcnt++;
	}
	
	tflm_close(fd, user_fdtbl);

	return errcnt;
}

int check_dirent(struct dirent* p_dirent, unsigned int exp_ino, char* exp_name) {
	int errcnt = 0;
	if (p_dirent->d_ino != exp_ino) {
		fprintf(stderr, "Error - p_dirent->d_ino=%#x\n", (unsigned int)p_dirent->d_ino);
		errcnt++;
	}
	if (strcmp(p_dirent->d_name, exp_name)) {
		fprintf(stderr, "Error - p_dirent->d_name=%s\n", p_dirent->d_name);
		errcnt++;
	}
	return errcnt;
}
