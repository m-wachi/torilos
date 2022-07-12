#include <stdio.h>
#include <string.h>
//int puts(const char* s) {
//	syscall(0x014, (unsigned int)s, 0x43, 0x44, 0x45, 0x46, 0x47);
//	return 0;
//}

int puts(const char* s) {
	unsigned int l;
	char newline[2] = "\n";
	l = strlen(s);
	
	syscall(0x04, 1, (unsigned int)s, l, 0, 0, 0);
	
	syscall(0x04, 1, (unsigned int)newline, 1, 0, 0, 0);
}

