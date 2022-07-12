#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <torilos/trmempage.h>
#include <torilos/debug01.h>
#include <torilos/trmempage_mdl.h>

int test_tmpgm_fork_vmmap();
int test_tmpgm_clone_appelf_pdt();
int test_tmpgm_brk();

int main(int argc, char** argv) {

	int errcnt = 0;

	trmpg_init();
	
	errcnt += test_tmpgm_fork_vmmap();
	
	errcnt += test_tmpgm_clone_appelf_pdt();
	
	errcnt += test_tmpgm_brk();
	
	if (errcnt)
		return -1;
		
	puts("OK - test passed.");
	return 0;
}

int test_tmpgm_fork_vmmap() {

	int errcnt = 0, i;
	unsigned int phs_addr0;
	TRMPG_VM_MAP_ITEM* vmmap[3];
	char* p;

	vmmap[0] = trmpg_alloc_vmmap_item();
	vmmap[0]->vir_addr = 0x402000;
	vmmap[0]->sz_page = 2;
	vmmap[0]->ref_count = 1;
	vmmap[0]->next = 0;
	
	//debug_switch(1);
	phs_addr0 = (unsigned int)malloc(4096 * 2);
	
	p = (char*)phs_addr0;
	for (i=0; i< (4096 * 2) / 16; i++) {
		memcpy(p, "0123456789ABCDEF", 16);
		p += 16;
	}
	vmmap[0]->phs_addr = phs_addr0;
	
	vmmap[1] = tmpgm_fork_vmmap(vmmap[0], 1);
	//trmpg_debug_vmmap_item(vmmap[1]);
	
	if (vmmap[1]->vir_addr != 0x402000) {
		fprintf(stderr, "Failed - tmpgm_fork_vmmap(): vir_addr=%#x\n", 
				vmmap[1]->vir_addr);
		errcnt++;
	}
	if (vmmap[1]->sz_page != 2) {
		fprintf(stderr, "Failed - tmpgm_fork_vmmap(): sz_page=%#x\n", 
				vmmap[1]->sz_page);
		errcnt++;
	}
	if (vmmap[1]->ref_count != 1) {
		fprintf(stderr, "Failed - tmpgm_fork_vmmap(): ref_count=%#x\n", 
				vmmap[1]->ref_count);
		errcnt++;
	}
	if (vmmap[1]->next != 0) {
		fprintf(stderr, "Failed - tmpgm_fork_vmmap(): next=%#x\n", 
				(unsigned int)vmmap[1]->next);
		errcnt++;
	}

	p = (char*)vmmap[1]->phs_addr;
	for (i=0; i< (4096 * 2) / 16; i++) {
		if (memcmp(p, "0123456789ABCDEF", 16)) {
			fprintf(stderr, "Failed - tmpgm_fork_vmmap(): different addr=%#x\n", 
					(unsigned int)p);
			errcnt++;
			break;
		}
		p += 16;
	}
	return errcnt;
}

