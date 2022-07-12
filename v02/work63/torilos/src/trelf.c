#include <torilos/trelf.h>
#include <torilos/elf.h>
#include <torilos/mylib01.h>
#include <torilos/trfile.h>
#include <torilos/trfile_opr.h>
#include <torilos/mymm01.h>
#include <torilos/trmempage.h>
#include <torilos/mytask01.h>
#include <torilos/trutil.h>
#include <torilos/debug01.h>
#include <torilos/trlock.h>
#include <torilos/trmempage_mdl.h>
#include <torilos/fcntl.h>
#include <torilos/myutil01.h>
#include <torilos/trpmemmgr_mdl.h>

/**
 * @brief alloc memory and load elf binary
 * 
 * allocate memory and map phisical/virtual address
 * according to elf-header and program-header information.
 * @param hnd_file   elf binary file handler
 * @param vminf		OUT: memory managenemt struct
 * @param entry_addr OUT: entry address
 * @param sz_pg_app_stk app stack page(4kb) size
 * @param sz_pg_knl_stk kernel stack page(4kb) size
 * @return 0: success, <0: error
 */
int trelf_alloc_and_load(TFL_HND_FILE* hnd_file, 
		TRMPG_VM_INF* vminf, unsigned int* entry_addr,
		int sz_pg_app_stk, int sz_pg_knl_stk) {
	TFL_HND_FILE* fdtbl[1];
	unsigned int header_phs_addr;
	int rc, i, idx_vmmap;
	Elf32_Ehdr* elfhdr;
	Elf32_Phdr* phdr;
	TRMPG_VM_MAP_ITEM* vmmap;
	
	fdtbl[0] = hnd_file;
	
	//header_phs_addr = mm_malloc_pg_app(MM_PGSIZE);
	header_phs_addr = tpmmm_malloc_pg_app(MM_PGSIZE);
	
	rc = tflop_read(0, (char*)header_phs_addr, MM_PGSIZE, fdtbl);
	if (!rc) {
		return -1;
	}
	elfhdr = (Elf32_Ehdr*)header_phs_addr;
	*entry_addr = elfhdr->e_entry;
	phdr = (Elf32_Phdr*) (header_phs_addr + elfhdr->e_phoff);

	for (i=0; i<elfhdr->e_phnum; i++) {
		if (phdr->p_type == PT_LOAD) {
			if (!(vmmap = trelf_alloc_load_section(hnd_file, phdr)))
				return -5;
			switch (phdr->p_flags) {
			case 5:
				debug_puts("TRMPG_VMMAP_CODE process.");
				idx_vmmap = TRMPG_VMMAP_CODE; break;
			case 6:
				debug_puts("TRMPG_VMMAP_DATA process.");
				idx_vmmap = TRMPG_VMMAP_DATA; break;
			default:
				return -2;
			}
			vminf->vmmap_hd[idx_vmmap].next = vmmap;

		} else if (phdr->p_type == PT_GNU_STACK) {
			if (!(vmmap = trelf_alloc_appstk(sz_pg_app_stk)))
				return -3;
			debug_puts("TRMPG_VMMAP_USRSTK process.");
			vminf->vmmap_hd[TRMPG_VMMAP_USRSTK].next = vmmap;
		}
		phdr++;
	}
	
	//mm_free(header_phs_addr);
	tpmmm_free(header_phs_addr);
	
	/*
	 * setup kernel stack
	 */
	if (!(vmmap = trelf_alloc_knlstk(sz_pg_knl_stk)))
		return -4;
	vminf->vmmap_hd[TRMPG_VMMAP_KNLSTK].next = vmmap;

	return 0;
}

/**
 * @brief load one 'LOAD' segment from elf binary
 * @param hnd_file file handle of elf file
 * @param phdr elf program header to load
 * @return vmmap of 'LOAD' segment
 */
TRMPG_VM_MAP_ITEM* trelf_alloc_load_section(
		TFL_HND_FILE* hnd_file, Elf32_Phdr* phdr) {

	TFL_HND_FILE* fdtbl[1];
	TRMPG_VM_MAP_ITEM* vmmap;
	char *p;
	int bss_size;
	
	fdtbl[0] = hnd_file;
	if (!(vmmap = trmpg_alloc_vmmap_by_phdr(phdr)))
		return 0;

	//vmmap->phs_addr = mm_malloc_pg_app(vmmap->sz_page * MM_PGSIZE);
	vmmap->phs_addr = tpmmm_malloc_pg_app(vmmap->sz_page * MM_PGSIZE);
	
	tflop_lseek(0, phdr->p_offset, SEEK_SET, fdtbl);
	p = (char*)(vmmap->phs_addr + trutl_mod4k(phdr->p_offset));
	tflop_read(0, p,	phdr->p_filesz, fdtbl);
	bss_size = phdr->p_memsz - phdr->p_filesz;
	if (bss_size) {
		p += phdr->p_filesz;
		my_memset(p, 0, bss_size);
	}
	
	return vmmap;
}

