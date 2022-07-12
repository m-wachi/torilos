#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <torilos/ext2fs.h>

void readblock(unsigned char* buff, int sz_block, int block_no);
void print_inodeblock(unsigned char* block, int sz_block, int start_ino);
void print_direntry(unsigned char* block);
unsigned int inode_block_no(unsigned int start_inode_block,
							unsigned int inode_no, int sz_block);
int inode_index(unsigned int inode_no, int sz_block);
int lookup_inode2(struct ext2_inode* p_inode, int idx_inode, 
				  unsigned char* block);
void print_inode(struct ext2_inode* p_inode, unsigned int inode_no);
struct ext2_dir_entry_2* lookup_direntry(char* filename, 
										 unsigned char* dirent_block, 
										 unsigned int sz_block);

struct ext2_dir_entry_2* next_direntry(struct ext2_dir_entry_2* p_entry, 
									   unsigned char* dirent_block, 
									   unsigned int sz_block);
	

void print_dirent(struct ext2_dir_entry_2* p_entry);
void print_superblock(struct ext2_super_block* p_ext2_sb);
void print_groupdesc(struct ext2_group_desc* p_ext2_gd);

static char ml_diskfile[80];


int main(int argc, char** argv) {

	FILE *fpin;
	unsigned char buff[512], buff2[512], sectbuf[512], buff3[1024];
	unsigned char block1[1024], block2[1024], block_a[1024], block_b[1024],
		block_c[1024];
	unsigned char block_sb[1024];

	size_t readsize, addsize, filesize;
	int i, j, cur_blk_no=0, idx_dsctbl_ino, idx_rootdir_ino;
	unsigned int dsctbl_inoblk_no, rootdir_inoblk_no;
	int cnt_bg;

	struct ext2_super_block ext2_sb;
	struct ext2_super_block* p_ext2_sb;
	struct ext2_super_block* p_ext2_sb_bk;
	struct ext2_group_desc* p_ext2_gd;
	struct ext2_inode* p_inode;
	struct ext2_inode root_inode, dsctbl01_inode;
	struct ext2_dir_entry_2* p_entry;

	if (argc < 2) {
		printf("usage: %s diskfile\n", argv[0]);
		exit(-1);
	}

	strcpy(ml_diskfile, argv[1]);

	if (NULL == (fpin = fopen(ml_diskfile, "rb"))) {
		printf("Couldn't open file: %s.n", ml_diskfile);
		return -1;
	}

	// skip first 2 sector
	readsize = fread(buff, 1, 512, fpin);
	readsize = fread(buff, 1, 512, fpin);

	cur_blk_no = 1;
	//readsize = fread(buff, 1, 512, fpin);

	printf("ext2 superblock size = %d\n", sizeof(ext2_sb));
	printf("ext2 group descriptor size = %d\n", sizeof(struct ext2_group_desc));
	printf("ext2 inode size = %d\n", sizeof(struct ext2_inode));

	readsize = fread(block1, 1, 1024, fpin);
	
	p_ext2_sb = (struct ext2_super_block*)block1;

	puts("super block information.");
	print_superblock(p_ext2_sb);

	cnt_bg = p_ext2_sb->s_blocks_count / p_ext2_sb->s_blocks_per_group;

	printf("max block group=%d\n", cnt_bg);
		   

	cur_blk_no++;
	readsize = fread(block2, 1, 1024, fpin);

	p_ext2_gd = (struct ext2_group_desc*)block2;

	printf("block2 contents\n");
	for (i=0; i<32; i++) {
		printf("group %d\n", i);
		print_groupdesc(p_ext2_gd);
		p_ext2_gd++;
		if (!p_ext2_gd->bg_block_bitmap)
			break;
	}

	if (cnt_bg >= 32) {
		readsize = fread(block2, 1, 1024, fpin);

		p_ext2_gd = (struct ext2_group_desc*)block2;

		printf("block3 contents\n");
		for (i=0; i<32; i++) {
			printf("group %d\n", i+32);
			print_groupdesc(p_ext2_gd);
			p_ext2_gd++;
		}
	}

	fclose(fpin);

	/*
	readblock(block_sb, 1024, p_ext2_sb->s_blocks_per_group+1);
	
	p_ext2_sb_bk = (struct ext2_super_block*)block_sb;

	puts("backup super block information.");
	print_superblock(p_ext2_sb_bk);
	*/

	printf("inode size=%d\n", sizeof(struct ext2_inode));
	printf("count of inode-block = %d\n", p_ext2_sb->s_inodes_per_group / 8);
	puts("###########################################");
	puts("inode No looks like starting with 1(not 0).");
	puts("###########################################");

	p_ext2_gd = (struct ext2_group_desc*)block2;
	readblock(block_a, 1024, p_ext2_gd->bg_inode_table);

	print_inodeblock(block_a, 1024, 1);

	lookup_inode2(&root_inode, 1, block_a);

	//lookup_inode(&root_inode, 2, block_a, 1024, 0);

	readblock(block_a, 1024, p_ext2_gd->bg_inode_table+1);

	print_inodeblock(block_a, 1024, 9);

	readblock(block_a, 1024, p_ext2_gd->bg_inode_table+2);

	print_inodeblock(block_a, 1024, 17);


	puts("root inode");
	print_inode(&root_inode, 2);

	readblock(block_a, 1024, root_inode.i_block[0]);
	
	print_direntry(block_a);

	/*
	puts("next_direntry test");

	p_entry = (struct ext2_dir_entry_2*)block_a;

	while(p_entry = next_direntry(p_entry, block_a, 1024)) {
		print_dirent(p_entry);
	}		

	puts("lookup_direntry test - fail");

	p_entry = lookup_direntry("dsctbl02.h", block_a, 1024);

	if (!p_entry) {
		puts("test success, lookup failed.");
	}

	puts("lookup_direntry test - success");

	p_entry = lookup_direntry("dsctbl01.h", block_a, 1024);

	print_dirent(p_entry);

	
	dsctbl_inoblk_no = inode_block_no(10, p_entry->inode, 1024);
	printf("dsctbl_inoblk_no=%#x\n", dsctbl_inoblk_no);

	readblock(block_b, 1024, dsctbl_inoblk_no);

	idx_dsctbl_ino = inode_index(p_entry->inode, 1024);
	printf("idx_dsctbl_ino=%d\n", idx_dsctbl_ino);

	lookup_inode2(&dsctbl01_inode, idx_dsctbl_ino, block_b);

	print_inode(&dsctbl01_inode, p_entry->inode);

	readblock(block_b, 1024, dsctbl01_inode.i_block[0]);
	
	puts("dsctbl01.h start -->");
	puts(block_b);
	puts("<-- end dsctbl01.h");
	*/
	return 0;
}


