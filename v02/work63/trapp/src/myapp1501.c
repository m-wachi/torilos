#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include "trinclude/syscl01.h"
#include "trinclude/trutil.h"

int main(int argc, char** argv);

/*
 * character device write test app.
 */
int main(int argc, char** argv) {
	char buff[80];
	char buff2[1024];
	unsigned int cnt_read;
	int rc=0, fd;
	
	puts("myapp1501 start.");

	//open
	//rc = open("/dev/tty0", O_RDONLY, 0);
	rc = open("/dev/tty0", O_RDWR, 0);
	
	if (rc < 0) {
		sprint_varval(buff, "Error - couldn't open file. rc", rc, 0);
		puts(buff);
		exit(0);
	}
	fd = rc;

	memset(buff, 0, sizeof(buff));
	memset(buff2, 0, sizeof(buff2));

	strcpy(buff2, "Hello, This is myapp1501.\nThis data has been written to /dev/tty0.\n");
	
	rc = write(fd, buff2, strlen(buff2));
	
	sprint_varval(buff, "write(): rc", rc, 0);
	puts(buff);

	close(fd);

	puts("myapp1501 end.");
	exit(0);
}
