#include <torilos/trfile.h>
#include <torilos/debug01.h>
#include <torilos/trutil.h>
#include <torilos/mylib01.h>

static TFL_HND_FILE ml_filetbl[TFL_MAX_FILETBL];
static TE2FS_HND_FS* ml_hnd_fs;

void tfl_close(int fd, TFL_HND_FILE** user_fdtbl) {
	TFL_HND_FILE* h_file;
	char buff[80];
	char buff2[80];
	
	h_file = user_fdtbl[fd];

	sprint_varval(buff, "tfl_close(): fd", fd, 0);
	sprint_varval(buff2, ", ref_count", h_file->ref_count, 0);
	my_strcat(buff, buff2);
	debug_puts(buff);
	
	user_fdtbl[fd] = 0;

	h_file->ref_count--;

	if (h_file->ref_count > 0)
		return;
	
	tfl_free_filetbl(h_file);

}

void tfl_init_filetbl(TE2FS_HND_FS* hnd_fs) {
	int i;

	for (i=0; i<TFL_MAX_FILETBL; i++) {
		ml_filetbl[i].tr_inode = 0;
		ml_filetbl[i].ext2_inode = 0;
		ml_filetbl[i].offset = 0;
		ml_filetbl[i].ref_count = 0;
		//ml_filetbl[i].p_dirty_inoblk = 0;
	}

	ml_hnd_fs = hnd_fs;

}

void tfl_init_userfd(TFL_HND_FILE** user_fdtbl) {
	int i;

	for (i=0; i<TFL_MAX_USERFD; i++) {
		user_fdtbl[i] = 0;
	}

}

/**
 * @brief allocate file from file-table
 */
TFL_HND_FILE* tfl_alloc_filetbl() {
	int i;
	
	for (i=0; i<TFL_MAX_FILETBL; i++) {
		//if (0 == ml_filetbl[i].inode)
		if (0 == ml_filetbl[i].tr_inode)
			return &ml_filetbl[i];
	}
	

	return 0;
}


void tfl_free_filetbl(TFL_HND_FILE* hnd_file) {
	
	hnd_file->ext2_inode = 0;
	//hnd_file->p_dirty_inoblk = 0;
}

/**
 * @brief assign file to unused user-file-table
 * @param user_fdtbl user-fd(file descriptor)-table
 * @param hnd_file
 * return file-descriptor (index of user_fdtbl), -1 if error
 */
int tfl_alloc_file_usertbl(TFL_HND_FILE** user_fdtbl, TFL_HND_FILE* hnd_file) {
	int i;
	
	for (i = 0; i < TFL_MAX_USERFD; i++) {
		if (!user_fdtbl[i]) {
			user_fdtbl[i] = hnd_file;
			return i;
		}
	}
	return -1;
}

/**
 * @brief 'dup()' system call main function
 * @param user_fdtbl user-fd(file descriptor)-table
 * @param oldfd duplication source file descriptor
 * @return duplicated new file descriptor
 */
int tfl_dup(TFL_HND_FILE** user_fdtbl, int oldfd) {
	TFL_HND_FILE* h_file;
	h_file = user_fdtbl[oldfd];
	h_file->ref_count++;
	return tfl_alloc_file_usertbl(user_fdtbl, h_file);
}

/**
 * @brief make absolute-path string
 * @param abspath_buf absolute-path string buffer (OUT parameter)
 * @param filepath path string
 * @param cwdpath current working directory path string
 * @return 0: success, <0: error 
 */
int tfl_make_abspath(char* abspath_buf, char* filepath, char* cwdpath) {
	int len_filepath, len_cwdpath;
	char fpath_buf[TFL_MAX_PATHSTR+1];
	
	my_strcpy(fpath_buf, filepath);
	
	tfl_strip_tail_slash(fpath_buf);
	
	len_filepath = my_strlen(fpath_buf);
	if (TFL_MAX_PATHSTR < len_filepath) {
		return -1;	// too long filename
	}
	
	if (filepath[0] != '/') {
		len_cwdpath = my_strlen(cwdpath);
		
		if (len_filepath + len_cwdpath + 1 > TFL_MAX_PATHSTR)
			return -2;	// too long abspash
		my_strcpy(abspath_buf, cwdpath);
		if (len_cwdpath > 1) {
			my_strcat(abspath_buf, "/");
		}
		my_strcat(abspath_buf, fpath_buf);
	} else {
		my_strcpy(abspath_buf, fpath_buf);
	}
	
	tfl_remove_period_exp(abspath_buf+1);
	
	return 0;
}

