#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include "trinclude/syscl01.h"
#include "trinclude/trutil.h"

extern char** environ;

int main(int argc, char** argv);

int main(int argc, char** argv) {
	int rc, child_pid, status;
	char buff[80], buff2[80];
	static char staticmsg[80] = "start infinite loop...."; 
	char* lclargv[4];
	puts("myapp1302 start.");
	char prog_path[] = "/myapp09.elf";
	
	lclargv[0] = "myapp09.elf";
	lclargv[1] = "pr1";
	lclargv[2] = "param2";
	lclargv[3] = 0;

	rc = fork();
	child_pid = rc;
	sprint_varval(buff, "fork rc", rc, 1);
	puts(buff);
	
	if (!rc) {
		strcpy(staticmsg, "I'm child: exec myapp09");
		puts(staticmsg);
		rc = execve(prog_path, lclargv, environ);
		sprint_varval(buff, "execve error: rc", rc, 0);
		strcat(buff, ", prog=");
		strcat(buff, prog_path);
		puts(buff);
	}
	
	//waitpid
	syscall(0x07, child_pid, (unsigned int)&status, 0, 0, 0, 0);
	waitpid(child_pid, &status, 0);
	puts("detect child process termination.");
	sprint_varval(buff, "child_pid", child_pid, 0);
	sprint_varval(buff2, ", status", status, 1);
	strcat(buff, buff2);
	puts(buff);
	puts("myapp1302 end.");
	
	exit(0);
}
