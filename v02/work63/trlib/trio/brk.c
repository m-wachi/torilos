extern int errno;

int _brk(unsigned int addr) {
	int rc;
	rc = syscall(0x2d, addr, 0, 0, 0, 0, 0);
	errno = rc;
	return rc;
}