/**
 * @brief remove period expression('..', '.') from the path expression
 * @param path
 * @return clean-uped path
 * ex: "abc/def/../ghi" -> "abc/ghi" 
 */
void tfl_remove_period_exp(char* path) {
	char *sp;
	char *src;
	int cnt;
	
	sp = src = path;
	
	while (*src) {
		switch (tfl_is_period_exp(src)) {
		case 2:	// '..'(parent)
			sp = tfl_backward_prev_slash(sp, path);
		case 1:	// '.' (current)
			src = tfl_forward_after_slash(src);
			break;
		default:	// other
			cnt = tfl_copy_until_slash(sp, src);
			sp += cnt; 
			src += cnt;
			break;
		}
	}
	*sp = *src;
	tfl_strip_tail_slash(path);
}

/**
 * @brief check period expression
 * @param s path string
 * @return 1: '.', 2: '..', 0: other
 */
int tfl_is_period_exp(char* s) {
	if (s[0] != '.') {
		return 0;
	}
	if (s[1] == '/' || s[1] == 0) {
		return 1;
	}
	if (s[1] == '.' && 
			(s[2] == '/' || s[2] == 0)) {
		return 2;
	}
	return 0;
}

/**
 * @brief copy string until '/'
 * @param dst copy destination
 * @param src copy source string
 * @retrun count of copied characters
 */
int tfl_copy_until_slash(char* dst, char*src) {
	int i=0;
	
	while ((*dst++ = *src++)) {
		i++;
		if (*src == '/') {
			*dst++ = *src++;
			return ++i;
		}
	}
	return i;
}

/**
 * @brief move pointer forward to slash
 * @param p char pointer
 * @param p_head string-head pointer
 * @return pointer after '/'
 * @code
 	ex)
 	path="abc/def/ghi"
	        
	before: point 'd'
	after:  point 'g'
	@endcode
 */
char* tfl_forward_after_slash(char* p) {
	while (*p) {
		p++;
		if (*p == '/') {
			p++;
			break;
		}
	}
	return p;
}

/**
 * @brief move pointer backward to slash
 * @param p char pointer
 * @param p_head string-head pointer * 
 * 
 * @return pointer after '/'
 * @code
 	ex)
 	path="abc/def/ghi"
	        
	before: point 'g'
	after:  point 'd'
	@endcode
 */
char* tfl_backward_prev_slash(char* p, char* p_head) {
	
	// p-=2 means skip tail '/'
	if (p_head > (p -= 2)) {
		return p_head;
	}
	
	while (p > p_head) {
		if (*p == '/') {
			return ++p;
		}
		p--;
	}
	return p_head;
}


/**
 * @brief strip tail '/'
 * @param filepath file path string
 */
void tfl_strip_tail_slash(char* filepath) {
	int len_filepath;
	
	len_filepath = my_strlen(filepath);

	if (len_filepath <= 1) return;
	
	if ('/' == filepath[len_filepath-1])
		filepath[len_filepath-1] = 0;
	
}

/**
 * @brief get directory path and file string from absolete path
 * @param dirpath directory path (OUT parameter)
 * @param file file name (OUT parameter)
 * @param abs_path absolete path
 */
void tfl_dirpath_file(char* dirpath, char* file, char* abs_path) {
	char* p_last_slash;
	
	my_strcpy(dirpath, abs_path);
	
	p_last_slash = my_strrchr(dirpath, '/');
	
	my_strcpy(file, p_last_slash+1);

	if (p_last_slash == dirpath)
		*++p_last_slash = 0;
	else
		*p_last_slash = 0;
}

