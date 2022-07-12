int dup(int oldfd) {
	return syscall(0x29, oldfd, 0, 0, 0, 0, 0);
}
