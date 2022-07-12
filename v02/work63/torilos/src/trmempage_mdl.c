#include <torilos/trmempage_mdl.h>
#include <torilos/mylib01.h>
#include <torilos/mymm01.h>
#include <torilos/debug01.h>
#include <torilos/trutil.h>
#include <torilos/trlock.h>
#include <torilos/trpmemmgr_mdl.h>

/**
 * @brief duplicate vmmap
 * 
 * clone vmmap for 'fork()', 
 * copy memory content based on phisical address optionally.
 * @param src_vmmap
 * @param flg_copy	0: not copy,  1: copy
 * @return duplicated vmmap. 0 if error
 */
TRMPG_VM_MAP_ITEM* tmpgm_fork_vmmap(TRMPG_VM_MAP_ITEM* src_vmmap, 
		int flg_copy) {
	TRMPG_VM_MAP_ITEM* dst_vmmap;
	int sz_copy;
	
	if (!(dst_vmmap = trmpg_alloc_vmmap_item())) {
		return 0;
	}
	
	sz_copy = MM_PGSIZE * src_vmmap->sz_page;
	
	my_memcpy(dst_vmmap, src_vmmap, sizeof(TRMPG_VM_MAP_ITEM));
	//dst_vmmap->phs_addr = mm_malloc_pg_app(sz_copy);
	dst_vmmap->phs_addr = tpmmm_malloc_pg_app(sz_copy);
	
	if (flg_copy) {
		my_memcpy((void*)dst_vmmap->phs_addr, (void*)src_vmmap->phs_addr, 
				sz_copy);
	}
	
	return dst_vmmap;
	
}

/**
 * @brief free vmmap.phs_addr and free vmmap 
 * @param vmmap vmmap to be free
 */
void tmpgm_free_vmmap_item(TRMPG_VM_MAP_ITEM* vmmap) {

	//mm_free(vmmap->phs_addr);
	tpmmm_free(vmmap->phs_addr);
	
	trmpg_free_vmmap_item(vmmap);
	
}

/**
 * @brief free all vmmap in the list
 * @param prev_vmmap the previous item of vmmap list to be free.
 */
void tmpgm_free_vmmap_list(TRMPG_VM_MAP_ITEM* prev_vmmap) {
	TRMPG_VM_MAP_ITEM* cur_vmmap;
	TRMPG_VM_MAP_ITEM* next_vmmap;
	
	//tlck_stop_intr();

	cur_vmmap = prev_vmmap->next;
	prev_vmmap->next = 0;
	
	while(cur_vmmap) {
		next_vmmap = cur_vmmap->next;
		debug_varval01("free vmmap addr", cur_vmmap->phs_addr, 1);
		tmpgm_free_vmmap_item(cur_vmmap);
		cur_vmmap = next_vmmap;
	}

	//tlck_allow_intr();
	
}

/**
 * @brief allocate new VMINF with pdt memory
 * @return allocated vminf
 */
TRMPG_VM_INF* tmpgm_alloc_vminf() {
	unsigned int pdt_addr;
	
	//pdt_addr = mm_malloc_pg_knl(MM_PGSIZE);
	pdt_addr = tpmmm_malloc_pg_knl(MM_PGSIZE);
	
	return trmpg_alloc_vminf(pdt_addr);
}

/**
 * @brief copy pdt for 'fork()'
 *
 * @param src_vminf			copy source vminf
 * @return              PAG_VM_INF*   !=0: success, =0: error
 */
TRMPG_VM_INF* tmpgm_clone_appelf_pdt(TRMPG_VM_INF* src_vminf) { 

	unsigned int pt_addr, code_phs_addr; 
	TRMPG_VM_INF* dst_vminf;
	TRMPG_VM_MAP_ITEM* vmmap;
	
	//dst_vminf = trmpg_alloc_vminf(pdt_addr);
	dst_vminf = tmpgm_alloc_vminf();
	if (!dst_vminf)
		return 0;

 	trmpg_init_app_pdt(dst_vminf->pdt_addr);

 	//pt_addr = mm_malloc_pg_knl(MM_PGSIZE);
 	pt_addr = tpmmm_malloc_pg_knl(MM_PGSIZE);
 	dst_vminf->pt_addr = pt_addr;
 	
	// initialize page-table
	trmpg_init_pt(pt_addr);

	/*
	 * text section setting (share src)
	 */
	code_phs_addr = src_vminf->vmmap_hd[TRMPG_VMMAP_CODE].next->phs_addr;
	vmmap = src_vminf->vmmap_hd[TRMPG_VMMAP_CODE].next;
	vmmap->ref_count++;
	dst_vminf->vmmap_hd[TRMPG_VMMAP_CODE].next = vmmap;

	// set page-table according to vmmap
	trmpg_set_pt_vmmap(vmmap, pt_addr);

	/*
	 * data section setting. (copy src)
	 * (some of programs don't have a data-section)
	 */
	if (src_vminf->vmmap_hd[TRMPG_VMMAP_DATA].next) {
		if (!(vmmap = tmpgm_fork_vmmap(
				src_vminf->vmmap_hd[TRMPG_VMMAP_DATA].next, 1)))
			return 0;
		dst_vminf->vmmap_hd[TRMPG_VMMAP_DATA].next = vmmap;
	
		// set page-table according to vmmap
		trmpg_set_pt_vmmap(vmmap, pt_addr);
	}

	/*
	 * user-stack area setting
	 */
	if (!(vmmap = tmpgm_fork_vmmap(
			src_vminf->vmmap_hd[TRMPG_VMMAP_USRSTK].next, 1)))
		return 0;
	dst_vminf->vmmap_hd[TRMPG_VMMAP_USRSTK].next = vmmap;

	// set page-table according to vmmap
	trmpg_set_pt_vmmap(vmmap, pt_addr);

	/*
	 * kernel-stack area setting
	 */
	if (!(vmmap = tmpgm_fork_vmmap(
			src_vminf->vmmap_hd[TRMPG_VMMAP_KNLSTK].next, 1)))
		return 0;
	dst_vminf->vmmap_hd[TRMPG_VMMAP_KNLSTK].next = vmmap;

	// set page-table according to vmmap
	trmpg_set_pt_vmmap(vmmap, pt_addr);
	
	// set page-table address to page-directory entry 1
	trmpg_set_app_pde(dst_vminf->pdt_addr, 1, pt_addr);
	
	return dst_vminf;
}

