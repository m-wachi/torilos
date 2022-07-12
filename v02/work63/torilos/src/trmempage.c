/**
 * @file trmempage.c
 * @brief Process Memory utilities module.
 * 
 * These function are used for process memory management, 
 * phisical-virtual memory mapping.
 * This program is inherited from mypage01.c
 */
#include <torilos/trmempage.h>
#include <torilos/debug01.h>
#include <torilos/trutil.h>
#include <torilos/mylib01.h>
#include <torilos/trlock.h>

static TRMPG_VM_INF ml_vminf_tbl[TRMPG_MAX_VM_INF_TBL];

static TRMPG_VM_MAP_ITEM ml_vmmap_tbl[TRMPG_MAX_VMMAP_TBL];
static TRMPG_VM_MAP_ITEM ml_free_vmmap_hd;


/**
 * @brief initialize trmempage module.
 */
void trmpg_init() {
	int i;
	
	for (i=0; i<TRMPG_MAX_VM_INF_TBL; i++) {
		trmpg_init_vminf(&ml_vminf_tbl[i]);
	}
	
	for (i=0; i<TRMPG_MAX_VMMAP_TBL; i++) {
		ml_vmmap_tbl[i].phs_addr = 0;
		ml_vmmap_tbl[i].vir_addr = 0;
		ml_vmmap_tbl[i].sz_page = 0;
		ml_vmmap_tbl[i].ref_count = 0;
		ml_vmmap_tbl[i].next = &ml_vmmap_tbl[i+1];
	}
	ml_vmmap_tbl[i-1].next = 0;
	ml_free_vmmap_hd.next = &ml_vmmap_tbl[0];
}

/**
 * @brief alloc vmmap from free-list
 */
TRMPG_VM_MAP_ITEM* trmpg_alloc_vmmap_item() {
	
	TRMPG_VM_MAP_ITEM* p_vmmap;
	
	//tlck_stop_intr();
	
	p_vmmap = ml_free_vmmap_hd.next;
	
	if (!p_vmmap) return 0;
	
	ml_free_vmmap_hd.next = p_vmmap->next;
	
	p_vmmap->next = 0;
	
	//tlck_allow_intr();

	return p_vmmap;
}

/**
 * @brief free vmmap (add vmmap to free-list)
 * @param vmmap
 */
void trmpg_free_vmmap_item(TRMPG_VM_MAP_ITEM* vmmap) {

	//tlck_stop_intr();

	vmmap->next = ml_free_vmmap_hd.next;
	ml_free_vmmap_hd.next = vmmap;
	
	//tlck_allow_intr();
	
}

/**
 * @brief count the number of free vmmap
 */
int trmpg_free_vmmap_size() {
	TRMPG_VM_MAP_ITEM* p_vmmap;
	int cnt = 0;
	
	p_vmmap = ml_free_vmmap_hd.next;

	while(p_vmmap) {
		cnt++;
		p_vmmap = p_vmmap->next;
	}

	return cnt;
}

/**
 * @brief allocate vmmap according to Elf-Program-Header.
 * @param phdr Elf-Program-Header
 * @return allocated vmmap
 * 
 * allocate one vmmap by one segment(one program-header).
 */
TRMPG_VM_MAP_ITEM* trmpg_alloc_vmmap_by_phdr(Elf32_Phdr* phdr) {
	TRMPG_VM_MAP_ITEM* p_vmmap;
	
	if (!(p_vmmap = trmpg_alloc_vmmap_item())) {
		return 0;
	}
	p_vmmap->sz_page = trutl_div4k(trutl_mod4k(phdr->p_vaddr) + phdr->p_memsz, 1);
	p_vmmap->vir_addr = trutl_align4k(phdr->p_vaddr);
	p_vmmap->ref_count = 1;
	p_vmmap->next = 0;
	//p_vmmap->phs_addr isn't set, will be set later
	
	return p_vmmap;
}

/**
 * @brief free new PAG_VMINF
 */
void trmpg_init_vminf(TRMPG_VM_INF* vminf) {
	int i;
	vminf->pdt_addr = 0;
	vminf->pt_addr = 0;
	vminf->free = 1;
	for (i=0; i<TRMPG_SZ_VMMAP_HD; i++) {
		vminf->vmmap_hd[i].next = 0;
	}	
}

/**
 * @brief allocate new VMINF
 */
TRMPG_VM_INF* trmpg_alloc_vminf(unsigned int pdt_addr) {
	int i;

	for (i=0; i<TRMPG_MAX_VM_INF_TBL; i++) {
		if (1 == ml_vminf_tbl[i].free) {
			ml_vminf_tbl[i].pdt_addr = pdt_addr;
			ml_vminf_tbl[i].free = 0;
			return &(ml_vminf_tbl[i]);
		}
	}

	return 0;
}

/**
 * @brief free new PAG_VMINF
 * @param vminf vminf to be free
 */
