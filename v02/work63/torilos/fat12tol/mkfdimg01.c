#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fat12.h"

#define BUFFSIZE 1024
#define MAX_INFILE_SIZE 16


int write512u(FILE* fpout, char* in_fname);
long getfilesize(char* filename);

int main(int argc, char** argv) {

	int i, infile_cnt, j;
	long size;
	FILE *fpout, *fpboot, *fpin;

	char buff[BUFFSIZE];
	char* p_fname;
	char* p_raw_fat; 
	size_t readsize, addsize, filesize;
	int next_entry_index;
	int clstr_no;
	FAT12_DIRENT dirent1;
	FAT12_DIRENT dirent[MAX_INFILE_SIZE];
	FAT12_DIRENT *p_dirent;
	char raw_fat[RAW_FAT_SIZE];
	int entry_size;
	FAT12_TBL fat12tbl;

	char* test01 = "12";
	int targetsize;


	if (argc<4) {
		puts("usage: mkfdimg01 outfile bootsectfile infile1 [infile2..]");
		puts("    outfile :      output file name");
		puts("    bootsectfile : bootsector file");
		puts("    infile :       input file name");
		return 0;
	}
	infile_cnt = argc - 3;
	//targetsize = atoi(argv[3]);

	printf("out=%s, bootsectfile=%s, infile_cnt=%d\n", 
		   argv[1], argv[2], infile_cnt);

	if (NULL == (fpout = fopen(argv[1], "wb"))) {
		printf("Couldn't open file: %s.n", argv[1]);
		return -1;
	}


	if (NULL == (fpboot = fopen(argv[2], "rb"))) {
		printf("Couldn't open file: %s.n", argv[2]);
		return -1;
	}



	/********************
	 * copy bootsector
	 ********************/
	memset(buff, 0, sizeof(buff));
  
	readsize = fread(buff, 1, sizeof(buff), fpboot);
	if (readsize != 512) {
		puts("incorrect bootsector");
		fclose(fpboot);
		fclose(fpout);
		exit(-1);
	}
	fwrite(buff, 1, readsize, fpout);
		
	fclose(fpboot);

	p_dirent = dirent;

	/* init FAT12_TBL */
	next_entry_index = fat12_tbl_init(&fat12tbl);

	for(i=0; i<infile_cnt; i++) {

		filesize = 0;

		p_fname = argv[3+i];

		filesize = getfilesize(p_fname);
	
		//printf("%s size=%d\n", p_fname, filesize);


		/* calculate entry size */
		entry_size = get_entry_size(filesize);
	
		//printf("entry_size=%d\n", entry_size);

		clstr_no = next_entry_index;

		/* mark fat12 entry */
		next_entry_index = fancy_append(&fat12tbl, next_entry_index, entry_size);

		memset(p_dirent, 0, sizeof(FAT12_DIRENT));

		//set_filename(&dirent1, argv[3]);
		set_direntry(p_dirent, p_fname, clstr_no, filesize);

		//memset(buff, 0, sizeof(buff));
		//getfilename(p_dirent, buff);
		//printf("file=%s\n", buff);
		p_dirent++;
	}
#ifdef DEBUG
	for (i=0; i<128; i++) {
		if (0==i%8) {
			printf("%03x: ", i);
		}
		if (7==i%8) {
			printf("%03hx\n", fat12tbl.entry[i]);
		} else {
			printf("%03hx, ", fat12tbl.entry[i]);
		}

	}
#endif

	p_raw_fat = raw_fat;
	
	/* compress FAT */
	for(i=0; i<FAT_ENTRY_SIZE/2; i++) {
		cmpr_fat12_2e(p_raw_fat, fat12tbl.entry[i*2], fat12tbl.entry[i*2+1]);
		p_raw_fat+=3;
	}

#ifdef DEBUG
	puts("=====raw_fat=====");
	for (i=0; i<128; i++) {
		if (0==i%8) {
			printf("%03x: ", i);
		}
		if (7==i%8) {
			printf("%02hhx\n", raw_fat[i]);
		} else {
			printf("%02hhx, ", raw_fat[i]);
		}

	}
#endif
	/* write raw_fat twice */
	for (i=0; i<2; i++) {
		for (j=0; j<RAW_FAT_SIZE; j++) {
			fputc(raw_fat[j], fpout);
		}
	}
	

	memset(buff, 0, 32); /* blank entry */

	/* write directory entry */
	p_dirent = dirent;
	for(i=0; i<ROOT_DIRENT_SIZE; i++) {
		if (i<infile_cnt) {
			fwrite(p_dirent, 1, 32, fpout);
			p_dirent++;
		} else {
			fwrite(buff, 1, 32, fpout);
		}
	}

	/* write file */
	for(i=0; i<infile_cnt; i++) {
		p_fname = argv[3+i];

		/*
		if (NULL == (fpin = fopen(p_fname, "rb"))) {
			printf("Couldn't open file: %s.n", p_fname);
			return -1;
		}
		
		while (0==feof(fpin)) {

			readsize = fread(buff, 1, sizeof(buff), fpin);

			fwrite(buff, 1, readsize, fpout);

		}
		fclose(fpin);
		*/
		if(write512u(fpout, p_fname))
			return -1;
	}
	
	fclose(fpout);

	return 0;
}

/*
 * write 512 byte unit 
 */
int write512u(FILE* fpout, char* in_fname) {
	FILE *fpin;
	size_t readsize, totalsize=0, modsize;
	char buff[BUFFSIZE];

	if (NULL == (fpin = fopen(in_fname, "rb"))) {
		printf("Couldn't open file: %s.n", in_fname);
		return -1;
	}
		
	while (0==feof(fpin)) {

		readsize = fread(buff, 1, sizeof(buff), fpin);

		fwrite(buff, 1, readsize, fpout);
		totalsize += readsize;
	}

	fclose(fpin);

	memset(buff, 0, sizeof(buff));

	modsize = totalsize % 512;
	if (modsize > 0) {
		fwrite(buff, 1, 512 - modsize, fpout);
	}
	return 0;
}

long getfilesize(char* filename) {

	FILE *fp;
	char buff[BUFFSIZE];
	long readsize, filesize;

	if (NULL == (fp = fopen(filename, "rb"))) {
		printf("Couldn't open file: %s.n", filename);
		return -1;
	}

	filesize = 0;

	while (0==feof(fp)) {

		memset(buff, 0, sizeof(buff));
  
		readsize = fread(buff, 1, sizeof(buff), fp);

		filesize += readsize;

	}

	fclose(fp);
  
	return filesize;
}
