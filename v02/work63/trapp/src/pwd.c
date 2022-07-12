#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "trinclude/syscl01.h"
#include "trinclude/trutil.h"

int main(int argc, char** argv);

int main(int argc, char** argv) {

	char buff[80];
	char buff2[128];
	//unsigned int sz_read;
	int rc=0, fd;
	
	//puts("pwd start.");
	
	memset(buff2, 0, sizeof(buff2));
	
	//open
	//rc = getcwd(buff2, sizeof(buff2));
	
	rc = syscall(0xb7, (unsigned int)buff2, 128, 0, 0, 0, 0);
	
	if (rc < 0) {
		sprint_varval(buff, "Error - getcwd. rc", rc, 0);
		puts(buff);
		exit(0);
	}
	fd = rc;

	puts(buff2);
	
	exit(0);
}
