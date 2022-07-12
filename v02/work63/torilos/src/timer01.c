#include <torilos/myasmfunc01.h>
#include <torilos/timer01.h>


void init_pit() {

	/* 0x34   -> 00110100
	 * bit0    - count mode - 0: binary count
	 * bit1-3  - count mode - 011: box wave rate generator ???
	 * bit4-5  - counter access mode - 01: low 8bit lead load ???
	 * bit6-7  - counter channel - 00: counter0(io_addr=0x40)
	 */
	io_out8(PIT_CTRL, 0x34);

	/*
	 * PIT clock rate = 1.19318MHz
	 * counter = 11932 -> 100Hz -> 100interrupt/sec
	 * 11932 = 0x2e9c
	 */
	
	io_out8(PIT_CNT0, 0x9c);	/* write lower byte value */
	io_out8(PIT_CNT0, 0x2e);	/* write higher byte value */
	
	return;
}
