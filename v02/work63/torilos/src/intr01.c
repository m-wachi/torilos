#include <torilos/myasmfunc01.h>
#include <torilos/graphic01.h>
#include <torilos/mylib01.h>
#include <torilos/fifo01.h>
#include <torilos/intr01.h>
#include <torilos/fdc01.h>
#include <torilos/debug01.h>
#include <torilos/mytask01.h>
#include <torilos/myutil01.h>
#include <torilos/trutil.h>
#include <torilos/trfpuctl.h>
#include <torilos/trtermmgr.h>

FIFO8 g_keyque;
unsigned long g_time_counter = 0;
FIFO8 g_floppyque;

unsigned long g_ts_timeout = 0;

static unsigned char keybuf[32];
static unsigned char floppybuf[32];

void init_keybuf() {
	fifo8_init(&g_keyque, sizeof(keybuf), keybuf);
}

void init_floppybuf() {
	fifo8_init(&g_floppyque, sizeof(floppybuf), floppybuf);
}


void init_pic(void) {
	io_out8(PIC0_IMR, 0xff);    /* mask all interrupt by PIC0 */
	io_out8(PIC1_IMR, 0xff);    /* mask all interrupt by PIC1 */

	io_out8(PIC0_ICW1, 0x11);   /* edge trigger mode */
	io_out8(PIC0_ICW2, 0x20);   /* IRQ0-7 -> INT20-27 */
	io_out8(PIC0_ICW3, 1<<2);   /* IRQ2 <- PIC1 interruption */
	io_out8(PIC0_ICW4, 0x01);   /* non-buffer mode */

	io_out8(PIC1_ICW1, 0x11);   /* edge trigger mode */
	io_out8(PIC1_ICW2, 0x28);   /* IRQ8-15 -> INT28-2f */
	io_out8(PIC1_ICW3, 2);      /* IRQ2 <- PIC1 interruption */
	io_out8(PIC1_ICW4, 0x01);   /* non-buffer mode */

	io_out8(PIC0_IMR, 0xfb);    /* 11111011 mask all interrupt without PIC1 */
	io_out8(PIC1_IMR, 0xff);    /* 11111111 mask all interrupt */

	init_keybuf();
	g_time_counter = 0;

	init_floppybuf();
	return;
}

void inthandler_unknown(int *esp) {
	
	debug_puts("Unknown Exception!!");
	while(1) 
		io_hlt();

	return;
}
/*
 * INT0x00: Division by 0 exception handler
 */
void inthandler00(int *esp) {
	
	debug_puts("Division by 0 Exception!");
	while(1) 
		io_hlt();

	return;
}

/*
 * INT0x01: debug exception handler
 */
void inthandler01(int *esp) {
	
	debug_puts("Debug Exception!");
	while(1) 
		io_hlt();

	return;
}

/*
 * INT0x02: NMI exception
 */
void inthandler02(int *esp) {
	
	debug_puts("NMI Exception!");
	while(1) 
		io_hlt();

	return;
}

/*
 * INT0x03: Break point exception
 */
void inthandler03(int *esp) {
	
	debug_puts("Break-Point Exception!");
	while(1) 
		io_hlt();

	return;
}

/*
 * INT0x04: Overflow exception
 */
void inthandler04(int *esp) {
	
	debug_puts("Break-Point Exception!");
	while(1) 
		io_hlt();

	return;
}

/*
 * INT0x05: BOUND exception
 */
void inthandler05(int *esp) {
	
	debug_puts("BOUND Exception!");
	while(1) 
		io_hlt();

	return;
}

/*
 * INT0x06: Invalid opcode exception handler
 */
void inthandler06(int *esp) {
	
	debug_puts("Invalid opcode Exception!");
	while(1) 
		io_hlt();

	return;
}

/*
 * INT0x07: Device disable exception handler
 */
