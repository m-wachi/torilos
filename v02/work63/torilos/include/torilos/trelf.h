#ifndef TRELF_H
#define TRELF_H

#include <torilos/trmempage.h>
#include <torilos/trfile.h>

#define TRELF_SZ_PG_APP_STK		16
#define TRELF_SZ_PG_KNL_STK		2

int trelf_alloc_and_load(TFL_HND_FILE* hnd_file, 
		TRMPG_VM_INF* vminf, unsigned int* entry_addr,
		int sz_pg_app_stk, int sz_pg_knl_stk);
TRMPG_VM_MAP_ITEM* trelf_alloc_load_section(
		TFL_HND_FILE* hnd_file, Elf32_Phdr* phdr);
TRMPG_VM_MAP_ITEM* trelf_alloc_appstk(int sz_pg_app_stk);
TRMPG_VM_MAP_ITEM* trelf_alloc_knlstk(int sz_pg_knl_stk);
int trelf_prep_execve(char* path, char** argv, char** envp);
int trelf_do_execve(int cr3, int esp, unsigned int current_stk_area_addr, 
		char* path, int argc, char** argv, char** envp);
int trelf_run_app(char* filename, int argc, char** argv,
		int envc, char** envv, char* cwd);
#endif


