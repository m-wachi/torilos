#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <torilos/trblkbuf.h>
#include <torilos/trfilemdl.h>
#include <torilos/trblkbufmdl.h>
#include <torilos/debug01.h>

#include <torilos_stab/trataiomdl_stdapp.h>

int main(int argc, char** argv) {
	int rc, errcnt=0, sz_bufpage, inono;
	char* blkbuf;
	char s_path[128];
	char diskimage_fname[] = "test_namei.img";
	TE2FM_HND_FSMDL hnd_fsmdl;	// FileSystem Handle (wrapper version)

	struct ext2_inode* p_inode;
	
	printf("diskimage=[%s]\n", diskimage_fname);
	

	debug_switch(0);
	
	/*
	 * test preparation
	 */
	rc = tatam_stdapp_init(diskimage_fname, 256);
	if (rc) {
		fprintf(stderr, "%s doesn't exist.\n", diskimage_fname);
		exit(-1);
	}
	
	
	sz_bufpage = 16;
	
	blkbuf = malloc(4096 * sz_bufpage);

	tflm_init_filetbl(&hnd_fsmdl);
	
	tblbf_init((unsigned char*)blkbuf, sz_bufpage, 1024);
	
	tflm_prepare_filesystem(&hnd_fsmdl);
	
	
	/*
	 * testing te2fm_namei
	 */
	strcpy(s_path, "/hello2.txt");
	inono = 0;

	p_inode = te2fm_namei(&inono, &hnd_fsmdl, s_path);
	//printf("p_inode->i_block[0]=%#x\n", p_inode->i_block[0]);
	if (0x37 != p_inode->i_block[0]) {
		fprintf(stderr, "Failed - te2fm_namei(\"%s\") - p_inode->i_block[0]=%#x\n", s_path, p_inode->i_block[0]);
		errcnt++;
	}
	if (inono != 0x0d) {
		fprintf(stderr, "Failed - te2fm_namei(\"%s\") - inono=%#x\n", s_path, inono);
		errcnt++;
	}
	
	strcpy(s_path, "/testdir");
	p_inode = te2fm_namei(&inono, &hnd_fsmdl, s_path);
	//printf("p_inode->i_block[0]=%#x\n", p_inode->i_block[0]);
	if (0x401 != p_inode->i_block[0]) {
		fprintf(stderr, "Failed - te2fm_namei(\"%s\") - p_inode->i_block[0]=%#x\n", s_path, p_inode->i_block[0]);
		errcnt++;
	}

	strcpy(s_path, "/testdir/hello3.txt");
	p_inode = te2fm_namei(&inono, &hnd_fsmdl, s_path);
	//printf("p_inode->i_block[0]=%#x\n", p_inode->i_block[0]);
	if (0x40 != p_inode->i_block[0]) {
		fprintf(stderr, "Failed - te2fm_namei(\"%s\") - p_inode->i_block[0]=%#x\n", s_path, p_inode->i_block[0]);
		errcnt++;
	}

	strcpy(s_path, "/");
	p_inode = te2fm_namei(&inono, &hnd_fsmdl, s_path);
	//printf("p_inode->i_block[0]=%#x\n", p_inode->i_block[0]);
	if (0x21 != p_inode->i_block[0]) {
		fprintf(stderr, "Failed - te2fm_namei(\"%s\") - p_inode->i_block[0]=%#x\n", s_path, p_inode->i_block[0]);
		errcnt++;
	}
	if (inono != 2) {
		fprintf(stderr, "Failed - te2fm_namei(\"%s\") - inono=%#x\n", s_path, inono);
		errcnt++;
	}
	
	strcpy(s_path, "/test2dir/hello3.txt");
	p_inode = te2fm_namei(&inono, &hnd_fsmdl, s_path);
	//printf("p_inode->i_block[0]=%#x\n", p_inode->i_block[0]);
	if (p_inode) {
		fprintf(stderr, "Failed - te2fm_namei(\"%s\") - p_inode != 0\n", s_path);
		errcnt++;
	}

	strcpy(s_path, "/testdir/helo3.txt");
	p_inode = te2fm_namei(&inono, &hnd_fsmdl, s_path);
	//printf("p_inode->i_block[0]=%#x\n", p_inode->i_block[0]);
	if (p_inode) {
		fprintf(stderr, "Failed - te2fm_namei(\"%s\") - p_inode != 0\n", s_path);
		errcnt++;
	}

	free(blkbuf);
	
	if (errcnt) {
		fputs("Test NG.", stderr);
		exit(-1);
	}
	puts("OK - test passed.");
	exit(0);
}