int test_tmpgm_clone_appelf_pdt() {
	int errcnt = 0, i;
	TRMPG_VM_MAP_ITEM vmmap_tbl[5];
	TRMPG_VM_INF src_vminf;
	TRMPG_VM_INF* dst_vminf;
	TRMPG_VM_MAP_ITEM* src_vmmap;
	TRMPG_VM_MAP_ITEM* dst_vmmap;
	
	int* p;

	for (i=0; i<TRMPG_SZ_VMMAP_HD; i++) {
		src_vminf.vmmap_hd[i].next = &vmmap_tbl[i];
	}
	
	
	vmmap_tbl[TRMPG_VMMAP_CODE].phs_addr = (unsigned int)malloc(4096);
	vmmap_tbl[TRMPG_VMMAP_CODE].vir_addr = 0x401000;
	vmmap_tbl[TRMPG_VMMAP_CODE].ref_count = 1;
	vmmap_tbl[TRMPG_VMMAP_CODE].sz_page = 1;
	vmmap_tbl[TRMPG_VMMAP_CODE].next = 0;
	memset((char*)vmmap_tbl[TRMPG_VMMAP_CODE].phs_addr, 
			'a', 4096);
	
	vmmap_tbl[TRMPG_VMMAP_DATA].phs_addr = (unsigned int)malloc(4096 * 2);
	vmmap_tbl[TRMPG_VMMAP_DATA].vir_addr = 0x403000;
	vmmap_tbl[TRMPG_VMMAP_DATA].ref_count = 1;
	vmmap_tbl[TRMPG_VMMAP_DATA].sz_page = 2;
	vmmap_tbl[TRMPG_VMMAP_DATA].next = 0;
	memset((char*)vmmap_tbl[TRMPG_VMMAP_DATA].phs_addr, 
			'b', 4096 * 2);

	vmmap_tbl[TRMPG_VMMAP_KNLSTK].phs_addr = (unsigned int)malloc(4096 * 3);
	vmmap_tbl[TRMPG_VMMAP_KNLSTK].vir_addr = 0x4a0000;
	vmmap_tbl[TRMPG_VMMAP_KNLSTK].ref_count = 1;
	vmmap_tbl[TRMPG_VMMAP_KNLSTK].sz_page = 3;
	vmmap_tbl[TRMPG_VMMAP_KNLSTK].next = 0;
	memset((char*)vmmap_tbl[TRMPG_VMMAP_DATA].phs_addr, 
			'c', 4096 * 3);
	
	vmmap_tbl[TRMPG_VMMAP_USRSTK].phs_addr = (unsigned int)malloc(4096 * 4);
	vmmap_tbl[TRMPG_VMMAP_USRSTK].vir_addr = 0x480000;
	vmmap_tbl[TRMPG_VMMAP_USRSTK].ref_count = 1;
	vmmap_tbl[TRMPG_VMMAP_USRSTK].sz_page = 4;
	vmmap_tbl[TRMPG_VMMAP_USRSTK].next = 0;
	memset((char*)vmmap_tbl[TRMPG_VMMAP_DATA].phs_addr, 
			'd', 4096 * 4);
	
	dst_vminf = tmpgm_clone_appelf_pdt(&src_vminf);
	
	for (i=0; i<TRMPG_SZ_VMMAP_HD; i++) {
		src_vmmap = src_vminf.vmmap_hd[i].next;
		dst_vmmap = dst_vminf->vmmap_hd[i].next;
		
		if (memcmp((char*)src_vmmap->phs_addr, (char*)dst_vmmap->phs_addr, 
				4096 * src_vmmap->sz_page)) {
			fprintf(stderr, "Failed - tmpgm_clone_appelf_pdt(): vmmap[%d] data.\n", i);
			errcnt++;
		}
	}
	
	p = (int*)dst_vminf->pt_addr;
	
	p += 0x80;
	
	if (*p != dst_vminf->vmmap_hd[TRMPG_VMMAP_USRSTK].next->phs_addr + 7) {
		fprintf(stderr, "Failed - tmpgm_clone_appelf_pdt(): pt data of USRSTK is incorrect. *p=%#x\n", *p);
		errcnt++;
	}
	
	if (2 != dst_vminf->vmmap_hd[TRMPG_VMMAP_CODE].next->ref_count) {
		fprintf(stderr, "Failed - tmpgm_clone_appelf_pdt(): rec_count of CODE is incorrect. ref_count=%d\n", 
				dst_vminf->vmmap_hd[TRMPG_VMMAP_CODE].next->ref_count);
		errcnt++;
	}

	return errcnt;
}

