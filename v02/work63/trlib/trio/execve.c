#include <unistd.h>
#include "include/syscl01.h"

int execve(const char *_path, char * const _argv[], char * const _envp[]) {
	return syscall(11, (int)_path, (int)_argv, (int)_envp, 0, 0, 0);
}
