#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include "trinclude/syscl01.h"
#include "trinclude/trutil.h"

int main(int argc, char** argv);

/*
 * editor mode test.
 */
int main(int argc, char** argv) {
	char buff[80];
	char buff2[1024];
	char cmdmsg[5];
	int rc=0, fd, i;
	
	puts("myapp1801 start.");

	//open
	//rc = open("/dev/tty0", O_RDONLY, 0);
	rc = open("/dev/tty0", O_RDWR, 0);
	
	if (rc < 0) {
		sprint_varval(buff, "Error - couldn't open file. rc", rc, 0);
		puts(buff);
		exit(0);
	}
	fd = rc;

	ioctl(fd, 7, 0);

	memset(buff, 0, sizeof(buff));
	memset(buff2, 0, sizeof(buff2));

	//strcpy(buff2, "pH  pe  pl  pl  po  ");
	
	//rc = write(fd, buff2, strlen(buff2));
	strcpy(cmdmsg, "pH  ");
	rc = write(fd, cmdmsg, 4);
	strcpy(cmdmsg, "pe  ");
	rc = write(fd, cmdmsg, 4);
	strcpy(cmdmsg, "pl  ");
	rc = write(fd, cmdmsg, 4);
	strcpy(cmdmsg, "pl  ");
	rc = write(fd, cmdmsg, 4);
	strcpy(cmdmsg, "po  ");
	rc = write(fd, cmdmsg, 4);
	
	syscall(0x08, 200, 0, 0, 0, 0, 0);
	
	cmdmsg[0] = 'r';
	cmdmsg[1] = -2;
	cmdmsg[2] = 0;
	rc = write(fd, cmdmsg, 4);
	
	syscall(0x08, 500, 0, 0, 0, 0, 0);

	
	strcpy(cmdmsg, "px  ");
	rc = write(fd, cmdmsg, 4);
	strcpy(cmdmsg, "px  ");
	rc = write(fd, cmdmsg, 4);
	strcpy(cmdmsg, "p   ");
	rc = write(fd, cmdmsg, 4);
	
	strcpy(cmdmsg, "pm  ");
	rc = write(fd, cmdmsg, 4);
	strcpy(cmdmsg, "po  ");
	rc = write(fd, cmdmsg, 4);
	
	cmdmsg[0] = 'r';
	cmdmsg[1] = -2;
	cmdmsg[2] = 0;
	
	rc = write(fd, cmdmsg, 4);
	
	syscall(0x08, 700, 0, 0, 0, 0, 0);
	
	cmdmsg[0] = 'r';
	cmdmsg[1] = 0;
	cmdmsg[2] = 2;
	rc = write(fd, cmdmsg, 4);
	
	strcpy(cmdmsg, "py  ");
	rc = write(fd, cmdmsg, 4);
	strcpy(cmdmsg, "py  ");
	rc = write(fd, cmdmsg, 4);
	strcpy(cmdmsg, "p   ");
	rc = write(fd, cmdmsg, 4);
	
	strcpy(cmdmsg, "pn  ");
	rc = write(fd, cmdmsg, 4);
	strcpy(cmdmsg, "pk  ");
	rc = write(fd, cmdmsg, 4);
	
	syscall(0x08, 500, 0, 0, 0, 0, 0);
	
	for (i=0; i<10; i++) {
		read(fd, buff, 1);
		buff2[i] = buff[0];
	}
	
	ioctl(fd, 6, 0);

	close(fd);
	
	for (i=0; i<10; i++) {
		iprintf("buff[%d]=%#x\n", i, (unsigned char)buff2[i]);
	}
	
	puts("myapp1801 end.");
	exit(0);
}
