#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <torilos/trext2fs.h>
#include <torilos/trblkbuf.h>
#include <torilos/trfile.h>

#define DISKBLOCK_SIZE 256

static unsigned char* g_diskimage;

void init_diskimage();
void init_superblock(struct ext2_super_block* p_ext2_sb);
void init_groupdesc(struct ext2_group_desc* p_ext2_gd);
void init_block_bitmap(unsigned char* block);
void init_inode_bitmap(unsigned char* block);
void init_inodetbl(struct ext2_inode* inodetbl);
void init_dirent_tbl(unsigned char* dirent_blk);

/*
TBLBF_BLKCELL_OLD* append_newblk(struct ext2_inode* p_inode, int blk_index,
							 TE2FS_HND_FS* hnd_fs);
*/

void bubble_sort(unsigned int[], int n);

void block_write_inodetbl(unsigned int start_blkno, struct ext2_inode* inodetbl, 
		int cnt_inode);



void block_write_dskimg(unsigned int blkno, unsigned char* buf);
void flush_dskimg(char* filename);

int main(int argc, char** argv) {
	FILE *fpin;
	struct ext2_super_block ext2_sb;
	struct ext2_group_desc ext2_gd;
	struct ext2_inode inodetbl[184];
	struct ext2_dir_entry_2 dirent_tbl[4];

	TE2FS_HND_FS hnd_fs;

	unsigned char buff[1024];
	unsigned char blkbmp[1024], inobmp[1024];
	unsigned char content[1024];
	unsigned char blankbuf[1024];
	unsigned char* blkbuf;

	int i, j, sz_blks, fd;
	/*
	TBLBF_BLKCELL_OLD* rootdir_blkcell;
	TBLBF_BLKCELL_OLD* p_blkcell;
	*/
	TBLBF_BLKCELL* rootdir_blkcell;
	TBLBF_BLKCELL* p_blkcell;
	
	unsigned int blknos[32];
	TFL_HND_FILE* user_fdtbl[16];

	if (argc < 2) {
		printf("usage mkfdimg_ext2 imgfile\n");
		exit(-1);
	}

	
	hnd_fs.sz_block = 1024;
	hnd_fs.blkbmp = blkbmp;
	hnd_fs.inobmp = inobmp;

	blkbuf = malloc(4096 * 16);
	init_diskimage();
	
	//tblbf_init_old(blkbuf, 16);
	tblbf_init(blkbuf, 16, 1024);
	
	
	memset(blankbuf, 0, 1024);

	//write bootblock
	//fwrite(buff, 1, 1024, fpout);

	//write super block
	init_superblock(&ext2_sb);
	//fwrite(&ext2_sb, 1, sizeof(ext2_sb), fpout);
	memset(buff, 0, 1024);
	memcpy(buff, &ext2_sb, 1024);
	block_write_dskimg(1, buff);

	//write group descriptor block
	init_groupdesc(&ext2_gd);
	memset(buff, 0, 1024);
	memcpy(buff, &ext2_gd, sizeof(ext2_gd));
	//fwrite(buff, 1, 1024, fpout);
	block_write_dskimg(2, buff);
	

	//write 3 to 7 block
	//memset(buff, 0, 1024);
	//fwrite(buff, 5, 1024, fpout);


	//init block bitmap and inode bitmap
	init_block_bitmap(blkbmp);
	init_inode_bitmap(inobmp);

	printf("blkbmp[4]=%#x\n", blkbmp[4]);
	printf("inobmp[1]=%#x\n", inobmp[1]);

	init_inodetbl(inodetbl);

	//init_dirent_tbl(dirent_tbl);
	
	//rootdir_blkcell = tblbf_get_blkcell_old(0x21);
	rootdir_blkcell = tblbf_alc_free_blkcell();
	rootdir_blkcell->blkno = 0x21;
	rootdir_blkcell->dirty = 1;
	
	init_dirent_tbl(rootdir_blkcell->blk);

	/****************
	 * prepare
	 ****************/
	tfl_init_userfd(user_fdtbl);

	printf("hnd_fs=%#x\n", &hnd_fs);

	tfl_init_filetbl(&hnd_fs);

	puts("preparation done.");

	for (i=2; i<argc; i++) {
		int readsize;
		int writesize;
		if (NULL == (fpin = fopen(argv[i], "rb"))) {
			printf("Couldn't open file: %s.\n", argv[i]);
			exit(-3);
		}

		fd = tfl_open(argv[i], rootdir_blkcell->blk, inodetbl, user_fdtbl);
		
		printf("open %s\n", argv[i]);

		while(readsize = fread(content, 1, 1024, fpin)) {
			printf("read %d bytes\n", readsize);
			writesize = tfl_write(fd, content, readsize, user_fdtbl);
		}
		tfl_close(fd, user_fdtbl);
		
		/*
		add_file(rootdir_blkcell->blk, argv[i], content,
				 &hnd_fs, readsize, inodetbl);
		*/
		fclose(fpin);
	}

	printf("blkbmp[4]=%#x\n", blkbmp[4]);
	printf("inobmp[1]=%#x\n", inobmp[1]);

	//write block bitmap
	//fwrite(blkbmp, 1, 1024, fpout);
	block_write_dskimg(8, blkbmp);
	//write inode bitmap
	//fwrite(inobmp, 1, 1024, fpout);
	block_write_dskimg(9, inobmp);

	//write inode block (blank)
	//fwrite(inodetbl, 1, sizeof(struct ext2_inode) * 184, fpout);
	block_write_inodetbl(10, inodetbl, 184);
	
	/*
	//write dirent_tbl[0]
	fwrite(&dirent_tbl[0], 1, dirent_tbl[0].rec_len, fpout);

	//write dirent_tbl[1]
	fwrite(&dirent_tbl[1], 1, dirent_tbl[1].rec_len, fpout);
	*/

	
	/*
	p_blkcell = tblbf_usedlist_head();
	i=0;
	while (p_blkcell) {
		printf("%03d: p_blkcell->blkno=%#x\n", i, p_blkcell->blkno);
		blknos[i] = p_blkcell->blkno;
		p_blkcell = p_blkcell->next;
		i++;
	}

	sz_blks = i;
	bubble_sort(blknos, sz_blks);
	
	for(i=0; i<sz_blks; i++) {
		printf("%03d: blkno=%#x\n", i, blknos[i]);
		//p_blkcell = tblbf_get_blkcell_old(blknos[i]);
		if (p_blkcell = tblbf_lookup_blkcell(blknos[i])) {
			fwrite(p_blkcell->blk, 1, 1024, fpout);
		} else {
			fwrite(blankbuf, 1, 1024, fpout);
		}

	}
	*/

	p_blkcell = tblbf_get_blkcell_dirty();
	while(p_blkcell) {
		printf("dirty: %#x\n", p_blkcell->blkno);
		block_write_dskimg(p_blkcell->blkno, p_blkcell->blk);
		p_blkcell->dirty = 0;
		p_blkcell = tblbf_get_blkcell_dirty();
	}

	
	//fclose(fpout);
	flush_dskimg(argv[1]);
	exit(0);

}



