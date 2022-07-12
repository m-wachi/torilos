/**
 * @file trlock.c
 * @brief Locking and Interruption control operation Module.
 */

#include <torilos/trlock.h>
#include <torilos/myasmfunc01.h>
#include <torilos/mytask01.h>

void tlck_stop_intr() {
	io_cli();
}

void tlck_allow_intr() {
	io_sti();
}

/**
 * @brief lock operation
 * @param p_lock mutex
 * @param lock_type locking type
 * 
 * NEVER USE THIS FUNCTION WITH TASK QUE OPERATION
 * use tsk_lock_tskques() as alternative
 */
void tlck_lock(int* p_lock, int lock_type) {
	while(1) {
		if (!tlck_swap(p_lock)) break;
		tsk_sleep_by_lock(lock_type);
	}
}
/**
 * @brief lock release operation
 * @param p_lock mutex
 * @param lock_type locking type
 */
void tlck_release(int* p_lock, int lock_type) {
	*p_lock = 0;
	tsk_wakeup_locked_tasks(lock_type);
}

