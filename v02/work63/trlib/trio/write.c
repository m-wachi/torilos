#include <unistd.h>
#include "include/syscl01.h"

int write(int fd, const void *buf, size_t count ) {
	return syscall(0x04, fd, (unsigned int)buf, count, 0, 0, 0);
}
