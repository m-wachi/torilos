#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <torilos/trext2fs.h>

int test_lookup_dirent(TE2FS_HND_FS* hnd_fs);
void print_dirent(struct ext2_dir_entry_2* p_entry);

int main(int argc, char** argv) {

	TE2FS_HND_FS hnd_fs;
	unsigned char blkbmp[1024], inobmp[1024], direntblk[1024];
	unsigned int rc_int;
	unsigned int ui_ret, inono;
	unsigned int sz_file = 0x2020;
	unsigned int req_blocks;
	//struct ext2_inode inode;
	struct ext2_dir_entry_2* p_dirent1;
	struct ext2_dir_entry_2* p_dirent2;
	struct ext2_dir_entry_2* p_dirent3;
	struct ext2_dir_entry_2* p_dirent4;
	int errcnt = 0;
	
	hnd_fs.sz_block = 1024;


	memset(blkbmp, 0, 1024);
	memset(inobmp, 0, 1024);
	
	blkbmp[0] = 0xff;
	blkbmp[1] = 0x5f;

	inobmp[0] = 0xff;
	inobmp[1] = 0xff;
	inobmp[2] = 0x01;

	hnd_fs.inobmp = inobmp;
	hnd_fs.blkbmp = blkbmp;

	//ui_ret = te2fs_free_inono(&hnd_fs);

	ui_ret = te2fs_notalc_bmp(inobmp, 1024);
	//printf("ui_ret=%#x\n", ui_ret);
	if (ui_ret != 0x12) {
		fprintf(stderr, "Failed - te2fs_notalc_bmp(). ui_ret=%#x\n", ui_ret);
		errcnt++;
	}
	
	
	inono = te2fs_notalc_inono(&hnd_fs);
	//printf("inono=%#x\n", inono);
	if (inono != 0x12) {
		fprintf(stderr, "Failed - te2fs_notalc_inono(). inono=%#x\n", inono);
		errcnt++;
	}
	
	
	req_blocks = te2fs_block_size(&hnd_fs, sz_file);
	//printf("sz_file=%d, req_blocks=%d\n", sz_file, req_blocks);

	
	te2fs_setbit_bmp(inobmp, inono, 1);
	//printf("inobmp[2]=%#x\n", inobmp[2]);
	if (inobmp[2] != 0x03) {
		fprintf(stderr, "Failed - te2fs_setbit_bmp() (1). inono=%#x, inobmp[2]=%#x\n", inono, inobmp[2]);
		errcnt++;
	}
	
	inono--;
	
	te2fs_setbit_bmp(inobmp, inono, 0);
	//printf("inono=%#x, inobmp[2]=%#x\n", inono, inobmp[2]);
	if (inobmp[2] != 0x02) {
		fprintf(stderr, "Failed - te2fs_setbit_bmp() (2). inono=%#x, inobmp[2]=%#x\n", inono, inobmp[2]);
		errcnt++;
	}
	
	inono = te2fs_alc_new_inono(&hnd_fs);
	if (inono != 0x11 || inobmp[2] != 0x03) {
		fprintf(stderr, "Failed - te2fs_alc_new_inono(). inono=%#x, inobmp[2]=%#x\n", inono, inobmp[2]);
		errcnt++;
	}
	
	te2fs_free_inono(&hnd_fs, inono);
	if (inobmp[2] != 0x02) {
		fprintf(stderr, "Failed - te2fs_free_inono(). inono=%#x, inobmp[2]=%#x\n", inono, inobmp[2]);
		errcnt++;
	}
	
	
	p_dirent1 = (struct ext2_dir_entry_2*)direntblk;

	p_dirent1->inode = 12;
	p_dirent1->rec_len = 1024;
	p_dirent1->name_len = 12;
	p_dirent1->file_type = EXT2_FT_REG_FILE;
	strcpy(p_dirent1->name, "hogehoge.txt");

	rc_int = te2fs_minsize_dirent(strlen("hogehoge.txt"));
	//printf("p_dirent1 minsize=%d\n", a);
	if (rc_int != 24) {
		fprintf(stderr, "Failed - te2fs_minsize_dirent(12). rc=%d\n", rc_int);
		errcnt++;
	}
	
	p_dirent2 = te2fs_alloc_dirent(&hnd_fs, direntblk, 132, 
								   EXT2_FT_REG_FILE, "hoggy.txt");
	puts("dirent1");
 	print_dirent(p_dirent1);

	puts("dirent2");
	print_dirent(p_dirent2);

	p_dirent3 = te2fs_alloc_dirent(&hnd_fs, direntblk, 133, 
								   EXT2_FT_REG_FILE, "dogger.txt");

	p_dirent4 = te2fs_alloc_dirent(&hnd_fs, direntblk, 134, 
								   EXT2_FT_REG_FILE, "maggy.txt");

	puts("--dirent1--");
	print_dirent(p_dirent1);
	puts("--dirent2--");
	print_dirent(p_dirent2);
	puts("--dirent3--");
	print_dirent(p_dirent3);
	puts("--dirent4--");
	print_dirent(p_dirent4);


	rc_int = te2fs_free_dirent(&hnd_fs, direntblk, "dogger.txt");
	puts("dogger.txt removed.");
	printf("removed inode=%d\n", rc_int);

	puts("--dirent2--");
	print_dirent(p_dirent2);

	puts("te2fs_alc_new_inono(&hnd_fs);");
	printf("before inobmp[2]=%#x\n", inobmp[2]);
	rc_int = te2fs_alc_new_inono(&hnd_fs);
	printf("after inobmp[2]=%#x\n", inobmp[2]);
	printf("new inode-no=%#x\n", rc_int);

	te2fs_free_inono(&hnd_fs, rc_int);
	printf("after te2fs_free_inono(%#x) inobmp[2]=%#x\n", rc_int, inobmp[2]);
	
	errcnt += test_lookup_dirent(&hnd_fs);

	if (errcnt) {
		fputs("Test NG", stderr);
		exit(-1);
	}
	
	puts("Test might be OK.");
	exit(0);
}

