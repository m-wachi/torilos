#include <dirent.h>
int _getdents(int fd, struct dirent* dirp, unsigned int count) {
	return syscall(0x8d, fd, (int)dirp, count, 0, 0, 0);
}
