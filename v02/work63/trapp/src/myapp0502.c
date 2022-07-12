#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include "trinclude/syscl01.h"

int main(int argc, char** argv);

// write test.
int main(int argc, char** argv) {
	int rc, fd;
	char buff2[128];
	char msg[] = "This file has been written by myapp0502.\n";
	
	puts("Hello App0502.");
	
	if (argc < 3) {
		puts("not enough parameters");
		exit(0);
	}

	//open
	rc = open(argv[1], O_CREAT | O_WRONLY, 0);
	
	iprintf("rc=%d\n", rc);

	if (rc < 0) {
		//sprint_varval(buff, "Error - couldn't open file. rc", rc, 0);
		//puts(buff);
		iprintf("Error - couldn't open file. rc=%d\n", rc);
		exit(0);
	}
	fd = rc;

	memset(buff2, 0, sizeof(buff2));
	strcpy(buff2, argv[2]);
	//write
	//rc = syscall(0x04, fd, (unsigned int)msg,	strlen(msg), 0x45, 0x46, 0x47);
	write(fd, msg, strlen(msg));
	write(fd, buff2, strlen(buff2));
	
	iprintf("rc=%d\n", rc);

	//close
	close(fd);
	
	exit(0);
}
