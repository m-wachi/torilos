#include <unistd.h>
#include "include/syscl01.h"

off_t lseek(int _fildes, off_t _offset, int _whence) {
	return syscall(0x13, _fildes, _offset, _whence, 0, 0, 0);
}
