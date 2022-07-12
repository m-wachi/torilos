#ifndef TRMEMPAGE_MDL_H
#define TRMEMPAGE_MDL_H

#include <torilos/trmempage.h>

TRMPG_VM_MAP_ITEM* tmpgm_fork_vmmap(TRMPG_VM_MAP_ITEM* src_vmmap, 
		int flg_copy);
void tmpgm_free_vmmap_item(TRMPG_VM_MAP_ITEM* vmmap);
void tmpgm_free_vmmap_list(TRMPG_VM_MAP_ITEM* vmmap_top);
TRMPG_VM_INF* tmpgm_alloc_vminf();
TRMPG_VM_INF* tmpgm_clone_appelf_pdt(TRMPG_VM_INF* src_vminf); 
void tmpgm_free_vminf(TRMPG_VM_INF* vminf);
int tmpgm_brk(TRMPG_VM_INF* vminf, unsigned int addr);

#endif
