#include <unistd.h>
#include "include/syscl01.h"

int read(int fd, void *buf, size_t count ) {
	return syscall(0x03, fd, (unsigned int)buf, count, 0, 0, 0);
}
