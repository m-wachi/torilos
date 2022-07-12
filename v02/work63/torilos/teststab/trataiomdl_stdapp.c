#include <torilos/trataiomdl.h>
#include <stdio.h>
#include <string.h>
#include <torilos_stab/trataiomdl_stdapp.h>

#define BLOCK_SIZE 1024

typedef struct memblock {
	char block[BLOCK_SIZE];
} MEMBLOCK;

static char* ml_diskfile;

static MEMBLOCK ml_memdisk[256];
static int ml_actual_disk_size;	// /kirobyte
/*
 * initialize module.
 * store diskimage on the memory
 * @filename - diskimage file
 * @disksize - diskimage size (/kb)
 */
int tatam_stdapp_init(char* filename, int disksize) {
	FILE* fp;
	size_t sz_read;
	int i;
	
	ml_diskfile = filename;
	
	return 0;
}


int tatam_readblock(unsigned char* dst, unsigned long lba) {
	unsigned int blk_no;
	FILE* fp;
	
	blk_no = lba / 2;

	if (!(fp = fopen(ml_diskfile, "r"))) {
		fprintf(stderr, "Error - couldn't open file. %s\n", ml_diskfile);
	}

	fseek(fp, BLOCK_SIZE * blk_no, SEEK_SET);
	
	fread(dst, BLOCK_SIZE, 1, fp);
	
	fclose(fp);
	
	return 0;
}

int tatam_writeblock(unsigned char* src, unsigned long lba) {
	unsigned int blk_no;
	FILE* fp;
	
	blk_no = lba / 2;

	if (!(fp = fopen(ml_diskfile, "r+"))) {
		fprintf(stderr, "Error - couldn't open file. %s\n", ml_diskfile);
		return -1;
	}

	if (fseek(fp, BLOCK_SIZE * blk_no, SEEK_SET)) {
		fprintf(stderr, "Error - fseek failed. %s\n", ml_diskfile);
		return -2;
	}
	
	fwrite(src, BLOCK_SIZE, 1, fp);
	
	fclose(fp);

	return 0;
	
}

int actual_sync(void) {
	return 0;
}
