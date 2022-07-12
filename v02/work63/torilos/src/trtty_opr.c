#include <torilos/trtty_opr.h>
#include <torilos/trtty.h>
#include <torilos/trfile.h>
#include <torilos/trinode.h>
#include <torilos/trmajor.h>
#include <torilos/trstat.h>
#include <torilos/mylib01.h>
#include <torilos/trutil.h>
#include <torilos/debug01.h>
#include <torilos/fcntl.h>
#include <torilos/trtty_mdl.h>
#include <torilos/trtermio.h>

int trtyo_open(char* filename, int flags, TFL_HND_FILE** user_fdtbl) {
	TFL_HND_FILE* file;
	TR_INODE* tr_inode;
	int fd, tty_no;
	
	if (!(tr_inode = trtyo_namei(filename))) {
		return -1;	//no such device
	}
	tty_no = trtyo_inono2ttyno(tr_inode->inono);
	
	trtty_open(tty_no);
	
	if (flags & O_NONBLOCK) {
		trtty_blockio(tty_no, TRTTY_BLKIO_OFF);
	} else { 
		trtty_blockio(tty_no, TRTTY_BLKIO_ON);
	}
	
	file = tfl_alloc_filetbl();
	file->tr_inode = tr_inode;
	file->offset = 0;
	file->ref_count = 1;
	
	if (0 > (fd = tfl_alloc_file_usertbl(user_fdtbl, file))) {
		return -2;	//couldn't alloc fd
	}
	
	return fd;
	
}

TR_INODE* trtyo_namei(char* s_path) {
	TR_INODE* tr_inode;
	unsigned int inono;

	inono = 0;
	if (!my_strcmp("/dev/tty0", s_path)) {
		inono = 1;
	}
	else if (!my_strcmp("/dev/tty1", s_path)) {
		inono = 2;
	}
	
	if (!inono) return 0;
	
	tr_inode = trind_lookup_inode(TTY_MAJOR, inono);
	
	if (tr_inode) return tr_inode;
	
	tr_inode = trind_alc_free_inode();
	
	tr_inode->log_dev_no = TTY_MAJOR;
	tr_inode->inono = inono;
	tr_inode->i_mode = S_IFCHR + 0666;
	
	return tr_inode;
	
}

int trtyo_read(int fd, char* buf, unsigned int sz, 
		TFL_HND_FILE** user_fdtbl) {
	
	TFL_HND_FILE* file;
	
	file = user_fdtbl[fd];
	
	if (file->tr_inode->log_dev_no != TTY_MAJOR) {
		return -1;
	}
	
	return trtym_read(trtyo_inono2ttyno(file->tr_inode->inono), buf, sz);
	
}

int trtyo_write(int fd, char* buf, unsigned int size, 
			  TFL_HND_FILE** user_fdtbl) {
	TR_INODE* tr_inode;
	int tty_no;
	char buff[80], buff2[80];
	
	tr_inode = user_fdtbl[fd]->tr_inode;
	
	tty_no = trtyo_inono2ttyno(tr_inode->inono);
	
	if (tr_inode->log_dev_no != TTY_MAJOR) return -1;

	sprint_varval(buff, "trtyo_write: fd", fd, 0);
	sprint_varval(buff2, ", tty_no", tty_no, 0);
	my_strcat(buff, buff2);
	debug_puts(buff);

	trtym_write(tty_no, buf, size);
	
	return size;
}


void trtyo_close(int fd, TFL_HND_FILE** user_fdtbl) {
	int tty_no;
	
	tty_no = trtyo_inono2ttyno(user_fdtbl[fd]->tr_inode->inono);

	tfl_close(fd, user_fdtbl);

	//trtty_close(tty_no);
	trtym_close(tty_no);
}

int trtyo_inono2ttyno(unsigned int inono) {
	return inono-1;
}

/**
 * @brief "ioctl" fancy procedure for tty device
 * @param cmd command
 * @param param1 command parameter
 * @return 0: success, -1: error
 */
int trtyo_ioctl(TFL_HND_FILE* hnd_file, int cmd, int param1) {
	int tty_no;

	if (cmd == TCGETA) {
		debug_puts("trtyo_ioctl: WARNING - TCGETA command do nothing!");
		return 0;
	} else if (cmd == TRTYO_NORMAL_MODE) {
		tty_no = trtyo_inono2ttyno(hnd_file->tr_inode->inono);
		trtym_editor(tty_no, 0);
		return 0;
	} else if (cmd == TRTYO_EDITOR_MODE) {
		tty_no = trtyo_inono2ttyno(hnd_file->tr_inode->inono);
		trtym_editor(tty_no, 1);
		return 0;
	}

	debug_puts("trtyo_ioctl: unimplemented command.");
	return -1;
}


