#ifndef DSCTBL01_H
#define DSCTBL01_H

#define SEGDSC_ADDR 0x00270000
#define GTDSC_ADDR  0x0026f800
/* torilos01 setting
  #define MYMAIN_ADDR 0x00280000
*/

#define MYMAIN_ADDR 0x00100000

#define AR_DATA32_RW_R0 0x4092
#define AR_CODE32_ER_R0 0x409a
#define AR_DATA32_RW_R3 0x40f2
#define AR_CODE32_ER_R3 0x40fa
#define AR_INTGATE32    0x008e
#define AR_TSS32	    0x0089

typedef struct segment_descriptor {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
} SEGMENT_DESCRIPTOR;

typedef struct gate_descriptor {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
} GATE_DESCRIPTOR;


void init_gdtidt(void);
void set_segmdesc(SEGMENT_DESCRIPTOR* sd, unsigned int limit, int base, int ar);
void set_gatedesc(GATE_DESCRIPTOR* gd, int offset, int selector, int ar);

#endif
