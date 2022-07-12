#include <torilos/myasmfunc01.h>
#include <torilos/dsctbl01.h>

void init_gdtidt(void) {
	
	SEGMENT_DESCRIPTOR *gdt;
	GATE_DESCRIPTOR *idt;
	int i, hdlr_addr;
	
	gdt = (SEGMENT_DESCRIPTOR*)SEGDSC_ADDR;
	idt = (GATE_DESCRIPTOR*) GTDSC_ADDR;

	/* init GDT */
	for (i=0; i<8192; i++) {
		set_segmdesc(gdt+i, 0, 0, 0);
	}

	/* read-write for system */
	set_segmdesc(gdt+1, 0xffffffff, 0x00000000, AR_DATA32_RW_R0);
	
	/* 
	 * readonly/executable for system 
	 * mymain was loaded on this segment
	 */
	set_segmdesc(gdt+2, 0xffffffff, 0x0, AR_CODE32_ER_R0);


	/* read-write for application */
	set_segmdesc(gdt+3, 0xffffffff, 0x00000000, AR_DATA32_RW_R3);

	/* 
	 * readonly/executable for application
	 */
	set_segmdesc(gdt+4, 0xffffffff, 0x0, AR_CODE32_ER_R3);


	load_gdtr(0xffff, SEGDSC_ADDR); /* 0xffff=8192*8-1 */

	/* init IDT */
	for (i=0; i<256; i++) {
		set_gatedesc(idt+i, 0, 0, 0);
	}

	/* keyboard interrupt (INT21) */
	//hdlr_addr = (int) asm_inthandler21 - MYMAIN_ADDR;
	hdlr_addr = (int) asm_inthandler21;
	set_gatedesc(idt + 0x21, hdlr_addr, 2*8, AR_INTGATE32);

	/* timer interrupt (INT20) */
	//hdlr_addr = (int) asm_inthandler20 - MYMAIN_ADDR;
	hdlr_addr = (int) asm_inthandler20;
	set_gatedesc(idt + 0x20, hdlr_addr, 2*8, AR_INTGATE32);

	/* FD interrupt (INT26) */
	hdlr_addr = (int) asm_inthandler26;
	set_gatedesc(idt + 0x26, hdlr_addr, 2*8, AR_INTGATE32);

	/* division by 0 (INT0x00) */
	set_gatedesc(idt + 0x00, (int) asm_inthandler00, 2*8, AR_INTGATE32);

	/* debug exception (INT0x01) */
	set_gatedesc(idt + 0x01, (int) asm_inthandler01, 2*8, AR_INTGATE32);

	/* NMI exception (INT0x02) */
	set_gatedesc(idt + 0x02, (int) asm_inthandler02, 2*8, AR_INTGATE32);

	/* Break point exception (INT0x03) */
	set_gatedesc(idt + 0x03, (int) asm_inthandler03, 2*8, AR_INTGATE32);

	/* Overflow exception (INT0x04) */
	set_gatedesc(idt + 0x04, (int) asm_inthandler04, 2*8, AR_INTGATE32);

	/* BOUND exception (INT0x05) */
	set_gatedesc(idt + 0x05, (int) asm_inthandler05, 2*8, AR_INTGATE32);

	/* invalid opcode (INT0x06) */
	hdlr_addr = (int) asm_inthandler06;
	set_gatedesc(idt + 0x06, hdlr_addr, 2*8, AR_INTGATE32);

	/* Device disable exception (INT0x07) */
	set_gatedesc(idt + 0x07, (int) asm_inthandler07, 2*8, AR_INTGATE32);

	/* Double fault (INT0x08) */
	set_gatedesc(idt + 0x08, (int) asm_inthandler08, 2*8, AR_INTGATE32);

	/* Co-processor Segment Overrun (INT0x09) */
	set_gatedesc(idt + 0x09, (int) asm_inthandler09, 2*8, AR_INTGATE32);

	/* invalid TSS (INT0x0a) */
	hdlr_addr = (int) asm_inthandler0a;
	set_gatedesc(idt + 0x0a, hdlr_addr, 2*8, AR_INTGATE32);

	/* segment not exist (INT0x0b) */
	hdlr_addr = (int) asm_inthandler0b;
	set_gatedesc(idt + 0x0b, hdlr_addr, 2*8, AR_INTGATE32);

	/* stack fault (INT0x0c) */
	hdlr_addr = (int) asm_inthandler0c;
	set_gatedesc(idt + 0x0c, hdlr_addr, 2*8, AR_INTGATE32);

	/* general protection exception (INT0x0d) */
	hdlr_addr = (int) asm_inthandler0d;
	set_gatedesc(idt + 0x0d, hdlr_addr, 2*8, AR_INTGATE32);

	/* page fault (INT0x0e) */
	hdlr_addr = (int) asm_inthandler0e;
	set_gatedesc(idt + 0x0e, hdlr_addr, 2*8, AR_INTGATE32);

	/* FPU Floating point number Error (INT0x10) */
	set_gatedesc(idt + 0x10, (int) asm_inthandler_unknown, 2*8, AR_INTGATE32);

	/* Alignment Check Exception (INT0x11) */
	set_gatedesc(idt + 0x11, (int) asm_inthandler_unknown, 2*8, AR_INTGATE32);

	/* Machine Check exception (INT0x12) */
	set_gatedesc(idt + 0x12, (int) asm_inthandler_unknown, 2*8, AR_INTGATE32);

	/* SIMD Floating point number Error (INT0x13) */
	set_gatedesc(idt + 0x13, (int) asm_inthandler_unknown, 2*8, AR_INTGATE32);

	/* system call DPL3 */
	set_gatedesc(idt + 0x80, (int)asm_inthandler80, 2*8, AR_INTGATE32 + 0x60);

	load_idtr(0x7ff, GTDSC_ADDR); /* 0x7ff=256*8-1 */
	
	return;
}

void set_segmdesc(SEGMENT_DESCRIPTOR* sd, unsigned int limit, int base, int ar) {
	if (limit > 0xfffff) {
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low = limit & 0xffff;
	sd->base_low = base & 0xffff;
	sd->base_mid = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high = (base >> 24) & 0xff;
	
	return;
}

void set_gatedesc(GATE_DESCRIPTOR* gd, int offset, int selector, int ar) {
	gd->offset_low = offset & 0xffff;
	gd->selector = selector;
	gd->dw_count = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high = (offset >> 16) & 0xffff;

	return;
}
