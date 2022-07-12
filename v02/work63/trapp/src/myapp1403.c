#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/fcntl.h>
//#include <sys/unistd.h>
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

	//p_newarea = sbrk(0x1200);
	
	p_newarea = malloc(0x1200);
	
	sprint_varval(buff, "p_newarea", (int)p_newarea, 1);
	puts(buff);
	
	memset(p_newarea, 'b', 0x1200);
	
	free(p_newarea);
	
	exit(0);
}
