#include <stdio.h>
#include <torilos/trutil.h>

char* sprint_varval(char* buff, char* varname, int value, int mode) {

	if (mode) 
		sprintf(buff, "%s=%#x", varname, value);
	else
		sprintf(buff, "%s=%d", varname, value);
}
