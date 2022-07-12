#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include "trinclude/syscl01.h"
#include "trinclude/trutil.h"

extern char** environ;
extern int end;

int main(int argc, char** argv);

int main(int argc, char** argv) {
	char buff[80];
	int rc;
	char* p_newarea;
	
	puts("myapp14 start.");

	sprint_varval(buff, "&end", (int)&end, 1);

	puts(buff);

	rc = syscall(45, 0x404fff, 0, 0, 0, 0, 0);
	
	p_newarea = (char*)0x404f00;
	strcpy(p_newarea, "This is new data area.");
	
	puts(p_newarea);
	
	exit(0);
}
