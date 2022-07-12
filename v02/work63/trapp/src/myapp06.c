#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trinclude/trutil.h"

extern char** environ;

int main(int argc, char** argv, char** envp);

int main(int argc, char** argv, char** envp) {

	char buff[80];
	int i;
	char** p_envp;
	
	puts("Hello app6");

	sprint_varval(buff, "app06: argc", argc, 0);

	puts(buff);
	
	sprint_varval(buff, "app06: &argc", (unsigned int)&argc, 1);

	puts(buff);

	for (i=0; i<argc; i++) {
		sprint_varval(buff, "app06: i", i, 0);
		strcat(buff, ", argv[i] value=");
		strcat(buff, argv[i]);
		puts(buff);
	}

	sprint_varval(buff, "environ value", (unsigned int)environ, 1);
	puts(buff);
	sprint_varval(buff, "envp", (unsigned int)envp, 1);
	puts(buff);
	
	puts("environment variables are..");
	for (p_envp=environ; *p_envp; p_envp++) {
		puts(*p_envp);
	}
	
	exit(0);
	
}