int test_lookup_dirent(TE2FS_HND_FS* hnd_fs) {
	int errcnt = 0;
	unsigned char direntblk[1024];
	struct ext2_dir_entry_2* p_dirent1;
	struct ext2_dir_entry_2* p_dirent2;
	struct ext2_dir_entry_2* p_dirent3;
	
	p_dirent1 = (struct ext2_dir_entry_2*)direntblk;

	p_dirent1->inode = 12;
	p_dirent1->rec_len = 1024;
	p_dirent1->name_len = 12;
	p_dirent1->file_type = EXT2_FT_REG_FILE;
	strcpy(p_dirent1->name, "hogehoge.txt");
	
	p_dirent2 = te2fs_alloc_dirent(hnd_fs, direntblk, 102, 
								   EXT2_FT_REG_FILE, "hoggy");
	
	p_dirent3 = te2fs_lookup_dirent("hoggy", direntblk, 1024);
	
	if (!p_dirent3) {
		errcnt++;
		fprintf(stderr, "Error - te2fs_lookup_dirent(): p_dirent3 should NOT be NULL\n");
	}

	p_dirent3 = te2fs_lookup_dirent("hoggy.txt", direntblk, 1024);
	
	if (p_dirent3) {
		errcnt++;
		fprintf(stderr, "Error - te2fs_lookup_dirent(): p_dirent3 should be NULL\n");
	}
	return errcnt;
}


void print_dirent(struct ext2_dir_entry_2* p_entry) {
	printf("inode_no=%d\n", p_entry->inode);
	printf("rec_len=%d\n", p_entry->rec_len);
	printf("name_len=%d\n", p_entry->name_len);
	printf("file_type=%d\n", p_entry->file_type);
	printf("name=%s\n", p_entry->name);

}
