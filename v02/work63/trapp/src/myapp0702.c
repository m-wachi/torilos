#include "trinclude/syscl01.h"

int main(int argc, char** argv);


int main(int argc, char** argv) {

	//write string
	syscall(0x04, (unsigned int)"Hello myapp0702", 0x43, 0x44, 0x45, 0x46, 0x47);

	//exit
	syscall(0x01, 0, 0, 0, 0, 0, 0);

}




