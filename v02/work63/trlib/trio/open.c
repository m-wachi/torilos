#include <sys/fcntl.h>
#include "include/syscl01.h"

extern int errno;

int open(const char* filename, int flag, ...) {
	int rc;
	rc = syscall(0x05, (unsigned int)filename, flag, 0, 0, 0, 0);
	if (rc<0) {
		errno = rc;
		rc = -1;
	}
	return rc;
}
