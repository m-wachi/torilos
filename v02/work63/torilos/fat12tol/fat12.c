#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fat12.h"

void getfilename(FAT12_DIRENT* dirent, char* buff) {
	
	strncpy(buff, dirent->name, 8);
	buff+=8;
	strncpy(buff, dirent->ext, 3);
	buff+=3;
	*buff = '\0';
}

void extr_fat12_3b(unsigned short* high_clst, unsigned short* low_clst,
				   unsigned char* tri_byte) {
	
	unsigned short work1, work2;

	/* make high_clst value */
	work1 = tri_byte[1] & 0x0f;
	work1 = work1 << 8;
	*high_clst = work1 + (unsigned short)tri_byte[0];

	
	/* make low_clst value */
	work2 = tri_byte[1] & 0xf0;
	*low_clst = work2 >> 4;

	work2 = tri_byte[2];
	work2 = work2 << 4;
	*low_clst += work2;

}

void cmpr_fat12_2e(unsigned char* tri_byte, 
				   unsigned short high_clst, unsigned short low_clst) {
	
	unsigned short work1;

	/* make 1st byte */
	*tri_byte = 0x00ff & high_clst;
	tri_byte++;
	
	/* make 2nd byte */
	/* low 4bit */
	work1 = high_clst & 0x0f00;
	work1 = work1 >> 8;
	*tri_byte = work1;
	/* high 4bit */
	work1 = low_clst & 0x000f;
	work1 = work1 << 4;
	*tri_byte += work1;
	
	tri_byte++;

	/* make 3rd byte */
	work1 = low_clst & 0x0ff0;
	work1 = work1 >> 4;
	*tri_byte = work1;

}

/* calculate entry size */
int get_entry_size(int filesize) {
	int entry_size;
	entry_size = filesize / SECTOR_SIZE;
	if (0 < filesize % SECTOR_SIZE) entry_size++;
	
	return entry_size;
}

/* reserve cluster0 and cluster1 */
int fat12_tbl_init(FAT12_TBL* fat12tbl) {

	memset((char*)fat12tbl, 0, sizeof(FAT12_TBL));

	/* reserved entry */
	fat12tbl->entry[0] = 0xff0; 
	fat12tbl->entry[1] = 0xfff;

	return 2;
}

int fancy_append(FAT12_TBL* fat12tbl, int start, int entry_size) {
	int i;
	
	for (i=0; i<entry_size-1; i++) {
		fat12tbl->entry[start] = start + 1;
		start++;
	}
	fat12tbl->entry[start++] = 0xfff;

	return start;
}

void set_filename(FAT12_DIRENT* dirent, char* filename) {
	int b_period = 0, ncnt=0, ecnt=0;
	char *p_name, *p_ext;

	memset(dirent->name, ' ', 8);
	memset(dirent->ext, ' ', 3);
	p_name = dirent->name;
	p_ext = dirent->ext;

	while (*filename && ecnt < 3) {
		if ('.' == *filename) {
			b_period = 1;
			filename++;
		}
		
		if (b_period) {
			*p_ext = *filename;
			p_ext++; ecnt++;
		} else if (ncnt < 8) {
			*p_name = *filename;
			p_name++; ncnt++;
		}
		filename++;
	}
}

void set_direntry(FAT12_DIRENT* dirent, char* filename, int clstr_no,
				  unsigned int size) {
	set_filename(dirent, filename);
	dirent->type=0x20;
	dirent->clstr_no = clstr_no;
	dirent->size = size;
}
