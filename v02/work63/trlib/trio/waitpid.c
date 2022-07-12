#include <sys/wait.h>
#include "include/syscl01.h"

pid_t waitpid (pid_t pid, int *status, int options) {
	return syscall(0x07, pid, (unsigned int)status, options, 0, 0, 0);
}
