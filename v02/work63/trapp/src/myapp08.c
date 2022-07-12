//#include "syscl01.h"
//#include "../mylib01.h"
//#include "../myapputil01.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trinclude/trutil.h"
#include "trinclude/syscl01.h"

int main(int argc, char** argv);

int main(int argc, char** argv) {

	unsigned long start_tick;
	unsigned long tick;
	unsigned long end_tick;
	//unsigned long wait_tick;
	int sec_cnt, prev_sec_cnt;
	static char msg[] = "hello app08";
	char buff[80];
	char buff2[80];

	//write string
	//syscall(0x04, (unsigned int)msg, 0x43, 0x44, 0x45, 0x46, 0x47);
	puts(msg);
	
	//get_tick
	syscall(0x17, (unsigned int)&tick, 0, 0, 0, 0, 0);

	sprint_varval(buff, "cur tick", tick, 0);

	puts(buff);

	end_tick = tick + 1000;
	start_tick = tick;

	sprint_varval(buff, "wait until ", end_tick, 0);

	strcat(buff, " tick.");

	puts(buff);

	prev_sec_cnt = sec_cnt = 0;
	while(tick < end_tick) {
		syscall(0x17, (unsigned int)&tick, 0, 0, 0, 0, 0);

		syscall(0x08, 100, 0, 0, 0, 0, 0);

		//prev_sec_cnt = sec_cnt;
		sec_cnt = (tick - start_tick) / 100;
		//if(prev_sec_cnt != sec_cnt) {
		strcpy(buff, "app8: ");
		sprint_varval(buff2, "sec", sec_cnt, 0);
		strcat(buff, buff2);
		puts(buff);
		//}

	}

	puts("waiting done.");

	//exit
	exit(0);
	
}

