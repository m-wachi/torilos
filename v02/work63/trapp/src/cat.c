#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include "trinclude/trutil.h"

int main(int argc, char** argv);

int main(int argc, char** argv) {
	//static char msg[] = "cat command execute."; 
	static char errmsg[] = "no arguments....";
	
	char buff[80];
	char buff2[1024];
	unsigned int sz_read;
	int rc=0, fd;
	
	//puts(msg);
	if (argc < 2) {
		puts(errmsg);
		exit(0);
	}

	rc = open(argv[1], O_RDONLY, 0);
	
	if (rc < 0) {
		sprint_varval(buff, "Error - couldn't open file. rc", rc, 0);
		puts(buff);
		exit(0);
	}
	fd = rc;

	memset(buff2, 0, sizeof(buff2));

	//read
	sz_read = read(fd, buff2, 500);
	
	while (sz_read) {
		puts(buff2);
		sz_read = read(fd, buff2, 500);
	} 
	
	//close
	close(fd);
	
	exit(0);
}