TRMPG_VM_MAP_ITEM* trelf_alloc_appstk(int sz_pg_app_stk) {
	TRMPG_VM_MAP_ITEM* vmmap;

	if (!(vmmap = trmpg_alloc_vmmap_item()))
		return 0;
	
	vmmap->sz_page = sz_pg_app_stk;
	//vmmap->phs_addr = mm_malloc_pg_app(sz_pg_app_stk * MM_PGSIZE);
	vmmap->phs_addr = tpmmm_malloc_pg_app(sz_pg_app_stk * MM_PGSIZE);
	vmmap->ref_count = 1;
	vmmap->vir_addr = TSK_APP_STACK_VIR_ADDR;
	vmmap->next = 0;
	
	return vmmap;
}

TRMPG_VM_MAP_ITEM* trelf_alloc_knlstk(int sz_pg_knl_stk) {
	TRMPG_VM_MAP_ITEM* vmmap;

	if (!(vmmap = trmpg_alloc_vmmap_item()))
		return 0;
	
	vmmap->sz_page = sz_pg_knl_stk;
	//vmmap->phs_addr = mm_malloc_pg_app(sz_pg_knl_stk * MM_PGSIZE);
	vmmap->phs_addr = tpmmm_malloc_pg_app(sz_pg_knl_stk * MM_PGSIZE);
	vmmap->ref_count = 1;
	vmmap->vir_addr = TSK_KNL_STACK_VIR_ADDR;
	vmmap->next = 0;
	
	return vmmap;
}

/**
 * @brief 'execve' preparation routine
 * @param path program file path
 * @param argv command-line parameter values
 * @param envp Environment variables
 * @return esp value after this routine
 */
int trelf_prep_execve(char* path, char** argv, char** envp) {
	int argc=0, envc=0;
	char** p_s;
	unsigned int stk_execve_area_addr, stk_execve_bottom_addr;
	unsigned int esp_value, path_addr;
	unsigned int* p_int;
	char buff[128];
	
	debug_puts("trelf_prep_exec start");

	my_strcpy(buff, "path=");
	my_strcat(buff, path);
	debug_puts(buff);

	debug_varval01("argv", (unsigned int)argv, 1);
	debug_varval01("envp", (unsigned int)envp, 1);
	debug_puts("argv values are...");
	for (p_s=argv; *p_s; p_s++) {
		argc++;
		debug_puts(*p_s);
	}
	debug_puts("envp values are...");
	for (p_s=envp; *p_s; p_s++) {
		envc++;
		debug_puts(*p_s);
	}
	
	
	stk_execve_area_addr = tpmmm_malloc_pg_knl(MM_PGSIZE * 2);
	stk_execve_bottom_addr = stk_execve_area_addr + MM_PGSIZE * 2;

	debug_varval01("stk_execve_bottom_addr", stk_execve_bottom_addr, 1);
	
	path_addr =  stk_execve_bottom_addr - 
		trmpg_push_one_string(stk_execve_bottom_addr, path); 
	
	esp_value = trmpg_prep_argenvs(path_addr, path_addr, 
			argc, argv, envc, envp);
	
	//push path_addr 
	esp_value -= 4;
	p_int = (unsigned int*) esp_value;
	*p_int = path_addr;

	//push stk_execve_area_addr 
	esp_value -= 4;
	p_int = (unsigned int*) esp_value;
	*p_int = stk_execve_area_addr;

	debug_varval01("esp_value", esp_value, 1);

	debug_puts("trelf_prep_exec end");
	
	return esp_value;
}

/**
 * @brief main 'execve' routine
 * @param cr3 INOUT %cr3 value for 'exec' task
 * @param esp INOUT %esp value to restore old context (NEVER CHANGE)
 * @param current_stk_area_addr current stack area addres
 * @param path program file path
 * @param argc command-line parameter count
 * @param argv command-line parameter values
 * @param envp Environment variables
 * @return esp value after this routine.
 * 
 * 'cr3' and 'esp' parameter will use to restore old context when error occur.
 * in addition, 'cr3' value set %cr3 if execve() success. 'cr3' value
 * is referred from assembler code(asm_execve()). 
 */
