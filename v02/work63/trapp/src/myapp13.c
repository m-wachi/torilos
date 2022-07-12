#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include "trinclude/syscl01.h"
#include "trinclude/trutil.h"

extern char** environ;

int main(int argc, char** argv);

int main(int argc, char** argv) {
	int rc;
	char* lclargv[4];
	
	puts("myapp13 start.");

	lclargv[0] = "myapp06.elf";
	lclargv[1] = "pr1";
	lclargv[2] = "param2";
	lclargv[3] = 0;

	rc = syscall(11, (unsigned int)lclargv, (unsigned int)environ, 0, 0, 0, 0);
	
	exit(0);
}
