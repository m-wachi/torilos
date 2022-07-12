#include <fcntl.h>
#include "include/syscl01.h"

//XXX This system call hasn't been implemented yet.
int fcntl(int fd, int cmd, ...) {
	return syscall(0x37, fd, cmd, 0, 0, 0, 0);
}