/**
 * @brief free vminf and allocated memory
 * @param vminf
 */
void tmpgm_free_vminf(TRMPG_VM_INF* vminf) {
	int i;
	TRMPG_VM_MAP_ITEM* vmmap;

	for (i=0; i<TRMPG_SZ_VMMAP_HD; i++) {
		vmmap = vminf->vmmap_hd[i].next;
		if (vmmap) {
			if (1 < vmmap->ref_count) {
				vmmap->ref_count--;
			} else {
				tmpgm_free_vmmap_list(&(vminf->vmmap_hd[i]));
			}
		}
	}

	if(vminf->pt_addr) {
		//mm_free(vminf->pt_addr);
		tpmmm_free(vminf->pt_addr);
		debug_varval01("free pt addr", vminf->pt_addr, 1);
	}
	
	if(vminf->pdt_addr) {
		//mm_free(vminf->pdt_addr);
		tpmmm_free(vminf->pdt_addr);
		debug_varval01("free pdt addr", vminf->pdt_addr, 1);
		trmpg_free_vminf(vminf);
	}
	
}

/**
 * @brief 'brk()' main routine
 * @vminf current task vminf
 * @addr new brk-address(the end accessible of data-segment address)
 * @return 0: success, !=0: error
 */
int tmpgm_brk(TRMPG_VM_INF* vminf, unsigned int addr) {
	TRMPG_VM_MAP_ITEM* cur_data_vmmap;
	TRMPG_VM_MAP_ITEM* prev_data_vmmap;
	TRMPG_VM_MAP_ITEM* new_vmmap;
	unsigned int cur_bottom_addr;
	unsigned int sz_diff;
	unsigned int sz_diff_pg;
	unsigned int phs_addr;

	//XXX remove this limitation(reminder)
	if (addr >= 0x480000) {
		return -1;
	}

	tlck_stop_intr();
	
	prev_data_vmmap = vminf->vmmap_hd[TRMPG_VMMAP_DATA].next;

	if (!prev_data_vmmap)
		return -1;	//no process has data-segment.

	cur_data_vmmap = prev_data_vmmap->next;

	// move-forward with checking decreasing
	while (cur_data_vmmap) {
		if (addr < cur_data_vmmap->vir_addr) {
			// decrease brk-address
			tmpgm_free_vmmap_list(prev_data_vmmap);
			return 0;
		}
		prev_data_vmmap = cur_data_vmmap;
		cur_data_vmmap = cur_data_vmmap->next;
	}

	//
	// brk-value increasing routine.
	//
	cur_bottom_addr = prev_data_vmmap->vir_addr + prev_data_vmmap->sz_page * MM_PGSIZE;
	
	if (cur_bottom_addr > addr)	// new brk-value is in the end ofallocated page.  
		return 0;
		
	//
	// I think 'addr' means 'the end of address which the program can access'.
	// so kernel must allocate new page if cur_bootm_addr = addr.
	//
	sz_diff = addr - cur_bottom_addr + 1;
	sz_diff_pg = trutl_div4k(sz_diff, 1);
	
	//phs_addr = mm_malloc_pg_app(MM_PGSIZE * sz_diff_pg);
	phs_addr = tpmmm_malloc_pg_app(MM_PGSIZE * sz_diff_pg);
	
	new_vmmap = trmpg_alloc_vmmap_item();
	
	new_vmmap->phs_addr = phs_addr;
	new_vmmap->vir_addr = cur_bottom_addr;
	new_vmmap->sz_page = sz_diff_pg;
	new_vmmap->next = 0;
	new_vmmap->ref_count = 1;
	prev_data_vmmap->next = new_vmmap;
	
	trmpg_set_pt_vmmap(new_vmmap, vminf->pt_addr);

	tlck_allow_intr();

	return 0;
}

