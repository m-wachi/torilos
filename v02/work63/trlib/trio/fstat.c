#include <sys/stat.h>
#include "include/syscl01.h"

int fstat(int _fd, struct stat *_sbuf) {
	return syscall(0x1c, _fd, (int)_sbuf, 0, 0, 0, 0);
}
