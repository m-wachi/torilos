#include <unistd.h>
#include "include/syscl01.h"

int close(int fd) {
	syscall(0x06, fd, 0, 0, 0, 0, 0);
	return 0;
}
