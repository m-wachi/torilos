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
	char* p_newarea;
	
	puts("myapp1402 start.");

	sprint_varval(buff, "&end", (int)&end, 1);

	puts(buff);

	//rc = syscall(45, 0x404fff, 0, 0, 0, 0, 0);

	//p_newarea = (char*)0x404f00;

	p_newarea = sbrk(0x1200);
	
	sprint_varval(buff, "p_newarea", (int)p_newarea, 1);
	puts(buff);
	
	memset(p_newarea, 'a', 0x1200);
	
	exit(0);
}
