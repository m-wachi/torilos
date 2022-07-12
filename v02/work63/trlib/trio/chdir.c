#include <unistd.h>
#include "include/syscl01.h"

int chdir(const char *_path) {
	return syscall(0x0c, (unsigned int)_path, 0, 0, 0, 0, 0);
}

