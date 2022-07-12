#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/dirent.h>
#include <sys/unistd.h>
#include "trinclude/syscl01.h"
#include "trinclude/trutil.h"

extern char** environ;
extern int end;

int main(int argc, char** argv);

int main(int argc, char** argv) {
	char buff[256];
	char buff2[256], buff3[256];
	struct dirent* p_dirent;
	char dirpath[256];
	char* p_char;
	int fd, rc;
	
	puts("myapp1601 start.");
	
	strcpy(dirpath, "/");
	
	if (argc == 2) {
		strcpy(dirpath, argv[1]);
	}
	
	fd = open(dirpath, O_RDONLY);
	sprint_varval(buff2, "fd", fd, 0);
	puts(buff2);

	if (fd < 0) {
		strcpy(buff2, "open error: ");
		strcat(buff2, dirpath);
		puts(buff2);
		exit(-1);
	}
	
	rc = 1;
	
	while(rc) {
		//printf("pre: errno=%d\n", errno);

		rc = syscall(0x8d, fd, (int)buff, 256, 0, 0, 0);

		sprint_varval(buff2, "rc", rc, 0);
		puts(buff2);
	
		if (0 < rc) {
			p_char = buff;
			p_dirent = (struct dirent*)p_char;

			while (buff+rc > p_char) {
				//printf("sizeof(p_dirent->d_off)=%d\n", sizeof(p_dirent->d_off));
				//puts("name=%s", p_dirent->d_name);
				strcpy(buff2, "name=");
				strcat(buff2, p_dirent->d_name);
				// I don't understand what d_off value is. 

				//printf(", d_reclen=%d\n", p_dirent->d_reclen);
				sprint_varval(buff3, ", d_recren", p_dirent->d_reclen, 1);
				strcat(buff2, buff3);
				puts(buff2);
				p_char += p_dirent->d_reclen;
				p_dirent = (struct dirent*)p_char;
			}
		}

	}
	
	close(fd);
	
	exit(0);
}
