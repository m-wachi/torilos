#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
	FILE* fp;
	char buff[256];
	int i;
	
	fp = fopen("hello2.txt", "r");
	
	memset(buff, 0, sizeof(256));
	
	for (i=0; i<10; i++) { 
		
		buff[i] = fgetc(fp);
	}
	puts(buff);
	
	fclose(fp);

	fp = fopen("not_exists.txt", "r");
	
	if (fp == 0) {
		puts("OK.");
	} else {
		puts("NG. not return NULL.");
	}
	
	exit(0);
}
