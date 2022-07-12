#include <stdio.h>
#include <stdlib.h>
#include "trinclude/trutil.h"

int main(int argc, char** argv);

int main(int argc, char** argv) {
	char buff[80];
	
	puts("Hello myapp10.");
	
	siprintf(buff, "argc=%d\n", argc);
	
	puts(buff);
	
	exit(0);
	
}