int trelf_do_execve(int cr3, int esp, unsigned int current_stk_area_addr, 
		char* path, int argc, char** argv, char** envp) {
	char** p_s;
	TFL_HND_FILE* fdtbl[16];

	unsigned int /*pt_addr,*/ entry_addr;
	unsigned int usrstk_bottom_phs_addr, usrstk_bottom_vir_addr, esp_value;
	unsigned int knlstk_bottom_phs_addr;
	int fd, rc, i, envc=0, knl_vir_esp_value, offset;
	TRMPG_VM_INF* vminf;
	TRMPG_VM_INF* old_vminf;
	//TSK_QUEENTRY* tsk_ent;
	TRTSK_TASK* cur_task;
	char buff[256];
	
	debug_puts("trelf_do_execve start");

	my_strcpy(buff, "path=");
	my_strcat(buff, path);
	debug_puts(buff);	
//	debug_varval01("argv", (unsigned int)argv, 1);
//	debug_varval01("envp", (unsigned int)envp, 1);
//	debug_puts("argv values are...");
//	for (p_s=argv; *p_s; p_s++) {
//		debug_puts(*p_s);
//	}
//	
//	debug_puts("envp values are...");
	for (p_s=envp; *p_s; p_s++) {
		envc++;
		//debug_puts(*p_s);
	}
	
	tfl_init_userfd(fdtbl);
	
	cur_task = tsk_get_cur_task();

	//
	// copy file data to application area
	//
	if (0 > (fd = tflop_open(path, O_RDONLY, fdtbl, cur_task->cwd))) {
		my_strcat(buff, "argv[0]=");
		my_strcat(buff, argv[0]);
		debug_puts(buff);
		//myutil_panic("execve - open failed!");
		return -1;
	}

	if (!(vminf = tmpgm_alloc_vminf())) {
		//myutil_panic("execve - tmpgm_alloc_vminf failed!");
		return -3;
	}

	vminf->pt_addr = tpmmm_malloc_pg_knl(MM_PGSIZE);
	
	trmpg_init_app_pdt(vminf->pdt_addr);

	my_strcpy(buff, "filename=");
	my_strcat(buff, argv[0]);
	debug_puts(buff);

	if ((rc = trelf_alloc_and_load(fdtbl[fd], vminf, &entry_addr,
			TRELF_SZ_PG_APP_STK, TRELF_SZ_PG_KNL_STK))) {
		debug_varval01("trelf_load error. rc", rc, 0);
		myutil_panic("execve - trelf_load error.");
		return -2;
	}

	tflop_close(fd, fdtbl);

	debug_puts("trelf_load end normally.");

	// set up page-table
	for (i=0; i<TRMPG_SZ_VMMAP_HD; i++) {
		if (vminf->vmmap_hd[i].next) 
			trmpg_set_pt_vmmap(vminf->vmmap_hd[i].next, vminf->pt_addr);
	}

	// set page-table address to page-directory entry 1
	trmpg_set_app_pde(vminf->pdt_addr, 1, vminf->pt_addr);
	
	debug_puts("pdt setup end.");

	/*
	 * prepare 'main' function parameter(**argv)
	 */ 
	usrstk_bottom_phs_addr = vminf->vmmap_hd[TRMPG_VMMAP_USRSTK].next->phs_addr + MM_PGSIZE * TRELF_SZ_PG_APP_STK;
	usrstk_bottom_vir_addr = vminf->vmmap_hd[TRMPG_VMMAP_USRSTK].next->vir_addr + MM_PGSIZE * TRELF_SZ_PG_APP_STK;

	esp_value = trmpg_prep_argenvs(usrstk_bottom_phs_addr, usrstk_bottom_vir_addr,
			argc, argv, envc, envp);

	debug_varval01("esp_value", esp_value, 1);
	
	/*
	 * TASK set up.
	 */
	old_vminf = cur_task->vminf;

	cur_task->vminf = vminf;
	
	cur_task->tss.cr3 = vminf->pdt_addr;
	cur_task->tss.esp0 = vminf->vmmap_hd[TRMPG_VMMAP_KNLSTK].next->vir_addr + MM_PGSIZE * TRELF_SZ_PG_KNL_STK;
	knlstk_bottom_phs_addr = vminf->vmmap_hd[TRMPG_VMMAP_KNLSTK].next->phs_addr + MM_PGSIZE * TRELF_SZ_PG_KNL_STK;

	// set-up kernel stack for IRET
	offset = trmpg_prep_execve_postroutine(knlstk_bottom_phs_addr,
			entry_addr, esp_value);

	knl_vir_esp_value = cur_task->tss.esp0 - offset;
	
	cr3 = vminf->pdt_addr; 
	
	pag_debug_vminf(cur_task->vminf);

	tmpgm_free_vminf(old_vminf);
	
	debug_varval01("current_stk_area_addr", current_stk_area_addr, 1);

	debug_varval01("knl_esp_value", knl_vir_esp_value, 1);
	
	debug_puts("trelf_do_execve end.");
	
	//
	// free stack-area using currently.
	// NEVER ALLOW INTERRUPT UNTIL IRET AFTER 
	// FOLLOWING mm_free()
	//
	
	tpmmm_free(current_stk_area_addr);
	
	esp = knl_vir_esp_value;
	return 0;
}

