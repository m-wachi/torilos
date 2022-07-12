#include <stdlib.h>

void exit(int sts) {
	syscall(0x01, sts, 0, 0, 0, 0, 0);
}
