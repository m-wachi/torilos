#ifndef SYSHNDLR01_H
#define SYSHNDLR01_H

#include <torilos/dirent.h>
#include <torilos/trstat.h>

#define SYSH_FLAGS_OVERRUN     0x0001



typedef struct sysh_regs {
	unsigned int eax, ebx, ecx, edx, esi, edi, ebp;
} SYSH_REGS;


typedef struct sysh_fifo {
	SYSH_REGS *buf;
	int out, in, size, free, flags;
} SYSH_FIFO;

void sysh_fifo_init(SYSH_FIFO* fifo, int size, SYSH_REGS *buf);
int sysh_fifo_put(SYSH_FIFO* fifo, unsigned int eax, unsigned int ebx, 
				  unsigned int ecx, unsigned int edx, unsigned int esi, 
				  unsigned int edi, unsigned int ebp);
SYSH_REGS* sysh_fifo_get(SYSH_FIFO* fifo);
int sysh_fifo_count(SYSH_FIFO* fifo);

void sysh_init_sysfifo();
int sysh_quecount();
SYSH_REGS* sysh_getregs();

unsigned int inthandler80(unsigned int eax, unsigned int ebx, 
						  unsigned int ecx, unsigned int edx, 
						  unsigned int esi, unsigned int edi, 
						  unsigned int ebp);

void sysh_exit(int sts);
int sysh_fork();
int sysh_puts(char* s);
int sysh_get_tick(unsigned long* p_tick);
int sysh_sleep(int sleep_time);
int sysh_open(char* filename, int flags, int mode);
int sysh_close(int fd);
int sysh_read(int fd, char* dst, int size);
int sysh_write(int fd, char* s, int size);
int sysh_waitpid(int pid, int* status, int options);
int sysh_chdir(char* path);
int sysh_lseek(int fd, int offset, int whence);
int sysh_fstat(int fd, struct stat *statbuf);
int sysh_mkdir(char* path, int mode);
int sysh_dup(int oldfd);
int sysh_brk(unsigned int addr);
int sysh_ioctl(int fd, int cmd, int param1);
int sysh_fcntl(int fd, int cmd);
int sysh_oscmd(char* cmd);
int sysh_getdents(int fd, struct dirent* dirp, unsigned int count);
int sysh_getcwd(char* buf, int size);

#endif
