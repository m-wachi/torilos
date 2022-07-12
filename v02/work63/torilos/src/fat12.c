#include <torilos/fat12.h>
#include <torilos/mylib01.h>

/*
 * get filename from directory entry
 */
void fat12_getfilename(FAT12_DIRENT* dirent, char* buff) {
	
	my_strncpy(buff, (char*)dirent->name, 8);
	buff+=8;
	my_strncpy(buff, (char*)dirent->ext, 3);
	buff+=3;
	*buff = '\0';
}

/*
 * extract fat12 (3byte->2entry)
 */
void fat12_extr_fat12_3b(unsigned short* high_clst, unsigned short* low_clst,
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

/*
 * compress fat12 (2entry->3byte)
 */
void fat12_cmpr_fat12_2e(unsigned char* tri_byte, 
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
int fat12_get_entry_size(int filesize) {
	int entry_size;
	entry_size = filesize / SECTOR_SIZE;
	if (0 < filesize % SECTOR_SIZE) entry_size++;
	
	return entry_size;
}

/* reserve cluster0 and cluster1 */
int fat12_tbl_init(FAT12_TBL* fat12tbl) {

	my_memset((char*)fat12tbl, 0, sizeof(FAT12_TBL));

	/* reserved entry */
	fat12tbl->entry[0] = 0xff0; 
	fat12tbl->entry[1] = 0xfff;

	return 2;
}

int fat12_fancy_append(FAT12_TBL* fat12tbl, int start, int entry_size) {
	int i;
	
	for (i=0; i<entry_size-1; i++) {
		fat12tbl->entry[start] = start + 1;
		start++;
	}
	fat12tbl->entry[start++] = 0xfff;

	return start;
}

/*
 * set filename to directory entry
 */
void fat12_set_filename(FAT12_DIRENT* dirent, char* filename) {
	int b_period = 0, ncnt=0, ecnt=0;
	char *p_name, *p_ext;

	my_memset(dirent->name, ' ', 8);
	my_memset(dirent->ext, ' ', 3);
	p_name = (char*)dirent->name;
	p_ext = (char*)dirent->ext;

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

/*
 * set directory entry
 * @dirent: target directory entry
 * @filename: filename
 * @clstr_no: entry cluster no
 * @size: file size
 */
void fat12_set_direntry(FAT12_DIRENT* dirent, char* filename, int clstr_no,
				  unsigned int size) {
	fat12_set_filename(dirent, filename);
	dirent->type=0x20;
	dirent->clstr_no = clstr_no;
	dirent->size = size;
}
