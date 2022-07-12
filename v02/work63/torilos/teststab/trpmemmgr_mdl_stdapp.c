#include <stdlib.h>

#include <torilos/trpmemmgr_mdl.h>


unsigned int tpmmm_malloc_pg_knl(int size) {
	return (unsigned int) malloc(size);
}


unsigned int tpmmm_malloc_pg_app(int size) {
	return (unsigned int) malloc(size);
}

void tpmmm_free(unsigned int addr) {
	free((void*)addr);
}
