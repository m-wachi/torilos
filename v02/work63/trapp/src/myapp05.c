#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include "trinclude/syscl01.h"

int main(int argc, char** argv);

int main(int argc, char** argv) {
	int rc, fd;
	char buff[128], buff2[128];
	unsigned int sz_read;
	char msg[] = "This file has been written by myapp05.";
	
	puts("Hello App05.");
	
	if (argc < 2) {
		puts("not enough parameters");
		exit(0);
	}

	//open
	//rc = syscall(0x05, (unsigned int)argv[1], O_CREAT, 0x44, 0x45, 0x46, 0x47);
	rc = open(argv[1], O_CREAT, 0);
	
	sprint_varval(buff, "rc", rc, 0);
	puts(buff);

	if (rc < 0) {
		sprint_varval(buff, "Error - couldn't open file. rc", rc, 0);
		puts(buff);
		exit(0);
	}
	fd = rc;

	memset(buff2, 0, sizeof(buff2));
	//sz_read = tflm_read(fd, buff2, 500, user_fdtbl);

	//write
	rc = syscall(0x04, fd, (unsigned int)msg,	strlen(msg), 0x45, 0x46, 0x47);

	sprint_varval(buff, "rc", rc, 0);
	puts(buff);

	//close
	close(fd);
	
	exit(0);
}
