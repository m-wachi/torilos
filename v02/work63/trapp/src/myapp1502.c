#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include "trinclude/syscl01.h"
#include "trinclude/trutil.h"

int main(int argc, char** argv);

/*
 * stdin, stdout, stderr and dup() test01.
 */
int main(int argc, char** argv) {
	char buff[80];
	char buff2[1024];
	static char msg[] = "Hello, This is myapp1502.\nThis data has been written to /dev/tty0.\n"; 
	unsigned int cnt_read;
	int rc=0, i, fd[3];
	
	
	puts("myapp1501 start.");

	puts("type 5 character to /dev/tty0 (stdin read).");
	
	rc = open("/dev/tty0", O_RDWR, 0);
	
	if (rc < 0) {
		sprint_varval(buff, "Error - couldn't open file. rc", rc, 0);
		puts(buff);
		exit(0);
	}
	fd[0] = rc;

	for (i=1; i<=2; i++) {
		//dup
		rc = syscall(0x29, fd[0], 0, 0, 0, 0, 0);
		if (rc < 0) {
			sprint_varval(buff, "Error - couldn't dup file. rc", rc, 0);
			puts(buff);
			exit(0);
		}
		fd[i] = rc;
	}
	memset(buff, 0, sizeof(buff));
	memset(buff2, 0, sizeof(buff2));

	cnt_read = 0;
	while (cnt_read < 5) {
		//read
		if(read(fd[0], buff, 1)) {
			buff2[cnt_read++] = buff[0];
		}
	}
	buff2[cnt_read] = 0;
	
	strcpy(buff, "typed data=[");
	strcat(buff, buff2);
	strcat(buff, "].");
	puts(buff);
	
	strcpy(buff2, "write() to stdout.\n");
	strcat(buff2, msg);
	rc = write(fd[1], buff2, strlen(buff2));
	
	sprint_varval(buff, "write(): rc", rc, 0);
	puts(buff);

	strcpy(buff2, "write() to stderr.\n");
	strcat(buff2, msg);
	rc = write(fd[2], buff2, strlen(buff2));
	
	sprint_varval(buff, "write(): rc", rc, 0);
	puts(buff);

	for (i=0; i<3; i++) {
		close(fd[i]);
	}
	
	puts("myapp1502 end.");
	exit(0);
}
