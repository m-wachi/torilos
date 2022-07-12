#include "include/syscl01.h"
#include <stdarg.h>

int ioctl(int fd, int cmd, ...) {
	va_list ap;
	int param1;
	
	va_start(ap, cmd);
	param1 = va_arg(ap, int);
	va_end(ap);
	return syscall(0x36, fd, cmd, param1, 0, 0, 0);
}

