#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {

	FILE *fpin;
	FILE *fpout;
	unsigned char buff[1024];
	size_t readsize;
	int i;

	if (argc < 3) {
		puts("usage: copy4k srcfile destfile");
		exit(1);
	}

	if (NULL == (fpin = fopen(argv[1], "rb"))) {
		printf("Couldn't open file: %s.n", argv[1]);
		return -1;
	}

	if (NULL == (fpout = fopen(argv[2], "wb"))) {
		printf("Couldn't open file: %s.n", argv[2]);
		return -2;
	}

	for (i=0; i<4; i++) {
		readsize = fread(buff, 1, 1024, fpin);
		fwrite(buff, 1, readsize, fpout);
	}
	fclose(fpout);

	fclose(fpin);
	exit(0);
}
