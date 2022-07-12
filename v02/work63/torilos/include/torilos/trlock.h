#ifndef TRLOCK_H
#define TRLOCK_H

#define TLCK_LOCK_MEM			0
#define TLCK_LOCK_IO				1
#define TLCK_CNT_LOCK_TYPE		2

void tlck_stop_intr();

void tlck_allow_intr();

void tlck_lock(int* p_lock, int lock_type);
void tlck_release(int* p_lock, int lock_type);

int tlck_swap(int* p_lock);	//defined in trlock_asm.s

#endif
