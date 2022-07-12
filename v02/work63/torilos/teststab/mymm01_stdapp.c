#include <stdlib.h>

#include <torilos/mymm01.h>

unsigned int mm_malloc_pg_knl(int size) {
	return (unsigned int) malloc(size);
}

unsigned int mm_malloc_pg_app(int size) {
	return (unsigned int) malloc(size);
}

void mm_free(unsigned int addr) {
	free((void*)addr);
}
