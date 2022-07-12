#ifndef TROSCMD_H
#define TROSCMD_H

#include "trfile.h"

#define TROC_MAX_ARGC 20
void troc_init(unsigned char* rootdir_ent);
//void troc_runoscmd(char* cmdbuf, unsigned char* rootdir_ent, TFL_HND_FILE** user_fdtbl);
void troc_runoscmd(char* cmdbuf, TFL_HND_FILE** user_fdtbl);
void fancy_ls(unsigned char* root_dirent);
void fancy_ll(unsigned char* root_dirent);
void fancy_rm(char* filename, char* rootdir_ent);
void fancy_sync_oscmd();
void dump_inode_oscmd(char* filename, char* rootdir_ent);
int troc_parse_run(char* cmdline, unsigned char* dirent_blk, 
		TFL_HND_FILE** user_fdtbl, char* cwd);
int run_app(char* filename, unsigned char* dirent_blk, 
		TFL_HND_FILE** user_fdtbl, int argc, char** argv, char* cwd);
char* troc_split_once(char* line);
void troc_statblkbuf();

#endif
