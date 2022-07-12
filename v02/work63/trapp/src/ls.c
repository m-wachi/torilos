//#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	DIR* h_dir;
	struct dirent* p_dirent;
	char dirpath[512];
	char buff[512];
	
	strcpy(dirpath, ".");
	
	if (argc == 2) {
		strcpy(dirpath, argv[1]);
	}
	
	if (!(h_dir = opendir(dirpath))) {
		strcpy(buff, "couldn't opendir ");
		strcat(buff, dirpath);
		puts(buff);
		exit(-1);
	}
	
	
	while((p_dirent = readdir(h_dir))) {
		puts(p_dirent->d_name);
		//sprint_varval(buff, "", p_dirent->d_reclen)
	}

	closedir(h_dir);
	
	return 0;
}