void trmpg_free_vminf(TRMPG_VM_INF* vminf) {
	
	vminf->pdt_addr = 0;
	vminf->pt_addr = 0;
	vminf->free = 1;
}

int trmpg_free_vminf_size() {
	int i, cnt = 0;
	
	for (i=0; i<TRMPG_MAX_VM_INF_TBL; i++) {
		if (!(ml_vminf_tbl[i].pdt_addr)) cnt++;
	}

	return cnt;
	
}

/**
 * @brief initialize pdt for application
 * @param pdt_addr Page-Directory-Table base address
 */
void trmpg_init_app_pdt(unsigned int pdt_addr) {
	unsigned int pde;
	unsigned int* pdt;
	int i;

	// first entry (use pt1 for kernel)
	pdt = (unsigned int*)pdt_addr;
	pde = TRMPG_FIRST_PT_ADDR + TRMPG_LOW3BIT_KNL;
	*pdt = pde;
	pdt++;

	//others initialize 0
	for(i=1; i<1024; i++) {
		*pdt = 0;
		pdt++;
	}

	// VESA VRAM PDTE set
	pdt = (unsigned int*) (pdt_addr + 0xe00);
	pde = TRMPG_VRAM_PT_ADDR + TRMPG_LOW3BIT_KNL;
	*pdt = pde;

}

/**
 * @brief initialize page-table entry
 */
void trmpg_init_pt(unsigned int pt_addr) {
	unsigned int* p_pte;
	int i;

	p_pte = (unsigned int*)pt_addr;

	for (i=0; i<1024; i++) {
		*p_pte = 0;
		p_pte++;
	}
	
}

/**
 * @brief set pdt entry
 * @param pdt_addr Page-Directory-Table base address
 * @param idx_pdt of Page-Directory-Table entry
 * @param pt_addr Page-Table base address
 */
void trmpg_set_app_pde(unsigned int pdt_addr, int idx_pdt, unsigned int pt_addr) {
	unsigned int* p_pde;
	
	p_pde = (unsigned int*) pdt_addr;
	
	p_pde += idx_pdt;
	
	*p_pde = pt_addr + TRMPG_LOW3BIT_APP;

}

/**
 * @brief set page-table according to PAG_VM_MAP_ITEM
 * @param vmmap
 * @param pt_addr Page-Table base address
 */
void trmpg_set_pt_vmmap(TRMPG_VM_MAP_ITEM* vmmap, 
					  unsigned int pt_addr) {

	unsigned int* p_pte;
	unsigned int pte;

	int i;
	int idx_pt;

	if (vmmap->sz_page > 48) {
		debug_varval01("Warning!!! sz_page is large value sz_page", 
				vmmap->sz_page, 0);
	}
		
	idx_pt = (vmmap->vir_addr - 0x400000) / 0x1000;

	debug_varval01("vmmap->phs_addr", vmmap->phs_addr, 1);
	debug_varval01("vmmap->vir_addr", vmmap->vir_addr, 1);
	debug_varval01("vmmap->sz_page", vmmap->sz_page, 1);
	debug_varval01("idx_pt", idx_pt, 0);

	p_pte = (unsigned int*)pt_addr;

	p_pte += idx_pt;
	pte = vmmap->phs_addr;
	
	
	for (i=0; i<vmmap->sz_page; i++) {
		*p_pte = pte + TRMPG_LOW3BIT_APP;
		pte += 0x1000;
		p_pte++;
	}

}

/**
 * @brief prepare '**argv' values on stack area
 * @param stk_bottom_phs_addr phisical address of stack bottom
 * @param stk_bottom_vir_addr virtual address of stack bottom
 * @param argc 'argc' of application 'main' function
 * @param argv '**argv' of application 'main' function
 * @param envc count of envv values
 * @param envv Environment-Variable array.
 * @return esp value 
 * 
 * see 'understanding linux boot process' book. P175, P183.
 * but there are some difference from linux.
 * stack bottom set-up following structure.
 * 
 * (1:esp+0) argc
 * (2:esp+4) the pointer of argv (point 4) 
 * (3:esp+8) the pointer of env  (point 6) 
 * (4) argv (array of char*) 
 * (5) argv value area
 * (6) envv (array of char*)
 * (7) envv value area (end stk_bottom_phs_addr - 1)
 */
