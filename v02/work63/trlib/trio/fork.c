#include <unistd.h>
#include "include/syscl01.h"

int fork() {
	return syscall(0x02, 0, 0, 0, 0, 0, 0);
}
