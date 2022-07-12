/**
 * @file syshndlr01.c
 * @brief System-Call Handler module
 */
#include <torilos/syshndlr01.h>
#include <torilos/debug01.h>
#include <torilos/mytask01.h>
#include <torilos/trtermmgr.h>
#include <torilos/trfilemdl.h>
#include <torilos/trfile.h>
#include <torilos/mylib01.h>
#include <torilos/trutil.h>
#include <torilos/trfile_opr.h>
#include <torilos/mymm01.h>
#include <torilos/myasmfunc01.h>
#include <torilos/trmempage_mdl.h>
#include <torilos/troscmd.h>
#include <torilos/dirent.h>
#include <torilos/trext2fs_opr.h>
#include <torilos/fcntl.h>
#include <torilos/trlock.h>
#include <torilos/trpmemmgr_mdl.h>

static SYSH_FIFO m_syscallque;
static SYSH_REGS m_regsbuf[32];

extern unsigned long g_time_counter;

/**
 * @brief fifo initialization.
 * 
 * @param fifo
 * @param size
 * @param buf
 */
void sysh_fifo_init(SYSH_FIFO* fifo, int size, SYSH_REGS *buf) {
	fifo->size = size;
	fifo->buf = buf;
	fifo->out = fifo->in = 0;
	fifo->flags = 0;
	fifo->free = size;
}


int sysh_fifo_put(SYSH_FIFO* fifo, unsigned int eax, unsigned int ebx, 
				  unsigned int ecx, unsigned int edx, unsigned int esi, 
				  unsigned int edi, unsigned int ebp) {

	if (0 == fifo->free) {
		fifo->flags |= SYSH_FLAGS_OVERRUN;
		return -1;
	}

	fifo->buf[fifo->in].eax = eax;
	fifo->buf[fifo->in].ebx = ebx;
	fifo->buf[fifo->in].ecx = ecx;
	fifo->buf[fifo->in].edx = edx;
	fifo->buf[fifo->in].esi = esi;
	fifo->buf[fifo->in].edi = edi;
	fifo->buf[fifo->in].ebp = ebp;
	fifo->in++;

	if (fifo->in >= fifo->size) {
		fifo->in = 0;
	}
	fifo->free--;
	return 0;
}

SYSH_REGS* sysh_fifo_get(SYSH_FIFO* fifo) {

	SYSH_REGS* p_data;

	/* return -1 if empty */
	if (fifo->size <= fifo->free) {
		return 0;
	}

	p_data = &fifo->buf[fifo->out++];

	if (fifo->out >= fifo->size) {
		fifo->out = 0;
	}
	fifo->free++;
	return p_data;
}

int sysh_fifo_count(SYSH_FIFO* fifo) {
	return fifo->size - fifo->free;
}


void sysh_init_sysfifo() {
	sysh_fifo_init(&m_syscallque, 32, m_regsbuf);
}


unsigned int inthandler80(unsigned int eax, unsigned int ebx, 
						  unsigned int ecx, unsigned int edx, 
						  unsigned int esi, unsigned int edi, 
						  unsigned int ebp) {
	int rc=0;

	//debug_puts("int80 called.");
	
	switch (eax) {
	case 0x01:    // exit
		sysh_exit(ebx);
		return 0;
	case 0x02:    // fork
		return sysh_fork();
	case 0x03:	//read
		return sysh_read(ebx, (char*)ecx, edx);
	case 0x04:    //write
		return sysh_write(ebx, (char*)ecx, edx);
	case 0x05:	//open
		return sysh_open((char*)ebx, ecx, edx);
	case 0x06:	//close
		return sysh_close(ebx);
	case 0x07:	//waitpid
		return sysh_waitpid(ebx, (int*)ecx, edx);
	case 0x08:    //sleep(torilos original)
		sysh_sleep(ebx);
		return 0;
	case 0x0c:	// chdir
		return sysh_chdir((char*)ebx);
	case 0x13:
		return sysh_lseek(ebx, ecx, edx);
	case 0x17:    //get_tick(torilos original)
		sysh_get_tick((unsigned long*)ebx);
		return 0;
	case 0x1c:	// fstat
		return sysh_fstat(ebx, (struct stat*)ecx);
	case 0x27:	// mkdir
		return sysh_mkdir((char*)ebx, ecx);
	case 0x29:	//dup
		return sysh_dup(ebx);
	case 0x2d:	//brk
		return sysh_brk(ebx);
	case 0x36:	// ioctl
		return sysh_ioctl(ebx, ecx, edx);
	case 0x37:	//fcntl
		return sysh_fcntl(ebx, ecx);
	case 0x62:	//oscmd(torilos original)
		return sysh_oscmd((char*)ebx);
	case 0x8d:	//getdents
		return sysh_getdents(ebx, (struct dirent*)ecx, edx);
	case 0xb7:	//getcwd
		sysh_getcwd((char*)ebx, (int)ecx);
		return 0;
	//case 0x014:    //debug_puts
		//sysh_puts((char*)ebx);
		//return 0;
	}

	rc = sysh_fifo_put(&m_syscallque, eax, ebx, ecx, edx, esi, edi, ebp);
	
	if (rc)
		debug_puts("syscall FIFO overflowed!!!");

	return -1;
}