void init_superblock(struct ext2_super_block* p_ext2_sb) {

	memset(p_ext2_sb, 0, sizeof(struct ext2_super_block));

	p_ext2_sb->s_inodes_count = 184;
	p_ext2_sb->s_blocks_count = 1440;
	//34 means bb+sb+gd+blank(5) + blkbmp + inobmp + inoblk(23)
	// rootdirent will be allocate later.
	p_ext2_sb->s_free_blocks_count = 1440 - 34;
	p_ext2_sb->s_free_inodes_count = 183;
	p_ext2_sb->s_first_data_block = 1;
	p_ext2_sb->s_log_block_size = 0;
	p_ext2_sb->s_log_frag_size = 0;
	p_ext2_sb->s_blocks_per_group = 8192;
	p_ext2_sb->s_frags_per_group = 8192;
	p_ext2_sb->s_inodes_per_group = 184;
	p_ext2_sb->s_magic = 0xef53;
	p_ext2_sb->s_minor_rev_level = 0;
	p_ext2_sb->s_rev_level = 0;

}

void init_groupdesc(struct ext2_group_desc* p_ext2_gd) {

	memset(p_ext2_gd, 0, sizeof(struct ext2_group_desc));

	p_ext2_gd->bg_block_bitmap = 8;
	p_ext2_gd->bg_inode_bitmap = 9;
	p_ext2_gd->bg_inode_table = 10;
	p_ext2_gd->bg_free_blocks_count = 1440 - 34;
	p_ext2_gd->bg_free_inodes_count = 183;
	p_ext2_gd->bg_used_dirs_count = 1;

}

