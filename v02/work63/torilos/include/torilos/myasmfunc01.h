#ifndef MYASMFUNC01_H
#define MYASMFUNC01_H

int asm_fork(int stk_fork_bottom_addr);
void farjmp(int eip, int cs);

void io_stihlt();
void io_hlt(void);
void io_sti(void);
void io_cli(void);
void io_out8(int port, int data);
void io_out16(int port, int data);
void io_out32(int port, int data);
int io_in8(int port);
int io_in16(int port);
int io_in32(int port);
int io_load_eflags(void);
void io_store_eflags(int eflags);
unsigned int retrieve_cr3();
void load_cr3(unsigned int addr);
void paging_on();
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void load_tr(int tr);
void asm_inthandler_unknown(void);
void asm_inthandler00(void);
void asm_inthandler01(void);
void asm_inthandler02(void);
void asm_inthandler03(void);
void asm_inthandler04(void);
void asm_inthandler05(void);
void asm_inthandler06(void);
void asm_inthandler07(void);
void asm_inthandler08(void);
void asm_inthandler09(void);
void asm_inthandler0a(void);
void asm_inthandler0b(void);
void asm_inthandler0c(void);
void asm_inthandler0d(void);
void asm_inthandler0e(void);
void asm_inthandler21(void);
void asm_inthandler20(void);
void asm_inthandler26(void);
void asm_inthandler80(void);

#endif