int sysh_quecount() {

	return sysh_fifo_count(&m_syscallque);
}

SYSH_REGS* sysh_getregs() {
	debug_puts("getregs");
	return sysh_fifo_get(&m_syscallque);
}

/**
 * @brief 'exit()' systemcall handler(0x01)
 * @param sts exit() status
 */
void sysh_exit(int sts) {
	//farjmp(0, 5*8);
	debug_varval01("exit status", sts, 0);
	//tsk_terminate_curtask();
	tsk_zonbie_curtask(sts);
}

int sysh_fork() {
	unsigned int stk_fork_area_addr;
	unsigned int stk_fork_bottom_addr;
	int rc;
	
	//stk_fork_area_addr = mm_malloc_pg_knl(MM_PGSIZE * 2);
	stk_fork_area_addr = tpmmm_malloc_pg_knl(MM_PGSIZE * 2);
	stk_fork_bottom_addr = stk_fork_area_addr + MM_PGSIZE * 2;
	
	debug_varval01("stk_fork_bottom_addr", stk_fork_bottom_addr, 1);
	
	rc = asm_fork(stk_fork_bottom_addr);
	
	//mm_free(stk_fork_area_addr);
	tpmmm_free(stk_fork_area_addr);
	
	return rc;
	
}


int sysh_puts(char* s) {
	char buff[160];

	//my_strcpy(buff, "sysh_puts:");
	my_strcpy(buff, s);
	//debug_puts(buff);
	trtrm_puts(buff);
	return 0;
}

int sysh_get_tick(unsigned long* p_tick) {
	*p_tick = g_time_counter;
	return 0;
}

int sysh_sleep(int sleep_time) {
	tsk_sleep_curtask(sleep_time);
	return 0;
}

int sysh_open(char* filename, int flags, int mode) {
	TRTSK_TASK* task;
	char buff[80];
	char buff2[80];
	int rc;
	my_strcpy(buff, "sysh_open: filename=");
	my_strcat(buff, filename);
	sprint_varval(buff2, ", flags", flags, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);
	
	task = tsk_get_cur_task();

	rc = tflop_open(filename, flags, task->fdtbl, task->cwd);
	
	return rc;
}

int sysh_close(int fd) {
	TRTSK_TASK* task;
	char buff[80];
	
	sprint_varval(buff, "sysh_close: fd", fd, 0);
	debug_puts(buff);
	
	task = tsk_get_cur_task();

	//tflm_close(fd, task->fdtbl);
	tflop_close(fd, task->fdtbl);
	
	return 0;
}

int sysh_read(int fd, char* dst, int size) {
	TRTSK_TASK* task;
	char buff[80];
	int sz_read;
	
	sprint_varval(buff, "sysh_read: fd", fd, 0);
	debug_puts(buff);
	
	task = tsk_get_cur_task();
	
	//sz_read = tflm_read(fd, (unsigned char*)dst, size, task->fdtbl); 
	sz_read = tflop_read(fd, dst, size, task->fdtbl);
	
	return sz_read;
}

int sysh_write(int fd, char* s, int count) {
	TRTSK_TASK* task;
	//char buff[80];
	int rc;
	
	//sprint_varval(buff, "sysh_write: fd", fd, 0);
	//debug_puts(buff);
	
	task = tsk_get_cur_task();

	rc = tflop_write(fd, s, count, task->fdtbl);
	
	return rc;
	
}

/**
 * @brief 'chdir()' systemcall handler(0x07)
 */
int sysh_waitpid(int pid, int* status, int options) {
	return tsk_waitpid(pid, status, options);
}

/**
 * @brief 'chdir()' systemcall handler(0x0c)
 * @param path path to change directory
 * @return 0: success, -1: error
 */
int sysh_chdir(char* path) {
	int inono;
	char fullpath[512];
	TRTSK_TASK* task;
	struct ext2_inode* p_inode;
	char buff[128];
	
	my_strcpy(buff, "sysh_chdir: ");
	my_strcat(buff, path);
	debug_puts(buff);
	
	
	task = tsk_get_cur_task();
	
	if (tfl_make_abspath(fullpath, path, task->cwd)) {
		return -1;
	}

	my_strcpy(buff, "path=");
	my_strcat(buff, fullpath);
	debug_puts(buff);
	
	//check if exists
	if (!(p_inode = te2fo_namei(&inono, fullpath))) {
		return -1;
	}
	
	//check if directory
	if (!te2fs_isdir(p_inode)) {
		return -1;
	}
	tlck_stop_intr();
	my_strcpy(task->cwd, fullpath);
	my_strcpy(buff, "task cwd=");
	my_strcat(buff, fullpath);
	debug_puts(buff);
	tlck_allow_intr();
	return 0;
}

