#ifndef TRPMEMMGR_MDL_H
#define TRPMEMMGR_MDL_H

void tpmmm_init();
unsigned int tpmmm_malloc_pg_app(int size);
unsigned int tpmmm_malloc_pg_knl(int size);
void tpmmm_free(unsigned int addr);

#endif
