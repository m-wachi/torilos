#ifndef TREXT2FS_OPR_H
#define TREXT2FS_OPR_H

#include <torilos/trfile.h>
#include <torilos/dirent.h>
#include <torilos/trext2fsmdl.h>

void te2fo_init(TE2FM_HND_FSMDL* hnd_fsmdl);
int te2fo_open(char* filename, TFL_HND_FILE** user_fdtbl);
int te2fo_create(char* filename, TFL_HND_FILE** user_fdtbl);
int te2fo_getdents(TFL_HND_FILE* h_file, struct dirent* dirp, unsigned int count);
int te2fo_getdent_one(int* sz_actual_read, TFL_HND_FILE* h_file, 
		struct dirent* dirp, unsigned int sz_rest_dirp);
struct ext2_inode* te2fo_namei(int* inono, char* s_path);
int te2fo_mkdir(char* path, int mode, char* cwd);
struct ext2_inode* te2fo_create_new_dirent(unsigned int* p_new_inono,
		unsigned int* p_parent_inono, 
		unsigned int** p_dirty_inoblk,char* entry_abs_path, 
		unsigned char file_type);

void te2fo_truncate(struct ext2_inode* p_inode);

#endif
