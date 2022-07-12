#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fat12.h"


int main(int argc, char** argv) {

	FILE *fpin;
	char buff[512], buff2[512], sectbuf[512];
	unsigned char raw_fat[512*3];
	char* p_char;
	char* addr2;
	size_t readsize, addsize, filesize;
	int blkno, i, clst_no;
	unsigned short unit1, unit2;

	FAT12_DIRENT *p_dirent;
	FAT12_DIRENT dirent1, dirent[16];

	if (argc!=2) {
		puts("not enough arguments");
		return -1;
	}

	if (NULL == (fpin = fopen(argv[1], "rb"))) {
		printf("Couldn't open file: %s.n", argv[1]);
		return -1;
	}

	blkno = 0;
	memset(raw_fat, 0, sizeof(raw_fat));
	while (0==feof(fpin)) {
		memset(sectbuf, 0, sizeof(sectbuf));
		readsize = fread(sectbuf, 1, sizeof(sectbuf), fpin);

		if (blkno == 1) {
			p_char = raw_fat;
			memcpy(p_char, sectbuf, sizeof(sectbuf));
		}
		if (blkno == 2) {
			p_char = raw_fat + 512;
			memcpy(p_char, sectbuf, sizeof(sectbuf));
		}
		if (blkno == 3) {
			p_char = raw_fat + 512*2;
			memcpy(p_char, sectbuf, sizeof(sectbuf));
		}
		if (blkno == 19) {
			//memset(buff, 0, sizeof(buff));
			//memcpy(buff, sectbuf, sizeof(buff));
			memcpy((char*)&dirent, sectbuf, sizeof(dirent));
		}
		blkno++;

	}

	
	fclose(fpin);
	
	/*
	getfilename(&dirent1, buff2);
	printf("entry1=%s, size=%d, clstr_no=%d\n", 
		   buff2, dirent1.size, dirent1.clstr_no);
	
	getfilename(&(dirent[2]), buff);
	printf("entry3=%s\n", buff);
	*/

	p_dirent = dirent;
	for (i=0; i<16; i++) {
		
		if (0x20 == p_dirent->type || 0x10 == p_dirent->type) {
			/* memset(buff2, 0, sizeof(buff2)); */
			getfilename(p_dirent, buff2);
		
			printf("entry[%d]=%s, type=%02x, size=%d, clstr_no=%03x\n", i,
				   buff2, p_dirent->type, p_dirent->size, p_dirent->clstr_no);

		}
		p_dirent++;
	}		

	for (i=0; i<16; i++) {
		printf("raw_fat[%d]=%02hhx\n", i, raw_fat[i]);
	}

	
	extr_fat12_3b(&unit1, &unit2, raw_fat);
	
	printf("unit1=%03hx, unit2=%03hx\n", unit1, unit2);

	p_char = raw_fat;
	p_char += 3;
	extr_fat12_3b(&unit1, &unit2, p_char);
	
	printf("unit1=%03hx, unit2=%03hx\n", unit1, unit2);


	p_char = raw_fat;
	for(i=0; i<8; i++) {
		
		extr_fat12_3b(&unit1, &unit2, p_char);
		printf("%02d: unit1=%03hx, unit2=%03hx\n", i, unit1, unit2);
		p_char += 3;
	}

	return 0;
}

