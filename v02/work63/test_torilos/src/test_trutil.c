#include <stdio.h>
#include <string.h>
#include <torilos/mylib01.h>
#include <torilos/trutil.h>

int main(int argc, char** argv) {
	int rc_int, errcnt=0;
	
	int a1 = 123;
	int a2 = 0xf3e;
	char buff[1024];
	char buff2[1024];
	unsigned int rc_ui;
	unsigned long l1 = 12345678;

	my_itoa(buff, a1);
	if (strcmp("123", buff)) {
		fprintf(stderr, "Failed - my_itoa()=%s\n", buff);
		errcnt++;
	}
	
	my_itoh(buff, a2);
	if (strcmp(buff, "0x0f3e")) {
		fprintf(stderr, "Failed - my_itoh()=%s\n", buff);
		errcnt++;
	}
		
	my_ultoa(buff, l1);
	if (strcmp(buff, "12345678")) {
		fprintf(stderr, "Failed - my_ultoa()=%s\n", buff);
		errcnt++;
	}

	my_ultoh(buff, l1);
	if (strcmp(buff, "0x0bc614e")) {
		fprintf(stderr, "Failed - my_ultoh()=%s\n", buff);
		errcnt++;
	}
	

	my_strcpy(buff, "a1=");

	my_strcat(buff, my_itoh(buff2, a1));

	if (strcmp("a1=0x07b", buff)) {
		fprintf(stderr, "Failed - my_strcpy(), my_strcat()=%s\n", buff);
		errcnt++;
	}
	
	my_strncpy(buff, "abcdef", 5);
	buff[5] = '\0';
	if (strcmp("abcde", buff)) {
		fprintf(stderr, "Failed - my_strncpy()=%s\n", buff);
		errcnt++;
	}

	my_memset(buff, 'X', 3);
	if (strcmp("XXXde", buff)) {
		fprintf(stderr, "Failed - my_memset()=%s\n", buff);
		errcnt++;
	}

	
	rc_int = my_stridx("abcdef", 'c');
	if (rc_int != 2) {
		fprintf(stderr, "Failed - my_stridx()=%d\n", rc_int);
		errcnt++;
	}
	
	rc_int = my_stridx("abcdef", 'g');
	if (rc_int != -1) {
		fprintf(stderr, "Failed - my_stridx()=%d\n", rc_int);
		errcnt++;
	}
	
	rc_int = my_stridx("abcdcf", 'c');
	if (rc_int != 2) {
		fprintf(stderr, "Failed - my_stridx()=%d\n", rc_int);
		errcnt++;
	}
	
	rc_ui = trutl_div4k(0x7654321, 0);
	if (rc_ui != 0x7654) {
		fprintf(stderr, "Failed - trutl_div4k()=%#x\n", rc_ui);
		errcnt++;
	}
	
	rc_ui = trutl_div4k(0x7654321, 1);
	if (rc_ui != 0x7655) {
		fprintf(stderr, "Failed - trutl_div4k()=%#x\n", rc_ui);
		errcnt++;
	}

	rc_ui = trutl_mod4k(0x7654321);
	if (rc_ui != 0x321) {
		fprintf(stderr, "Failed - trutl_mod4k()=%#x\n", rc_ui);
		errcnt++;
	}
	
	if (errcnt)
		return -1;
		
	puts("OK - test passed.");
	return 0;

}