int test_tmpgm_brk() {
	int errcnt = 0;
	TRMPG_VM_INF vminf;
	TRMPG_VM_MAP_ITEM vmmap_data;
	TRMPG_VM_MAP_ITEM* new_vmmap, *new_vmmap2;
	int sz_vmmap_free1, sz_vmmap_free2;
	int* p_int;
	int rc;
	
	p_int = malloc(4096);
	memset(p_int, 0, 4096);
	
	vminf.pt_addr = (unsigned int)p_int;
	vminf.vmmap_hd[TRMPG_VMMAP_DATA].next = &vmmap_data;
	vmmap_data.next = 0;
	vmmap_data.phs_addr = 0;
	vmmap_data.sz_page = 1;
	vmmap_data.vir_addr = 0x402000;
	
	rc = tmpgm_brk(&vminf, 0x404200);
	
	if (!vmmap_data.next) {
		fprintf(stderr, "Failed - tmpgm_brk(): vmmap_data.next=0\n");
		errcnt++;
	}
	new_vmmap = vmmap_data.next;
	if (new_vmmap->ref_count != 1) {
		fprintf(stderr, "Failed - tmpgm_brk(): new_vmmap->ref_count=%d\n",
				new_vmmap->ref_count);
		errcnt++;
	}
	if (new_vmmap->sz_page != 2) {
		fprintf(stderr, "Failed - tmpgm_brk(): new_vmmap->sz_page=%d\n",
				new_vmmap->sz_page);
		errcnt++;
	}
	if (new_vmmap->next != 0) {
		fprintf(stderr, "Failed - tmpgm_brk(): new_vmmap->next=%#x\n",
				(int)new_vmmap->next);
		errcnt++;
	}
	if (new_vmmap->vir_addr != 0x403000) {
		fprintf(stderr, "Failed - tmpgm_brk(): new_vmmap->vir_addr=%#x\n",
				new_vmmap->vir_addr);
		errcnt++;
	}
	if (new_vmmap->phs_addr + 7 != p_int[3]) {
		fprintf(stderr, "Failed - tmpgm_brk(): new_vmmap->phs_addr=%#x, p_int[3]=%#x\n",
				(unsigned int)new_vmmap->phs_addr, p_int[3]);
		errcnt++;
	}

	rc = tmpgm_brk(&vminf, 0x404400);
	if (new_vmmap->next != 0) {
		fprintf(stderr, "Failed - tmpgm_brk() (2nd): new_vmmap->next=%#x\n",
				(unsigned int)new_vmmap->next);
		errcnt++;
	}
	
	rc = tmpgm_brk(&vminf, 0x405100);
	if (new_vmmap->next == 0) {
		fprintf(stderr, "Failed - tmpgm_brk() (2nd): new_vmmap->next=%#x\n",
				(unsigned int)new_vmmap->next);
		errcnt++;
	}
	new_vmmap2 = new_vmmap->next;
	if (new_vmmap2->vir_addr != 0x405000) {
		fprintf(stderr, "Failed - tmpgm_brk() (2nd): new_vmmap2->vir_addr=%#x\n",
				(unsigned int)new_vmmap2->vir_addr);
		errcnt++;
	}
	sz_vmmap_free1 = trmpg_free_vmmap_size();
	
	tmpgm_free_vmmap_list(&vmmap_data);
	
	sz_vmmap_free2 = trmpg_free_vmmap_size();
	
	if (sz_vmmap_free1 + 2 != sz_vmmap_free2) {
		fprintf(stderr, "Failed - tmpgm_free_vmmap_list(): sz_vmmap_free1=%d, sz_vmmap_free2=%d\n",
				sz_vmmap_free1, sz_vmmap_free2);
		errcnt++;
	}

	rc = tmpgm_brk(&vminf, 0x404200);
	new_vmmap = vmmap_data.next;
	rc = tmpgm_brk(&vminf, 0x405100);
	new_vmmap2 = new_vmmap->next;

	rc = tmpgm_brk(&vminf, 0x404800);
	if (new_vmmap->next) {
		fprintf(stderr, "Failed - tmpgm_brk() (decl): new_vmmap->next!=0\n");
		errcnt++;
	}
	rc = tmpgm_brk(&vminf, 0x403200);
	if (!vmmap_data.next) {
		fprintf(stderr, "Failed - tmpgm_brk() (decl 2nd): vmmap_data.next==0\n");
		errcnt++;
	}
	rc = tmpgm_brk(&vminf, 0x4029f0);
	if (vmmap_data.next) {
		fprintf(stderr, "Failed - tmpgm_brk() (decl 3rd): vmmap_data.next!=0\n");
		errcnt++;
	}
	
	free((void*)vminf.pt_addr);
	
	return errcnt;
}


