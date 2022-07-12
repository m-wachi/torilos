#ifndef INTR01_H
#define INTR01_H

#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

#define PORT_KEYDAT     0x0060


void init_pic(void);
void inthandler_unknown(int *esp);
void inthandler06(int *esp);
void inthandler08(int *esp);
void inthandler0a(int *esp);
void inthandler0b(int *esp);
void inthandler0d(int esp);
void inthandler0e(int esp);
void inthandler20(int *esp);
void inthandler21(int *esp);
void inthandler26(int *esp);

#endif
