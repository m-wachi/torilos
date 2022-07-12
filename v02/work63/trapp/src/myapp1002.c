#include <stdio.h>
#include <stdlib.h>
#include "trinclude/trutil.h"

int main(int argc, char** argv);

int main(int argc, char** argv) {
	char buff[80], buff2[80];
	double dval = 1.23;
	
	puts("Hello myapp1002.");
	
	dval += 4.7;
	
	sprintf(buff, "dval=%f\n", dval);
	
	puts(buff);
	
	exit(0);
	
}
