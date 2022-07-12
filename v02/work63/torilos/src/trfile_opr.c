/**
 * @file trfile_opr.c
 * @brief Generic File Operation Module.
 */

#include <torilos/trfile_opr.h>
#include <torilos/trtty_opr.h>
#include <torilos/trmajor.h>
#include <torilos/trfilemdl.h>
#include <torilos/trtermmgr.h>
#include <torilos/trext2fs_opr.h>
#include <torilos/fcntl.h>
#include <torilos/debug01.h>
#include <torilos/mylib01.h>

/**
 * @brief generic 'open' procedure
 * @param  filename
 * @param  flags
 * @param  user_fdtbl
 * @param cwd current-working-directory path
 * @return fd(file descriptor)
 */ 
int tflop_open(char* filename, int flags, TFL_HND_FILE** user_fdtbl, char* cwd) {
	int fd = -10;
	char abspath_buf[TFL_MAX_PATHSTR+1];
	char buff[300];
	
	if (-1 != (fd = trtyo_open(filename, flags, user_fdtbl))) {
		return fd;
	}

	if(tfl_make_abspath(abspath_buf, filename, cwd)) {
		return -11;
	}

	if (flags & O_CREAT) {
		fd = te2fo_open(abspath_buf, user_fdtbl);
		if (fd >= 0)
			te2fo_truncate(user_fdtbl[fd]->ext2_inode);
		else
			fd = te2fo_create(abspath_buf, user_fdtbl);
		
	} else {
		my_strcpy(buff, "tflop_open: abspath_buf=");
		my_strcat(buff, abspath_buf);
		debug_puts(buff);
		fd = te2fo_open(abspath_buf, user_fdtbl);
	}
	debug_varval01("fd", fd, 0);
	return fd;
}

/**
 * @brief generic "read" procedure
 * @param fd file descriptor
 * @param buf
 * @param count count to read
 * @param user_fdtbl
 * @return read size
 */
int tflop_read(int fd, char* buf, unsigned int count, 
		TFL_HND_FILE** user_fdtbl) {
	TR_INODE* tr_inode;
	int rc = 0;
	
	tr_inode = user_fdtbl[fd]->tr_inode;
	
	switch (tr_inode->log_dev_no) {
	case TTY_MAJOR:
		rc = trtyo_read(fd, buf, count, user_fdtbl);
		break;
	case HD_MAJOR:
		rc = tflm_read(fd, (unsigned char*)buf, count, user_fdtbl);
		break;
	}
	return rc;
}

/**
 * @brief generic "write" procedure
 * 
 * @param fd
 * @param buf
 * @param count
 * @param user_fdtbl
 * @return written size
 */
int tflop_write(int fd, char* buf, unsigned int count, 
			  TFL_HND_FILE** user_fdtbl) {
	TR_INODE* tr_inode;
	unsigned int rc = 0;

	//TODO This code should be removed. 
	if (!user_fdtbl[fd]) {
		trtrm_write(buf, count);
		return count;
	}
	
	tr_inode = user_fdtbl[fd]->tr_inode;
	
	switch (tr_inode->log_dev_no) {
	case TTY_MAJOR:
		rc = trtyo_write(fd, buf, count, user_fdtbl);
		break;
	case HD_MAJOR:
		rc = tflm_write(fd, buf, count, user_fdtbl);
		break;
	}
	return rc;
}


/**
 * @brief generic "close" procedure
 * @param fd
 * @param user_fdtbl 
 * @return 0: success
 */
int tflop_close(int fd, TFL_HND_FILE** user_fdtbl) {
	TR_INODE* tr_inode;
	int rc = 0;
	
	tr_inode = user_fdtbl[fd]->tr_inode;
	
	switch (tr_inode->log_dev_no) {
	case TTY_MAJOR:
		trtyo_close(fd, user_fdtbl);
		rc = 0;
		break;
	case HD_MAJOR:
		rc = tflm_close(fd, user_fdtbl);
		break;
	}
	return rc;
}

/**
 * @brief generic "lseek" procedure
 * @param fd file descriptor
 * @param offset
 * @param whence		SEEK_SET/SEEK_CUR/SEEK_END
 * @param user_fdtbl
 * @return file offset
 */
int tflop_lseek(int fd, long offset, int whence, 
		TFL_HND_FILE** user_fdtbl) {
	TFL_HND_FILE* hnd_file;
	
	hnd_file = user_fdtbl[fd];

	if (hnd_file->tr_inode->log_dev_no != HD_MAJOR)
		return -1;
	
	switch (whence) {
	case SEEK_SET:
		hnd_file->offset = offset;
		break;
	case SEEK_CUR:
		hnd_file->offset += offset;
		break;
	case SEEK_END:
		hnd_file->offset = hnd_file->ext2_inode->i_size + offset; 
		break;
	}
	return hnd_file->offset;
}

/**
 * @brief 'fstat()' fancy procedure
 * @param fd file descriptor
 * @param statbuf struct 'stat' buffer
 * @param user_fdtbl
 * @return 0: success, -1: error
 */
int tflop_fstat(int fd, struct stat *statbuf, TFL_HND_FILE** user_fdtbl) {
	TFL_HND_FILE* hnd_file;
	
	hnd_file = user_fdtbl[fd];
	
	if (!hnd_file) {
		return -1;
	}
	
	statbuf->st_dev = hnd_file->tr_inode->log_dev_no; 
	if (hnd_file->tr_inode->log_dev_no == HD_MAJOR) {
		statbuf->st_size = hnd_file->ext2_inode->i_size;
		statbuf->st_mode = hnd_file->ext2_inode->i_mode;
	}
	return 0;
}

/**
 * @brief 'ioctl()' fancy procedure
 * @param fd file descriptor
 * @param cmd command
 * @param param1 parameter
 * @return 0: success, -1: error
 */
int tflop_ioctl(int fd, int cmd, int param1, TFL_HND_FILE** user_fdtbl) {
	TFL_HND_FILE* hnd_file;
	
	hnd_file = user_fdtbl[fd];
	if (!hnd_file) {
		return -1;
	}

	if (hnd_file->tr_inode->log_dev_no != TTY_MAJOR) {
		debug_puts("tflop_ioctl: not implement device.");
		return -1;
	}
	
	return trtyo_ioctl(hnd_file, cmd, param1);
	
}