void readblock(unsigned char* buff, int sz_block, int block_no) {
	FILE *fpin;
	unsigned char *p_temp;
	int i;
	size_t readsize;

	p_temp = malloc(sz_block);

	if (NULL == (fpin = fopen(ml_diskfile, "rb"))) {
		printf("Couldn't open file: %s.n", "floppy_elf.img");
		return;
	}

	readsize = fread(p_temp, 1, sz_block, fpin);

	for(i=0; i<block_no-1; i++) {
		readsize = fread(p_temp, 1, sz_block, fpin);
	}
	readsize = fread(buff, 1, sz_block, fpin);

	fclose(fpin);
	
}

void print_inodeblock(unsigned char* block, int sz_block, int start_ino) {
	int i, j, max;
	struct ext2_inode* p_inode;

	p_inode = (struct ext2_inode*)block;
	
	max = sz_block / 128;

	for(i=0; i<max; i++) {
		if (p_inode->i_size) {

			print_inode(p_inode, i+start_ino);
		}
		p_inode++;
	}
}

void print_direntry(unsigned char* block) {
	struct ext2_dir_entry_2* p_entry;
	unsigned char* p;
	int pos = 0;
	char buff[80];
	
	p = block;

	while(pos < 1024) {
		p_entry = (struct ext2_dir_entry_2*)p;
		printf("inode_no=%d\n", p_entry->inode);
		printf("rec_len=%d\n", p_entry->rec_len);
		printf("name_len=%d\n", p_entry->name_len);
		printf("file_type=%d\n", p_entry->file_type);
		strncpy(buff, p_entry->name, p_entry->name_len);
		buff[p_entry->name_len] = 0;
		printf("name=%s\n", buff);
		puts("");

		p+=p_entry->rec_len;
		pos+=p_entry->rec_len;
	}

}

unsigned int inode_block_no(unsigned int start_inode_block,
							unsigned int inode_no, int sz_block) {
	
	return ((inode_no - 1)/ (sz_block / 128)) + start_inode_block;
}

int inode_index(unsigned int inode_no, int sz_block) {
	return (inode_no - 1) % (sz_block / 128);
}


