#include <torilos/troscmd.h>
#include <torilos/mylib01.h>
#include <torilos/mymm01.h>
#include <torilos/ext2fs.h>
#include <torilos/trfile.h>
#include <torilos/trblkbuf.h>
#include <torilos/trfilemdl.h>
#include <torilos/debug01.h>
#include <torilos/myutil01.h>
#include <torilos/trlock.h>
#include <torilos/mytask01.h>
#include <torilos/trutil.h>
#include <torilos/trstat.h>
#include <torilos/trelf.h>
#include <torilos/trext2fs_opr.h>

static unsigned char* ml_rootdir_ent;

void troc_init(unsigned char* rootdir_ent) {
	ml_rootdir_ent = rootdir_ent;
}

//void troc_runoscmd(char* cmdbuf, unsigned char* rootdir_ent, 
//		TFL_HND_FILE** user_fdtbl) {
void troc_runoscmd(char* cmdbuf, TFL_HND_FILE** user_fdtbl) {
	char *p_file;
	char *p;
	int rc;
	
	if (!my_strlen(cmdbuf)) return;
	
	if (!my_strcmp(cmdbuf, "lsos")) {
		//fancy_ls(rootdir_ent);
		fancy_ls(ml_rootdir_ent);
	} else	if (!my_strcmp(cmdbuf, "ll")) {
		//fancy_ll(rootdir_ent);
		fancy_ll(ml_rootdir_ent);
	} else	if (!my_strcmp(cmdbuf, "sync")) {
			fancy_sync_oscmd();
	} else if (!my_strncmp(cmdbuf, "rm ", 3)) {
		p_file = cmdbuf + 3;
		//fancy_rm(p_file, (char*)rootdir_ent);
		fancy_rm(p_file, (char*)ml_rootdir_ent);
	} else if (!my_strncmp(cmdbuf, "kill ", 5)) {
		int pid;
		
		p = cmdbuf + 5;
		pid = my_atoi(p);
		//debug_varval01("pid", pid, 0);
		tsk_set_signal(pid, 1);
	} else if (!my_strncmp(cmdbuf, "dumpinode ", my_strlen("dumpinode "))) {
		char *p_file;
		p_file = cmdbuf + my_strlen("dumpinode ");
		//dump_inode_oscmd(p_file, (char*)rootdir_ent);
		dump_inode_oscmd(p_file, (char*)ml_rootdir_ent);
	} else if (!my_strcmp(cmdbuf, "dmprootdir")) {
		//print_direntry(rootdir_ent);
		print_direntry(ml_rootdir_ent);
	} else if (!my_strcmp(cmdbuf, "memdmp")) {
		mm_print_pg_mngr();
	} else if (!my_strcmp(cmdbuf, "psdmp")) {
		tsk_debug_running();
	} else if (!my_strcmp(cmdbuf, "freevmmap")) {
		debug_varval01("free vmmap count", trmpg_free_vmmap_size(), 0);
	} else if (!my_strcmp(cmdbuf, "statblkbuf")) {
		troc_statblkbuf();
	} else if (!my_strncmp(cmdbuf, "debug ", 6)) {
		if (!my_strcmp(cmdbuf, "debug on")) 
			debug_switch(1);
		else
			debug_switch(0);
	} else {
		tlck_allow_intr();
		//rc = run_app(cmdbuf, rootdir_ent, user_fdtbl);
		//rc = troc_parse_run(cmdbuf, rootdir_ent, user_fdtbl, "/");
		rc = troc_parse_run(cmdbuf, ml_rootdir_ent, user_fdtbl, "/");
		if (rc) {
			debug_puts("coundn't execute.");
		}
		
	}
}


void fancy_ls(unsigned char* root_dirent) {
	struct ext2_dir_entry_2* p_entry;
	unsigned char* p;
	int pos = 0;
	char buff[128];
	
	p = root_dirent;

	while(pos < 1024) {
		p_entry = (struct ext2_dir_entry_2*)p;
		
		my_strncpy(buff, p_entry->name, p_entry->name_len);
		buff[p_entry->name_len] = '\0';
		debug_puts(buff);
		
		p+=p_entry->rec_len;
		pos+=p_entry->rec_len;
	}
}

