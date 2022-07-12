#include <torilos/trlock.h>

void tlck_stop_intr(){}

void tlck_allow_intr(){}

void tlck_lock(int* p_lock, int lock_type) {
	while(1) {
		if (!(*p_lock)) {
			*p_lock = 1;
			break;
		}
	}
}

void tlck_release(int* p_lock, int lock_type) {
	*p_lock = 0;
}
