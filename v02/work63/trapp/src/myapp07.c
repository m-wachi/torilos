#include <stdlib.h>
#include "trinclude/syscl01.h"

int main(int argc, char** argv);


int main(int argc, char** argv) {


	//write string
	syscall(0x04, 1, (unsigned int)"Hello MyApp07.\n", 15, 0x45, 0x46, 0x47);

	//exit
	exit(0);

}




