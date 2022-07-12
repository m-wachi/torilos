#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trinclude/trutil.h"
#include "trinclude/syscl01.h"

int main(int argc, char** argv);

int main(int argc, char** argv) {

	unsigned long tick;
	//unsigned long wait_tick;
	char buff[80];

	//write string
	puts("start myapp09.");

	//get_tick
	syscall(0x17, (unsigned int)&tick, 0, 0, 0, 0, 0);

	sprint_varval(buff, "app9: cur tick", tick, 0);

	puts(buff);

	puts("app9: about to sleep 8sec.");
	
	syscall(0x08, 800, 0, 0, 0, 0, 0);

	puts("app9: waked up!!");

	//exit
	exit(13);
}

