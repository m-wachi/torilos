#ifndef FAT12_H
#define FAT12_H

#define SECTOR_SIZE 512
#define FAT_ENTRY_SIZE 3072   /* 9sector * 512byte * (2/3)byte/entry */
#define RAW_FAT_SIZE 4608
#define ROOT_DIRENT_SIZE 224
/* 
 * FAT12 directory entry
 */
typedef struct fat12_dirent {
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clstr_no;
	unsigned int size;
} FAT12_DIRENT;

typedef struct fat12_tbl {
	unsigned short entry[FAT_ENTRY_SIZE];
} FAT12_TBL;


void fat12_getfilename(FAT12_DIRENT* dirent, char* buff);
void fat12_extr_fat12_3b(unsigned short* high_clst, unsigned short* low_clst,
				   unsigned char* tri_byte);
void fat12_cmpr_fat12_2e(unsigned char* tri_byte, 
				   unsigned short high_clst, unsigned short low_clst);
int fat12_tbl_init(FAT12_TBL* fat12tbl);
int fat12_fancy_append(FAT12_TBL* fat12tbl, int start, int entry_size);
void fat12_set_filename(FAT12_DIRENT* dirent, char* filename);

void fat12_set_direntry(FAT12_DIRENT* dirent, char* filename, int clstr_no,
				  unsigned int size);



#endif
