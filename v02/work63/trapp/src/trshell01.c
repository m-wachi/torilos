#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include "trinclude/syscl01.h"
#include "trinclude/trutil.h"

#define MAX_ARGV 25

extern char** environ;

static char* ml_argv[MAX_ARGV];

int main(int argc, char** argv);
void readline_loop(char* cmdbuf);
void cmdread_loop();
int parse_cmdline(char* cmdline);
static char* troc_split_once(char* line);

static int fd[3];

/*
 * trshell ver01
 */
int main(int argc, char** argv) {
	char buff[80];
	int rc=0, i;
	
	puts("trshell01 start.");
	puts("press 'F1' to enable keyboard input.");

	rc = open("/dev/tty0", O_RDWR, 0);
	
	if (rc < 0) {
		sprint_varval(buff, "Error - couldn't open file. rc", rc, 0);
		puts(buff);
		exit(0);
	}
	fd[0] = rc;

	for (i=1; i<=2; i++) {
		//dup
		rc = dup(fd[0]);
		if (rc < 0) {
			sprint_varval(buff, "Error - couldn't dup file. rc", rc, 0);
			puts(buff);
			exit(0);
		}
		fd[i] = rc;
	}
	memset(buff, 0, sizeof(buff));

	cmdread_loop();

	for (i=0; i<3; i++) {
		close(fd[i]);
	}

	puts("trshell01 end.");
	exit(0);
}

void cmdread_loop() {
	char buff2[1024];
	char cmdbuf[256];
	char prompt[80];
	int len_prompt;
	int mode;	//0: normal, 1:OScmd mode
	int rc, status, i, argc;
	//char* argv[4];
	char msgbuf[128];
	char prog_path[128];
	
	mode = 0;
	memset(buff2, 0, sizeof(buff2));
	strcpy(prompt, "$ ");
	len_prompt = strlen(prompt);
	
	for (;;) {
		write(fd[1], prompt, len_prompt);
		readline_loop(cmdbuf);
		if (!strlen(cmdbuf)) continue;
		strcpy(buff2, "cmdbuf=[");
		strcat(buff2, cmdbuf);
		strcat(buff2, "] -> ");
		
		argc = parse_cmdline(cmdbuf);
		for (i=0; i<argc; i++) {
			strcat(buff2, ml_argv[i]);
			strcat(buff2, ", ");
		}
		strcat(buff2, "\n");
	
		write(fd[1], buff2, strlen(buff2));
		
		if (!strcmp("exit", cmdbuf)) {
			break;
		} else if (!strcmp(ml_argv[0], "cd")) {
			
			rc = chdir(ml_argv[1]);
			if (rc) {
				strcpy(msgbuf, ml_argv[1]);
				strcat(msgbuf, " is not directory.\n");
				write(fd[1], msgbuf, strlen(msgbuf));
			}
		}
		else if (!strcmp("oscmd", cmdbuf)) {
			mode = 1;
			strcpy(prompt, "OSMODE: ");
			len_prompt = strlen(prompt);
		} else if (!strcmp("nmlcmd", cmdbuf)) {
			mode = 0;
			strcpy(prompt, "$ ");
			len_prompt = strlen(prompt);
		}
		/*
		else if (!strcmp("cat", ml_argv[0])) {
			rc = fork();
			if (rc) {
				waitpid(rc, &status, 0);
			} else {
				//ml_argv[0] = "/cat";
				//execve(0, ml_argv, environ);
				execve("/cat", ml_argv, environ);
			}
		}
		else if (!strcmp("ls", ml_argv[0])) {
			rc = fork();
			if (rc) {
				waitpid(rc, &status, 0);
			} else {
				//argv[0] = "/ls";
				//argv[1] = ml_argv[1];
				//argv[2] = 0;
				//execve(0, argv, environ);
				execve("/ls", ml_argv, environ);
			}
		} else if (!strcmp("pwd", cmdbuf)) {
			rc = fork();
			if (rc) {
				waitpid(rc, &status, 0);
			} else {
				//argv[0] = "/pwd";
				//argv[1] = 0;
				//execve(0, argv, environ);
				execve("/pwd", ml_argv, environ);
			}
		}
		*/ 
		else {
			if (mode) {
				// run OS-command
				syscall(0x62, (int)cmdbuf, 0, 0, 0, 0, 0);
			} else {
				rc = fork();
				if (rc) {
					waitpid(rc, &status, 0);
				} else {
					strcpy(prog_path, "/");
					strcat(prog_path, ml_argv[0]);
					execve(prog_path, ml_argv, environ);
					strcpy(msgbuf, "command not found.\n");
					write(fd[1], msgbuf, strlen(msgbuf));
				}
			}
		}
	}
	
}

void readline_loop(char* cmdbuf) {
	char* p;
	int flg_loop=1;
	p = cmdbuf;
	while (flg_loop) {
		if(read(fd[0], p, 1)) {
			write(fd[1], p, 1);
			switch (*p) {
			case 0x08:
				*p-- = 0;
				break;
			case 0x0a:
				*p = 0;
				flg_loop = 0;
				break;
			default:
				p++;
			}
		}
	}
}

int parse_cmdline(char* cmdline) {
	//char* argv[MAX_ARGV];
	char *p1;
	char *p2;
	int cnt = 0;
	static char errmsg[] = "ERROR - too much argument!";

	p1 = cmdline;

	while((p2 = troc_split_once(p1))) {
		if (cnt >= MAX_ARGV) {
			write(fd[1], errmsg, strlen(errmsg));
			return -1;
			
		}
			
		ml_argv[cnt++] = p1;
		p1 = p2;
	}
	ml_argv[cnt++] = p1;
	ml_argv[cnt] = 0;
	return cnt;

}


/*
 * split string with ' ' once.
 * serial ' ' is treated as a ' '
 */
static char* troc_split_once(char* line) {
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

