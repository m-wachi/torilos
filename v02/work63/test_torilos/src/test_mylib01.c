#include <stdio.h>
#include <string.h>

#include <torilos/mylib01.h>

int main(int argc, char** argv) {

	int a3;
	char buff[1024];

	my_strcpy(buff, "hello");

	puts(buff);

	my_strncpy(buff, "abcdef", 5);
	buff[5] = '\0';

	puts(buff);

	memset(buff, 0, sizeof(buff));
	
	my_memset(buff, 'X', 3);

	puts(buff);

	a3 = my_atoi("321");
	
	printf("a3=%d\n", a3);
	
	return 0;
}
