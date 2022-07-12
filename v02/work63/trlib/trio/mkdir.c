#include <sys/stat.h>
#include "include/syscl01.h"

int mkdir(const char *path, mode_t mode) {
	return syscall(0x27, (unsigned int)path, mode, 0, 0, 0, 0);
}

