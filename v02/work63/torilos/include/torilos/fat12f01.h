#ifndef FAT12F01_H
#define FAT12F01_H

#include "fat12.h"
#include "fdc01.h"

typedef struct cyl_hd_sec {
	int cyl;    /* cylinder no */
	int hd;     /* head address */
	int sec;    /* sector no */
} CYL_HD_SEC;

typedef struct hnd_fat12f_file {
	char* buff;
	CYL_HD_SEC chs[128];
	unsigned short clstr_no[128];
	int clstr_cnt;
} HND_FAT12F_FILE;

void clstr2chs(CYL_HD_SEC* chs, int clstr_no);
void fat12_extr_fat12(unsigned short* fat12_ext, unsigned char* fat12);
void fat12f_setup_hndl(HND_FAT12F_FILE* hnd_f, 
					   unsigned short start_clstr_no, 
					   unsigned short* fat12_ext);

void fat12f_open_file(HND_FAT12F_FILE* hnd_f, 
					  FAT12_DIRENT* fat12_dirent, 
					  unsigned short* fat12_ext);

void fat12f_freefile(HND_FAT12F_FILE* hnd_f);

void print_hnd_fat12f_file(HND_FAT12F_FILE* hnd_f);


#endif
