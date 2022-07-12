#ifndef TRMEMPAGE_H
#define TRMEMPAGE_H

#include <torilos/elf.h>

#define TRMPG_MAX_VM_INF_TBL   8

#define TRMPG_SZ_VMMAP_HD	4	
#define TRMPG_VMMAP_CODE	0
#define TRMPG_VMMAP_DATA	1
#define TRMPG_VMMAP_USRSTK	2
#define TRMPG_VMMAP_KNLSTK	3

#define TRMPG_MAX_VMMAP_TBL 32

#define TRMPG_LOW3BIT_APP      7
#define TRMPG_LOW3BIT_KNL      3

#define TRMPG_FIRST_PT_ADDR		0x281000
#define TRMPG_VRAM_PT_ADDR		0x283000

/**
 * @brief phisical - virtual address maping data
 */
typedef struct trmpg_vm_map_item {
	unsigned int phs_addr;			//!< phsical address 
	unsigned int vir_addr;			//!< virtual address 
	int sz_page;  					//!< page size
	int ref_count;					//!< reference count
	struct trmpg_vm_map_item* next;	//!< next
} TRMPG_VM_MAP_ITEM;

/**
 * @brief process memory management data
 */
typedef struct trmpg_vm_inf {
	unsigned int pdt_addr; //!< pdt addr (phisical addr)
	//TODO fix to have plural pt_addr
	unsigned int pt_addr; //!< pt addr (phisical addr)
	
	/**
	 * @brief VM_MAP list header
	 * 
	 * vmmap_hd[i] doesn't have data. 
	 * effective data starts from vmmap_hd[i]->next
	 */
	TRMPG_VM_MAP_ITEM vmmap_hd[TRMPG_SZ_VMMAP_HD];
	char free;						//!< free flag 0:used, 1:free
	
	//int sz_vmmap;
} TRMPG_VM_INF;

void trmpg_init();
TRMPG_VM_MAP_ITEM* trmpg_alloc_vmmap_item();
void trmpg_free_vmmap_item(TRMPG_VM_MAP_ITEM* vmmap);
void trmpg_free_vmmap_list(TRMPG_VM_MAP_ITEM* vmmap_top);
int trmpg_free_vmmap_size();

TRMPG_VM_MAP_ITEM* trmpg_alloc_vmmap_by_phdr(Elf32_Phdr* phdr);
void trmpg_init_vminf(TRMPG_VM_INF* vminf);
TRMPG_VM_INF* trmpg_alloc_vminf(unsigned int pdt_addr);
void trmpg_free_vminf(TRMPG_VM_INF* vminf);
int trmpg_free_vminf_size();
void trmpg_init_app_pdt(unsigned int pdt_addr);;
void trmpg_init_pt(unsigned int pt_addr);
void trmpg_set_app_pde(unsigned int pdt_addr, int idx_pdt, unsigned int pt_addr); 
void trmpg_set_pt_vmmap(TRMPG_VM_MAP_ITEM* vmmap,  unsigned int pt_addr);
unsigned int trmpg_prep_argenvs(int stk_bottom_phs_addr, 
		int stk_bottom_vir_addr, int argc, char** argv, int envc, char** envv);

int trmpg_push_strptrs(unsigned int bottom_phs_addr, unsigned int bottom_vir_addr, 
		int argc, int* argoffset);

int trmpg_push_strings(int* argoffset, unsigned int stk_bottom_phs_addr, 
		int argc, char** argv);
int trmpg_push_one_string(unsigned int bottom_addr, char* s);
unsigned int trmpg_prep_execve_postroutine(unsigned int bottom_addr,
		int eip, int user_esp);
void trmpg_debug_vmmap_item(TRMPG_VM_MAP_ITEM* vmmap);
void pag_debug_vminf(TRMPG_VM_INF* vminf);

#endif