int lookup_inode2(struct ext2_inode* p_inode, int idx_inode, 
				  unsigned char* block) {
	int i, j, max;
	unsigned int start_ino;
	struct ext2_inode* p_srcinode;
	//unsigned char *p;

	p_srcinode = (struct ext2_inode*)block;
	//start_ino = (sz_block / 128) * inode_block_no + 1;
	p_srcinode += idx_inode;
	memcpy(p_inode, p_srcinode, 128);

	return 0;
}

void print_inode(struct ext2_inode* p_inode, unsigned int inode_no) {
	int j;

	printf("=== i_no=%d ===\n", inode_no);
	printf("inode: i_mode=%#x\n", (unsigned short)p_inode->i_mode);
	printf("inode: i_uid=%#x\n", p_inode->i_uid);
	printf("inode: i_size=%#x\n", p_inode->i_size);
	printf("inode: i_blocks=%#x\n", p_inode->i_blocks);
	printf("inode: i_flags=%#x\n", p_inode->i_flags);
	printf("inode: i_links_count=%#x\n", p_inode->i_links_count);
	printf("inode: i_file_acl=%#x\n", p_inode->i_file_acl);
	printf("inode: i_dir_acl=%#x\n", p_inode->i_dir_acl);
	printf("inode: i_gid=%#x\n", p_inode->i_gid);
	for(j=0; j<EXT2_N_BLOCKS; j++) {
		printf("inode: i_block[%d]=%#x\n", j, p_inode->i_block[j]);
		if (j > (p_inode->i_blocks / 2)) break;
	}

}

struct ext2_dir_entry_2* lookup_direntry(char* filename, 
										 unsigned char* dirent_block, 
										 unsigned int sz_block) {
	struct ext2_dir_entry_2* p_entry;

	p_entry = (struct ext2_dir_entry_2*)dirent_block;

	while(p_entry = next_direntry(p_entry, dirent_block, sz_block)) {
		if (!strcmp(p_entry->name, filename))
			return p_entry;
	}		
	return 0;

}

struct ext2_dir_entry_2* next_direntry(struct ext2_dir_entry_2* p_entry, 
									   unsigned char* dirent_block, 
									   unsigned int sz_block) {
	
	unsigned char* p;

	p = (unsigned char*)p_entry;
	p += p_entry->rec_len;
	if (p == dirent_block + sz_block)
		return 0;

	return (struct ext2_dir_entry_2*)p;

}

void print_dirent(struct ext2_dir_entry_2* p_entry) {
	printf("inode_no=%d\n", p_entry->inode);
	printf("rec_len=%d\n", p_entry->rec_len);
	printf("name_len=%d\n", p_entry->name_len);
	printf("file_type=%d\n", p_entry->file_type);
	printf("name=%s\n", p_entry->name);

}

void print_superblock(struct ext2_super_block* p_ext2_sb) {
	printf("ext2_fs: s_inodes_count=%d\n", p_ext2_sb->s_inodes_count);
	printf("ext2_fs: s_blocks_count=%d\n", p_ext2_sb->s_blocks_count);
	printf("ext2_fs: s_first_data_block=%d\n", p_ext2_sb->s_first_data_block);
	printf("ext2_fs: s_free_blocks_count=%d\n", p_ext2_sb->s_free_blocks_count);
	printf("ext2_fs: s_free_inodes_count=%d\n", p_ext2_sb->s_free_inodes_count);
	printf("ext2_fs: s_log_block_size=%d\n", p_ext2_sb->s_log_block_size);
	printf("ext2_fs: s_blocks_per_group=%d\n", p_ext2_sb->s_blocks_per_group);
	printf("ext2_fs: s_inodes_per_group=%d\n", p_ext2_sb->s_inodes_per_group);
	printf("ext2_fs: s_mnt_count=%d\n", (unsigned short)p_ext2_sb->s_mnt_count);
	printf("ext2_fs: s_max_mnt_count=%d\n", (unsigned short)p_ext2_sb->s_max_mnt_count);
	printf("ext2_fs: s_magic=%#x\n", (unsigned short)p_ext2_sb->s_magic);
	printf("ext2_fs: s_rev_level=%d\n", p_ext2_sb->s_rev_level);
	printf("ext2_fs: s_minor_rev_level=%d\n", p_ext2_sb->s_minor_rev_level);
}

void print_groupdesc(struct ext2_group_desc* p_ext2_gd) {
	printf("ext2_fs: bg_block_bitmap=%#x\n", p_ext2_gd->bg_block_bitmap);
	printf("ext2_fs: bg_inode_bitmap=%#x\n", p_ext2_gd->bg_inode_bitmap);
	printf("ext2_fs: bg_inode_table=%#x\n", p_ext2_gd->bg_inode_table);
}