unsigned int trmpg_prep_argenvs(int stk_bottom_phs_addr, 
		int stk_bottom_vir_addr, int argc, char** argv, int envc, char** envv) {
	int argoffset[256];
	int envoffset[256];
	//unsigned int cur_phs_btm_addr, cur_vir_btm_addr;
	unsigned int* p_int;
	int total_offset = 0, temp_vir_offset;
	unsigned int envvp;
	unsigned int argvp;
	
	total_offset += trmpg_push_strings(envoffset, stk_bottom_phs_addr, 
			envc, envv);
	
	total_offset = (total_offset + 3) & 0xfffffffc;	//align 4byte
	
	total_offset += trmpg_push_strptrs(stk_bottom_phs_addr - total_offset, 
			stk_bottom_vir_addr, envc, envoffset);
	
	temp_vir_offset = total_offset;
	envvp = stk_bottom_vir_addr - total_offset;
	
	total_offset += trmpg_push_strings(argoffset, stk_bottom_phs_addr - total_offset, 
			argc, argv);
	
	total_offset = (total_offset + 3) & 0xfffffffc;	//align 4byte

	total_offset += trmpg_push_strptrs(stk_bottom_phs_addr - total_offset, 
			stk_bottom_vir_addr - temp_vir_offset, argc, argoffset);

	argvp = stk_bottom_vir_addr - total_offset;

	
	p_int = (unsigned int*)(stk_bottom_phs_addr - total_offset);
	*--p_int = envvp;	//push envvp
	*--p_int = argvp;	//push argvp
	*--p_int = (unsigned int)argc;		//push argc
	
	return argvp - 4*3;
}
/**
 * @brief set string pointers to stack bottom
 * @param bottom_phs_addr phisical address of stack bottom
 * @param bottom_vir_addr virtual address of stack bottom
 * @param argc the number of string pointers
 * @param argoffset string pointer array
 * @return offset from bottom_phs_addr after set
 */
int trmpg_push_strptrs(unsigned int bottom_phs_addr, 
		unsigned int bottom_vir_addr, int argc, int* argoffset) {
	int i;
	unsigned int* p;
	
	p = (unsigned int*)bottom_phs_addr;
	
	*--p = 0;
	
	//set argv addr for argv[]
	for (i=argc-1; 0 <= i; i--) {
		*--p = bottom_vir_addr - argoffset[i];
	}
	
	return bottom_phs_addr - (unsigned int)p;
}

/**
 * @brief copy strings to stack bottom
 * @param argoffset array of offset from bottom_addr
 * @param bottom_addr stack bottom address
 * @param argc the number of strings
 * @param argv the value of strings
 * @return offset from bottom addr after copy
 */   
int trmpg_push_strings(int* argoffset, unsigned int bottom_addr, 
		int argc, char** argv) {
	char* p;
	int i, sz_data;
	int offset_sum;
	
	p = (char*)bottom_addr;
	offset_sum = 0;

	for (i = argc-1; 0 <= i; i--) {
//		sz_data = my_strlen(argv[i]) + 1;
//		p -= sz_data;
//		my_strcpy(p, argv[i]);
//		offset_sum += sz_data;
		offset_sum += trmpg_push_one_string(bottom_addr - offset_sum, argv[i]);
		argoffset[i] = offset_sum;
	}
	return offset_sum;
}
/**
 * @brief copy one string to stack bottom
 * @param bottom_addr stack bottom address
 * @param s copying string 
 * @return offset from bottom addr after copy
 */
int trmpg_push_one_string(unsigned int bottom_addr, char* s) {
	char* p;
	int sz_data;

	p = (char*)bottom_addr;
	sz_data = my_strlen(s) + 1;
	p -= sz_data;
	my_strcpy(p, s);
	return sz_data;
}



/**
 * @brief prepare execve post-routine
 * @param bottom_addr
 * @param eip
 * @param user_esp user-mode esp value
 * @return offset from bottom address
 * execve post-routine do followings
 * @verbatim
	mov %eax, %esp 
	iret
   @endverbatim
 * so prepare the bottom of stack data.
 */
unsigned int trmpg_prep_execve_postroutine(unsigned int bottom_addr,
		int eip, int user_esp) {
	unsigned int* p_int;
	
	p_int = (unsigned int*)bottom_addr;
	*--p_int = 3*8+3;	//ss
	*--p_int = user_esp;	//esp
	*--p_int = 0x00000202;	//eflags
	*--p_int = 4*8+3; 	//cs
	*--p_int = eip;		//eip
	
	return bottom_addr - (unsigned int)p_int;
}


void trmpg_debug_vmmap_item(TRMPG_VM_MAP_ITEM* vmmap) {
	char buff[120], buff2[120];
	
	sprint_varval(buff, "phs_addr", vmmap->phs_addr, 1);
	sprint_varval(buff2, ", vir_addr", vmmap->vir_addr, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", sz_page", vmmap->sz_page, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", ref_count", vmmap->ref_count, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", next", (int)vmmap->next, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);
}


void pag_debug_vminf(TRMPG_VM_INF* vminf) {
	char buff[81];
	int i;

	debug_puts("### dump vminf ###");

	sprint_varval(buff, "pdt_addr", vminf->pdt_addr, 1);
	debug_puts(buff);
	
	for (i=0; i<TRMPG_SZ_VMMAP_HD; i++) {
		debug_varval01("vmmap_type", i, 0);
		if (vminf->vmmap_hd[i].next)
			trmpg_debug_vmmap_item(vminf->vmmap_hd[i].next);
	}		  
}


