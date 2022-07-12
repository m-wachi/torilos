#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>

int main(int argc, char** argv);

/*
 * stdin, stdout, stderr and dup() test01.
 */
int main(int argc, char** argv) {
	char buff[80];
	char buff2[1024];
	static char msg[] = "Hello, This is myapp1503.\nThis program expects starting on the 'trshell'.\n"; 
	unsigned int cnt_read;
	int rc=0, i, fd[3];
	
	
	puts("myapp1503 start.");

	puts("Please type 5 characters.");
	
	cnt_read = 0;
	while (cnt_read < 5) {
		//read
		if(read(fd[0], buff, 1)) {
			buff2[cnt_read++] = buff[0];
		}
	}
	buff2[cnt_read] = 0;
	
	strcpy(buff, "typed data=[");
	strcat(buff, buff2);
	strcat(buff, "].");
	puts(buff);
	
	puts("myapp1503 end.");
	exit(0);
}