void inthandler07(int esp) {
	
	int *p;
	int eip;
	int cs;
	int eflags;
	char buff[80];
	char buff2[80];
	
	debug_puts("Device disable exception!");
	debug_varval01("esp", esp, 1);
	
	p = (int*)esp;
	p += 9;
	
	//error_code = *p++;
	eip = *p++;
	cs = *p++;
	eflags = *p;
	
	//sprint_varval(buff, "error_code", error_code, 1);
	sprint_varval(buff, "eip", eip, 1);
	//my_strcat(buff, buff2);
	sprint_varval(buff2, ", cs", cs, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", eflags", eflags, 1);
	my_strcat(buff, buff2);
	
	debug_puts(buff);
	debug_refresh();

	trtrm_puts("WARNING!! You might run x87FPU operation.");
	trtrm_puts("          Torilos x87FPU Controller is not accurate.");
	
	//TODO implement x87FPU save/load status and switching.
	asm_clts();
	
	return;
}

/**
 * @brief Double Fault Handler
 */
void inthandler08(int *esp) {
	
	debug_puts("Double Fault!");
	while(1) 
		io_hlt();

	return;
}

/**
 * @brief Co-processor Segment Overrun Handler
 */
void inthandler09(int *esp) {
	
	debug_puts("Co-processor Segment Overrun!");
	while(1) 
		io_hlt();

	return;
}

/*
 * INT0x0a: Invalid TSS exception handler
 */
void inthandler0a(int *esp) {
	
	debug_puts("Invalid TSS Exception!");
	while(1) 
		io_hlt();

	return;
}

/*
 * INT0x0b: segment not exist
 */
void inthandler0b(int *esp) {
	
	debug_puts("Segment not exist Exception!");
	while(1) 
		io_hlt();

	return;
}

/**
 * @brief INT0x0c: stack fault
 */
void inthandler0c(int *esp) {
	
	debug_puts("Stack Fault!");
	while(1) 
		io_hlt();

	return;
}

void inthandler0d(int esp) {
	int *p;
	int error_code;
	int eip;
	int cs;
	int eflags;
	char buff[80];
	char buff2[80];
	
	debug_puts("General Protection Exception!");
	debug_varval01("esp", esp, 1);
	
	p = (int*)esp;
	p += 9;
	
	error_code = *p++;
	eip = *p++;
	cs = *p++;
	eflags = *p;
	
	sprint_varval(buff, "error_code", error_code, 1);
	sprint_varval(buff2, ", eip", eip, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", cs", cs, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", eflags", eflags, 1);
	my_strcat(buff, buff2);
	
	debug_puts(buff);
	debug_refresh();
	while(1) 
		io_hlt();

	return;
}
/**
 * @brief Page-Fault Handler
 */
void inthandler0e(int esp) {
	int *p;
	int error_code;
	int eip;
	int cs;
	int eflags;
	char buff[80];
	char buff2[80];
	
	debug_puts("Page Fault!");
	debug_varval01("esp", esp, 1);
	
	p = (int*)esp;
	p += 9;
	
	error_code = *p++;
	eip = *p++;
	cs = *p++;
	eflags = *p;
	
	sprint_varval(buff, "error_code", error_code, 1);
	sprint_varval(buff2, ", eip", eip, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", cs", cs, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", eflags", eflags, 1);
	my_strcat(buff, buff2);
	
	debug_puts(buff);
	
	while(1) 
		io_hlt();

	return;
}
	
void inthandler20(int *esp) {
	/* notify interruput reception completed */
	io_out8(PIC0_OCW2, 0x60);

	g_time_counter++;

	if (g_time_counter > 0xffffffff)
		g_time_counter = 0;

	/*
	if (g_ts_timeout != 0)
		if (g_time_counter > g_ts_timeout){
			g_ts_timeout = 0;
			io_sti();
			farjmp(0, 5*8);
		}
	*/
	tsk_switch();
	return;
}

/**
 * @brief keyboard interruption handler
 * @param esp
 */
void inthandler21(int *esp) {
	
	char data;

	/* notify interruput reception completed */
	io_out8(PIC0_OCW2, 0x61);

	data = io_in8(PORT_KEYDAT);

	fifo8_put(&g_keyque, data);

	return;
}

void inthandler26(int *esp) {
	
	char data;

	/* null read */
	io_in8(FDC_CSR);

	/* notify interruput reception completed */
	io_out8(PIC0_OCW2, 0x66);

	data = 0;

	fifo8_put(&g_floppyque, data);

	return;
}