/**
 * @brief 'lseek()' systemcall handler (0x13)
 * @param fd file descriptor
 * @param offset
 * @param whence		SEEK_SET/SEEK_CUR/SEEK_END
 * @return file offset
 */ 
int sysh_lseek(int fd, int offset, int whence) {
	TRTSK_TASK* task;
	task = tsk_get_cur_task();
	
	return tflop_lseek(fd, offset, whence, task->fdtbl);
}

/**
 * @brief 'fstat()' systemcall handler (0x1c)
 * @param fd file descriptor
 * @param statbuf struct 'stat' buffer
 * @return 0: success, -1: error
 */
int sysh_fstat(int fd, struct stat *statbuf) {
	TRTSK_TASK* task;
	char buff[40];
	
	sprint_varval(buff, "sysh_fstat: fd", fd, 0);
	debug_puts(buff);

	task = tsk_get_cur_task();

	return tflop_fstat(fd, statbuf, task->fdtbl);
}

/**
 * @brief 'mkdir()' systemcall handler (0x27)
 * @param path directory path
 * @param mode
 * @return 0: success, -1: error
 */
int sysh_mkdir(char* path, int mode) {
	TRTSK_TASK* task;

	task = tsk_get_cur_task();
	
	return te2fo_mkdir(path, mode, task->cwd);
}

/**
 * @brief 'dup()' systemcall handler (0x29)
 * @param oldfd source fd to duplicate
 * @return new duplicated fd
 */
int sysh_dup(int oldfd) {
	TRTSK_TASK* task;

	debug_varval01("sysh_dup: oldfd", oldfd, 0);

	task = tsk_get_cur_task();
	
	return tfl_dup(task->fdtbl, oldfd);
}

/**
 * @brief 'brk()' systemcall handler (0x2d)
 * @param addr break-value
 * @return 0: success, !=0: error
 */
int sysh_brk(unsigned int addr) {
	TRTSK_TASK* task;
	int rc;
	
	debug_varval01("sysh_brk: addr", addr, 1);
	
	task = tsk_get_cur_task();
	
	rc = tmpgm_brk(task->vminf, addr);
	
	debug_varval01("sysh_brk: rc", rc, 0);
	
	return rc;
}


/**
 * @brief 'ioctl()' systemcall handler (0x36)
 * 
 * see ioctl() manual
 */
int sysh_ioctl(int fd, int cmd, int param1) {
	TRTSK_TASK* task;
	char buff[80];
	char buff2[20];
	
	sprint_varval(buff, "sysh_ioctl: fd", fd, 0);
	sprint_varval(buff2, ", cmd", cmd, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);
	
	task = tsk_get_cur_task();
	
	return tflop_ioctl(fd, cmd, param1, task->fdtbl);
	
}

/**
 * @brief 'fcntl()' systemcall handler (0x37)
 * 
 * see fcntl() manual
 */
int sysh_fcntl(int fd, int cmd) {
	//TODO implment this system call
	
	debug_varval01("sysh_fcntl: cmd", cmd, 0);
	
	if (cmd == F_SETFD) {
		return 0;
	}
	
	return -1;
}

/**
 * @brief 'oscmd()' system call handler (0x62) (torilos original)
 * @param cmd comamnd
 * @return 0: success
 */
int sysh_oscmd(char* cmd) {
	TRTSK_TASK* task;
	int deb_sw;
	
	deb_sw = get_debug_switch();
	
	debug_switch(1);
	task = tsk_get_cur_task();
	troc_runoscmd(cmd, task->fdtbl);

	debug_switch(deb_sw);
	
	return 0;
}

/**
 * @brief 'getdents' system call handler (0x8d)
 * @param fd file-descriptor (directory)
 * @param dirp read buffer
 * @param count read buffer size
 * @return 0: end of directory, >0: read count, -1: error
 */
int sysh_getdents(int fd, struct dirent* dirp, unsigned int count) {
	TRTSK_TASK* task;
	int rc;
	task = tsk_get_cur_task();
	
	if (0 > (rc = te2fo_getdents(task->fdtbl[fd], dirp, count))) {
		debug_varval01("sysh_getdents(): rc", rc, 0);
		return -1;
	}
	
	return rc;
}


/**
 * @brief 'getcwd' - get current working directory
 * @param buf  path-buffer
 * @param size buffer-size
 * @return path-buffer size
 */
int sysh_getcwd(char* buf, int size) {
	TRTSK_TASK* task;
	int rc, len;
	char temp_buff[128];
	
	if (size < 2) return -1;
	
	task = tsk_get_cur_task();

	buf[size-1] = 0;
	
	my_strcpy(temp_buff, "cwd=");
	
	my_strcat(temp_buff, task->cwd);
	
	debug_puts(temp_buff);
	
	my_strncpy(buf, task->cwd, size-1);
	
	len = my_strlen(task->cwd);
	
	if (size > len) {
		rc = len;
	} else {
		rc = size;
	}
	return rc;
	/*
	my_strcpy(buf, "/");
	return 1;
	*/
}

