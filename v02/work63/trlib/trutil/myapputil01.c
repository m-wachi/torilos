#include <string.h>
#include "include/trutil.h"

/*
 * make string of variable and value.
 * buff - string buffer
 * varname - variable name string
 * value - variable value
 * mode - 0: decimal, 1: hex
 */
char* sprint_varval(char* buff, char* varname, int value, int mode) {
	char buffval[50];

	strcpy(buff, varname);
	strcat(buff, "=");

	if (mode)
		my_itoh(buffval, value);
	else
		my_itoa(buffval, value);
	strcat(buff, buffval);
	return buff;
}

char* my_itoa(char *s, int n) {
  
	int a = n;
	int b, cnt, i;
	char *p;
	char buff[100];

	if (0 == n) {
		*s++ = '0';
		*s = '\0';
		return;
	}
	p = s;

	if (a < 0) {
		*p = '-';
		p++;
		a = a * -1;
	}

	cnt = 0;
	while(a != 0) {
		b = a % 10;

		b += 0x30;

		buff[cnt++] = (char) b;

		a /= 10;
	}
    
	for(i=cnt-1; i>-1; i--) {
		*p = buff[i];
		p++;
	}
	*p = '\0';
	return s;
}

char *my_itoh(char *s, unsigned int n) {
	unsigned int a = n;
	int b, cnt, i;
	char *p;
	char buff[100];

	p = s;
	cnt = 0;
	while(a != 0) {
		b = a % 16;

		if (b < 10) 
			b += 0x30;
		else
			b += 0x60 - 9;

		buff[cnt++] = (char) b;

		a /= 16;
	}
    *p = '0'; p++;
	*p = 'x'; p++;
    *p = '0'; p++;

	for(i=cnt-1; i>-1; i--) {
		*p = buff[i];
		p++;
	}
	*p = '\0';

	return s;
}

char* my_ultoa(char *s, unsigned long n) {
  
	unsigned long a = n, b;
	int cnt, i;
	char *p;
	char buff[100];

	p = s;
	cnt = 0;
	while(a != 0) {
		b = a % 10;

		b += 0x30;

		buff[cnt++] = (char) b;

		a /= 10;
	}
    
	for(i=cnt-1; i>-1; i--) {
		*p = buff[i];
		p++;
	}
	*p = '\0';
	return s;
}

char *my_ultoh(char *s, unsigned long n) {
	unsigned long a = n, b;
	int cnt, i;
	char *p;
	char buff[100];

	p = s;
	cnt = 0;
	while(a != 0) {
		b = a % 16;

		if (b < 10) 
			b += 0x30;
		else
			b += 0x60 - 9;

		buff[cnt++] = (char) b;

		a /= 16;
	}
    *p = '0'; p++;
	*p = 'x'; p++;
    *p = '0'; p++;

	for(i=cnt-1; i>-1; i--) {
		*p = buff[i];
		p++;
	}
	*p = '\0';

	return s;
}
