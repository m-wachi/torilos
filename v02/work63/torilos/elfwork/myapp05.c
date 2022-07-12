#include "../syscl01.h"
#include "../mylib01.h"
#include "../myapputil01.h"

int main(int argc, char** argv);

int main(int argc, char** argv) {

	unsigned long start_tick;
	unsigned long tick;
	unsigned long end_tick;
	//unsigned long wait_tick;
	int sec_cnt, prev_sec_cnt;
	static char msg[] = "hello app5";
	char buff[80];
	char buff2[80];

	//write string
	syscall(0x04, (unsigned int)msg, 0x43, 0x44, 0x45, 0x46, 0x47);


	//get_tick
	syscall(0x07, (unsigned int)&tick, 0, 0, 0, 0, 0);

	sprint_varval(buff, "cur tick", tick, 0);

	syscall(0x04, (unsigned int)buff, 0, 0, 0, 0, 0);

	end_tick = tick + 1000;
	start_tick = tick;

	sprint_varval(buff, "wait until ", end_tick, 0);

	my_strcat(buff, " tick.");

	syscall(0x04, (unsigned int)buff, 0x43, 0x44, 0x45, 0x46, 0x47);
	
	prev_sec_cnt = sec_cnt = 0;
	while(tick < end_tick) {
		syscall(0x07, (unsigned int)&tick, 0, 0, 0, 0, 0);

		syscall(0x08, 100, 0, 0, 0, 0, 0);

		//prev_sec_cnt = sec_cnt;
		sec_cnt = (tick - start_tick) / 100;
		//if(prev_sec_cnt != sec_cnt) {
		my_strcpy(buff, "app5: ");
		sprint_varval(buff2, "sec", sec_cnt, 0);
		my_strcat(buff, buff2);
		syscall(0x04, (unsigned int)buff, 0, 0, 0, 0, 0);
		//}

	}

	syscall(0x04, (unsigned int)"waiting done.", 0x43, 0x44, 0x45, 0x46, 0x47);

	//exit
	syscall(0x01, 0, 0, 0, 0, 0, 0);
	
}

