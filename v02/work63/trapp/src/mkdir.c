#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "trinclude/trutil.h"
#include <stdio.h>

int main(int argc, char** argv);

int main(int argc, char** argv) {
	char buff[128];
	
	if (argc < 2) {
		puts("usage: mkdir (dirname)");
		exit(0);
	}
	
	strcpy(buff, "dir=");
	strcat(buff, argv[1]);
	puts(buff);
	
	//syscall(0x27, (unsigned int)argv[1], 1, 0, 0, 0, 0);
	mkdir(argv[1], 0);
	
	exit(0);
}