void init_block_bitmap(unsigned char* block) {
	int i;

	memset(block, 0xff, 1024);
	
	for(i=4; i<0xb4; i++) {
		block[i] = 0;
	}
	
	// 0x20, 0x21 blocks are already used
	// (last inode block, rootdirectory-entry)
	block[4] = 3;
	

}

void init_inode_bitmap(unsigned char* block) {
	int i;

	memset(block, 0xff, 1024);
	
	for(i=2; i<184/8; i++) {
		block[i] = 0;
	}
	
}

void init_inodetbl(struct ext2_inode* inodetbl) {
	int i;

	for (i=0; i<184; i++) {
		memset(&inodetbl[i], 0, sizeof(struct ext2_inode));
	}

	inodetbl[1].i_mode = 0x41ed;
	inodetbl[1].i_size = 0x400;
	inodetbl[1].i_blocks = 2;
	inodetbl[1].i_flags = 0;
	inodetbl[1].i_block[0] = 0x21;

}

void init_dirent_tbl(unsigned char* dirent_blk) {

	struct ext2_dir_entry_2* p_entry;
	unsigned char* p;
	int rest = 0x400;

	p = dirent_blk;
	p_entry = (struct ext2_dir_entry_2*)p;

	p_entry->inode = 0x02;
	p_entry->rec_len = 12;
	p_entry->name_len = 1;
	p_entry->file_type = 2;
	strcpy(p_entry->name, ".");

	rest -=	p_entry->rec_len;

	p += p_entry->rec_len;
	p_entry = (struct ext2_dir_entry_2*)p;

	p_entry->inode = 0x02;
	p_entry->rec_len = rest;
	p_entry->name_len = 2;
	p_entry->file_type = 2;
	strcpy(p_entry->name, "..");
	
}

void bubble_sort(unsigned int a[], int n) {
	unsigned int i, j, t;

	for(i=0; i<n-1; i++)
		for (j=n-1; j>i; j--)
			if (a[j-1] > a[j]) {
				t = a[j];
				a[j] = a[j-1];
				a[j-1] = t;
			}


}

void block_write_inodetbl(unsigned int start_blkno, struct ext2_inode* inodetbl, 
		int cnt_inode) {
	int sz_blk_inotbl;
	int i;
	unsigned char* p;
	
	sz_blk_inotbl = (sizeof(struct ext2_inode) * cnt_inode) / 1024;
	p = (unsigned char*)inodetbl;
	
	for (i=0; i<sz_blk_inotbl; i++) {
		block_write_dskimg(start_blkno + i, p);
		p += 1024;
	}
	
}



void init_diskimage() {
	g_diskimage = malloc(1024 * DISKBLOCK_SIZE);
	memset(g_diskimage, 0, 1024 * DISKBLOCK_SIZE);
	
}

void block_write_dskimg(unsigned int blkno, unsigned char* buf) {
	unsigned char* p;
	int i;
	p = g_diskimage;
	
	for (i=0; i<blkno; i++) {
		p += 1024;
	}
	
	memcpy(p, buf, 1024);
	
}

void flush_dskimg(char* filename) {
	FILE *fpout;

	if (NULL == (fpout = fopen(filename, "wb"))) {
		printf("Couldn't open file: %s.\n", filename);
		exit(-2);
	}
	
	fwrite(g_diskimage, 1024, DISKBLOCK_SIZE, fpout);
	
	fclose(fpout);
}