int trelf_run_app(char* filename, int argc, char** argv,
		int envc, char** envv, char* cwd) {
	TFL_HND_FILE* fdtbl[16];

	unsigned int /*pt_addr,*/ entry_addr;
	unsigned int stk_bottom_phs_addr, stk_bottom_vir_addr, esp_value;
	int fd, rc, i;
	TRMPG_VM_INF* vminf;
	TRTSK_QUEENTRY* tsk_ent;

	debug_puts("start trelf_run_app");
	
	//tlck_stop_intr();

	tfl_init_userfd(fdtbl);

	//
	// copy file data to application area
	//
	if (0 > (fd = tflop_open(filename, O_RDONLY, fdtbl, cwd)))
		return -1;

	if (!(vminf = tmpgm_alloc_vminf()))
		return -3;

	//vminf->pt_addr = mm_malloc_pg_knl(MM_PGSIZE);
	vminf->pt_addr = tpmmm_malloc_pg_knl(MM_PGSIZE);
	
	trmpg_init_app_pdt(vminf->pdt_addr);

	if ((rc = trelf_alloc_and_load(fdtbl[fd], vminf, &entry_addr,
			TRELF_SZ_PG_APP_STK, TRELF_SZ_PG_KNL_STK))) {
		debug_varval01("trelf_load error. rc", rc, 0);
		return -2;
	}

	tflop_close(fd, fdtbl);

	debug_puts("trelf_load end normally.");

	for (i=0; i<TRMPG_SZ_VMMAP_HD; i++) {
		if (vminf->vmmap_hd[i].next) 
			trmpg_set_pt_vmmap(vminf->vmmap_hd[i].next, vminf->pt_addr);
	}

	// set page-table address to page-directory entry 1
	trmpg_set_app_pde(vminf->pdt_addr, 1, vminf->pt_addr);
	
	debug_puts("pdt setup end.");

	/*
	 * prepare 'main' function parameter(**argv)
	 */ 
	stk_bottom_phs_addr = vminf->vmmap_hd[TRMPG_VMMAP_USRSTK].next->phs_addr + MM_PGSIZE * TRELF_SZ_PG_APP_STK;
	stk_bottom_vir_addr = vminf->vmmap_hd[TRMPG_VMMAP_USRSTK].next->vir_addr + MM_PGSIZE * TRELF_SZ_PG_APP_STK;

	
	debug_varval01("argc", argc, 0);
	debug_varval01("stk_bottom_phs_addr", stk_bottom_phs_addr, 1);
	
	esp_value = trmpg_prep_argenvs(stk_bottom_phs_addr, stk_bottom_vir_addr,
			argc, argv, envc, envv);

	debug_varval01("esp_value", esp_value, 1);


	/*
	 * create new task
	 */ 
	tsk_ent = tsk_new_task(4*8+3, 3*8+3, entry_addr, vminf->pdt_addr, cwd);

	tsk_ent->task.vminf = vminf;

	tsk_ent->task.tss.esp = esp_value;
	tsk_ent->task.tss.esp0 = vminf->vmmap_hd[TRMPG_VMMAP_KNLSTK].next->vir_addr + MM_PGSIZE * TRELF_SZ_PG_KNL_STK;

	tsk_debugout_tss(&tsk_ent->task.tss);

	pag_debug_vminf(tsk_ent->task.vminf);

	tsk_add_to_running(tsk_ent);
	
	//tlck_allow_intr();

	debug_puts("end trelf_run_app normally.");
	
	return 0;

}