void fancy_ll(unsigned char* root_dirent) {
	struct ext2_dir_entry_2* p_entry;
	unsigned char* p;
	int pos = 0;
	char buff[128], buff2[128];
	struct ext2_inode* p_inode;
	unsigned short mode;
	
	p = root_dirent;

	while(pos < 1024) {
		p_entry = (struct ext2_dir_entry_2*)p;
		
		my_strncpy(buff, p_entry->name, p_entry->name_len);
		buff[p_entry->name_len] = '\0';
		
		p_inode = tflm_get_inode_by_dirent(p_entry);
		
		sprint_varval(buff2, " i_mode", p_inode->i_mode, 1);
		my_strcat(buff, buff2);
		
		mode = p_inode->i_mode & S_IFMT; 
		sprint_varval(buff2, " mode", mode, 1);
		my_strcat(buff, buff2);
		
		switch (mode) {
		case S_IFREG:
			my_strcpy(buff2, " FILE");
			break;
		case S_IFDIR:
			my_strcpy(buff2, " DIR");
			break;
		default:
			buff2[0] = 0;
		}
		my_strcat(buff, buff2);
		
		debug_puts(buff);
		
		p+=p_entry->rec_len;
		pos+=p_entry->rec_len;
	}
}

void fancy_rm(char* filename, char* rootdir_ent) {
	tflm_unlink(filename, (unsigned char*)rootdir_ent);
}

void fancy_sync_oscmd() {
	tflm_sync();
	debug_puts("filesystem has been syncronized with hard-disk.");
}

void dump_inode_oscmd(char* filename, char* rootdir_ent) {
	int fd;
	TFL_HND_FILE* user_fdtbl[16];  // user fd table
	char buf2[128];
	
	tfl_init_userfd(user_fdtbl);
	
	my_strcpy(buf2, "dump_inode: filename=");
	my_strcat(buf2, filename);
	debug_puts(buf2);
	
	//fd = tflm_open2(filename, (unsigned char*)rootdir_ent, user_fdtbl);
	fd = te2fo_open(filename, user_fdtbl);
	
	if (fd < 0) {
		my_strcpy(buf2, "error - file not found. ");
		my_strcat(buf2, filename);
		debug_puts(buf2);
		return;
	}
	
	dump_inode(user_fdtbl[fd]->ext2_inode, 0xff);
	
	tflm_close(fd, user_fdtbl);			
	
}

/*
 * parse command-line to argv[] then run application.
 */
int troc_parse_run(char* cmdline, unsigned char* dirent_blk, 
		TFL_HND_FILE** user_fdtbl, char* cwd) {
	char* argv[TROC_MAX_ARGC];
	char cmdbuf[100];
	char *p1;
	char *p2;
	int cnt = 0;
	int envc = 3;
	char* envv[] = {"HOME=/root", "PWD=/", "TERM=torilos"};
	my_strcpy(cmdbuf, cmdline);
	
	//argv[0] = argbuf;
	p1 = cmdbuf;

	while((p2 = troc_split_once(p1))) {
		if (cnt >= TROC_MAX_ARGC) {
			debug_puts("ERROR - too much argument!");
			return -1;
			
		}
			
		argv[cnt++] = p1;
		//cnt++;
		p1 = p2;
	}
	argv[cnt++] = p1;
	
	return trelf_run_app(argv[0], cnt, argv, envc, envv, cwd);
}

/*
 * split string with ' ' once.
 * serial ' ' is treated as a ' '
 */
char* troc_split_once(char* line) {
	char* p;
	int found = 0;
	
	p = line;
	
	while(*p) {
		if (*p == ' ') {
			found = 1;
			break;
		}
		p++;
	}
	
	if (!found) {
		return 0;
	}
	
	*p = 0;
	p++;
	while(*p == ' ') {
		p++;
	}
	return p;

}

/*
 * display trblkbuf statistics
 */
void troc_statblkbuf() {
	char buff[128];
	char buff2[128];
	
	sprint_varval(buff, "blkbuf-count: total", tblbf_get_blkcells_count(), 0);
	sprint_varval(buff2, ", free", tblbf_get_freeblk_count(), 0);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", unallocated", tblbf_get_unalc_count(), 0);
	my_strcat(buff, buff2);
	
	debug_puts(buff);
	
	tblbf_dump_used_block();
	//tblbf_dump_all_block();
	
}
