#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include "trinclude/syscl01.h"
#include "trinclude/trutil.h"

int main(int argc, char** argv);

int main(int argc, char** argv) {
	unsigned int cnt_read=0;
	int rc;
	char buff[80];
	static char staticmsg[80] = "start infinite loop...."; 

	puts("myapp12 start.");

	rc = syscall(2, 0, 0, 0, 0, 0, 0);
	
	sprint_varval(buff, "rc", rc, 1);
	puts(buff);
	
	if (rc) {
		puts(staticmsg);
		strcpy(staticmsg, "parent msg: start infinite loop..");
	} else {
		strcpy(staticmsg, "I'm child: start infinite loop..");
		puts(staticmsg);
	}
	
	for (;;) {
		if (cnt_read > 0xffffff)
			cnt_read = 0;
		cnt_read++;
		
	}
	
	puts("myapp11 end.");
	exit(0);
}
