#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include "trinclude/syscl01.h"
#include "trinclude/trutil.h"

int main(int argc, char** argv);

int main(int argc, char** argv) {
	char buff[80];
	char buff2[1024];
	unsigned int cnt_read;
	int rc=0, fd;
	
	puts("myapp11 start.");

	//open
	//rc = open("/dev/tty0", O_RDONLY | O_NONBLOCK, 0);
	rc = open("/dev/tty0", O_RDONLY, 0);
	
	if (rc < 0) {
		sprint_varval(buff, "Error - couldn't open file. rc", rc, 0);
		puts(buff);
		exit(0);
	}
	fd = rc;

	memset(buff, 0, sizeof(buff));
	memset(buff2, 0, sizeof(buff2));

	cnt_read = 0;
	while (cnt_read < 5) {
		//read
		if(read(fd, buff, 1)) {
			buff2[cnt_read++] = buff[0];
			write(fd, buff, 1);
		}
	}
	close(fd);

	strcpy(buff, "data=");
	strcat(buff, buff2);
	
	puts(buff);
	
	puts("myapp11 end.");
	exit(0);
}
