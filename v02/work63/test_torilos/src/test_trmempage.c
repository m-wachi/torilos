#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <torilos/trmempage.h>
#include <torilos/elf.h>
#include <torilos/debug01.h>

int test_vmmap();
int test_vminf();
int test_pdtpt();
int test_push_strings();
int test_trmpg_push_strptrs();
int test_trmpg_prep_argenvs();

int main(int argc, char** argv) {
	int errcnt = 0;
	trmpg_init();
	
	/*
	 * vmmap test
	 */
	errcnt += test_vmmap();
	
	/*
	 * vminf test
	 */
	errcnt += test_vminf();
	
	/*
	 * pdt, pt test
	 */
	errcnt += test_pdtpt();
	
	/*
	 * push_strings test
	 */
	errcnt += test_push_strings();
	
	/*
	 * push_strptrs test 
	 */
	errcnt += test_trmpg_push_strptrs();
	
	
	/*
	 * prep_argenvs test 
	 */
	errcnt += test_trmpg_prep_argenvs();
	
	if (errcnt)
		return -1;
		
	puts("OK - test passed.");
	return 0;
}

int test_vmmap() {
	int sz_free, errcnt = 0, i;
	TRMPG_VM_MAP_ITEM* vmmap[3];
	Elf32_Phdr phdr;

	sz_free = trmpg_free_vmmap_size();
	if (TRMPG_MAX_VMMAP_TBL != sz_free) {
		fprintf(stderr, "Failed - trmpg_init(): free size=%d\n", sz_free);
		errcnt++;
	}
	
	vmmap[0] = trmpg_alloc_vmmap_item();
	
	vmmap[1] = trmpg_alloc_vmmap_item();
	
	sz_free = trmpg_free_vmmap_size();
	if (TRMPG_MAX_VMMAP_TBL - 2 != sz_free) {
		fprintf(stderr, "Failed - trmpg_alloc_vmmap_item(): free size=%d\n", sz_free);
		errcnt++;
	}
	if (vmmap[0]->next) {
		fprintf(stderr, "Failed - trmpg_alloc_vmmap_item(): alloc vmmap->next has a value(%#x).\n",
				(unsigned int)vmmap[0]->next);
		errcnt++;
	}

	trmpg_free_vmmap_item(vmmap[0]);
	sz_free = trmpg_free_vmmap_size();
	if (TRMPG_MAX_VMMAP_TBL - 1 != sz_free) {
		fprintf(stderr, "Failed - trmpg_free_vmmap_item(): free size=%d\n", sz_free);
		errcnt++;
	}

	trmpg_free_vmmap_item(vmmap[1]);
	sz_free = trmpg_free_vmmap_size();
	if (TRMPG_MAX_VMMAP_TBL != sz_free) {
		fprintf(stderr, "Failed - trmpg_free_vmmap_item(): free size=%d\n", sz_free);
		errcnt++;
	}
	
	for (i=0; i<3; i++) {
		vmmap[i] = trmpg_alloc_vmmap_item();
	}
	for (i=0; i<2; i++) {
		vmmap[i]->next = vmmap[i+1];
	}
	sz_free = trmpg_free_vmmap_size();

	/*
	trmpg_free_vmmap_list(vmmap[0]);
	
	sz_free = trmpg_free_vmmap_size();
	if (TRMPG_MAX_VMMAP_TBL != sz_free) {
		fprintf(stderr, "Failed - trmpg_free_vmmap_list(): free size=%d\n", sz_free);
		errcnt++;
	}
	*/
	
	//phdr.p_filesz = 0x719;
	phdr.p_filesz = 0x700;
	phdr.p_memsz = 0x719;
	//phdr.p_filesz = 0x34;
	phdr.p_offset = 0x18e8;
	phdr.p_vaddr = 0x4028e8;
	//phs_addr = 0x420000;
	
	vmmap[0] = trmpg_alloc_vmmap_by_phdr(&phdr);
	/* if (vmmap[0]->phs_addr != phs_addr) {
		fprintf(stderr, "Failed - trmpg_alloc_vmmap_by_phdr(): phs_addr=%#x\n", 
				vmmap[0]->phs_addr);
		errcnt++;
	}*/
	if (vmmap[0]->vir_addr != 0x402000) {
		fprintf(stderr, "Failed - trmpg_alloc_vmmap_by_phdr(): vir_addr=%#x\n", 
				vmmap[0]->vir_addr);
		errcnt++;
	}
	if (vmmap[0]->sz_page != 2) {
		fprintf(stderr, "Failed - trmpg_alloc_vmmap_by_phdr(): sz_page=%#x\n", 
				vmmap[0]->sz_page);
		errcnt++;
	}
	if (vmmap[0]->ref_count != 1) {
		fprintf(stderr, "Failed - trmpg_alloc_vmmap_by_phdr(): ref_count=%#x\n", 
				vmmap[0]->ref_count);
		errcnt++;
	}
	if (vmmap[0]->next != 0) {
		fprintf(stderr, "Failed - trmpg_alloc_vmmap_by_phdr(): next=%#x\n", 
				(unsigned int)vmmap[0]->next);
		errcnt++;
	}
	return errcnt;
}

int test_vminf() {
	int sz_free, errcnt = 0;
	TRMPG_VM_INF* vminf[3];
	unsigned int pdt_addr[3];

	sz_free = trmpg_free_vminf_size();
	
	pdt_addr[0] = 0x1000;
	vminf[0] = trmpg_alloc_vminf(pdt_addr[0]);
	
	if (vminf[0]->pdt_addr != pdt_addr[0]) {
		fprintf(stderr, "Failed - trmpg_alloc_vminf(): vminf[0]->pdt_addr=%#x\n", 
				vminf[0]->pdt_addr);
		errcnt++;
	}
	
	pdt_addr[1] = 0x2000;
	vminf[1] = trmpg_alloc_vminf(pdt_addr[1]);
	
	if (vminf[1]->pdt_addr != pdt_addr[1]) {
		fprintf(stderr, "Failed - trmpg_alloc_vminf(): vminf[1]->pdt_addr=%#x\n", 
				vminf[1]->pdt_addr);
		errcnt++;
	}

	sz_free = trmpg_free_vminf_size();
	if (sz_free != TRMPG_MAX_VM_INF_TBL -2) {
		fprintf(stderr, "Failed - trmpg_free_vminf_size(): sz_free=%d\n", 
				sz_free);
		errcnt++;
	}
	
	trmpg_free_vminf(vminf[0]);
	sz_free = trmpg_free_vminf_size();
	if (sz_free != TRMPG_MAX_VM_INF_TBL - 1) {
		fprintf(stderr, "Failed - trmpg_free_vminf(): sz_free=%d\n", 
				sz_free);
		errcnt++;
	}
	return errcnt;
}

int test_pdtpt() {
	int i, errcnt = 0;
	TRMPG_VM_MAP_ITEM vmmap;
	unsigned int pdt_addr, pt_addr;
	int* p;

	p = malloc(4096);

	pdt_addr = (unsigned int)p;
	
	trmpg_init_app_pdt(pdt_addr);
	for(i=0; i<1024; i++) {
		if (i == 0 ) {
			if (*p != 0x281003) {
				fprintf(stderr, "Failed - trmpg_init_app_pdt(): i=%d, *p=%#x\n", 
						i, *p);
				errcnt++;
				break;
			}
		}
		else if (i == 0x380) {
			if (*p != 0x283003) {
				fprintf(stderr, "Failed - trmpg_init_app_pdt(): i=%#x, *p=%#x\n", 
						i, *p);
				errcnt++;
				break;
			}
		} else {
			if (*p) {
				fprintf(stderr, "Failed - trmpg_init_app_pdt(): i=%#x, *p=%#x\n", 
						i, *p);
				errcnt++;
				break;
			}
		}
		p++;
	}
	
	free((int*)pdt_addr);
	
	p = malloc(4096);
	
	pt_addr = (unsigned int)p;
	
	trmpg_init_pt(pt_addr);
	
	for(i=0; i<1024; i++) {
		if (*p) {
			fprintf(stderr, "Failed - trmpg_init_pt(): i=%d, *p=%#x\n", 
					i, *p);
			errcnt++;
			break;
		}
		p++;
	}
	free((int*)pt_addr);
	
	p = malloc(4096);
	memset(p, 0, 4096);
	pdt_addr = (unsigned int)p;
	trmpg_set_app_pde(pdt_addr, 0, 0x1000);
	
	if (*p != 0x1007) {
		fprintf(stderr, "Failed - trmpg_set_app_pde(): *p=%#x\n", *p);
		errcnt++;
	}
	
	trmpg_set_app_pde(pdt_addr, 5, 0x22000);
	p += 5;
	if (*p != 0x22007) {
		fprintf(stderr, "Failed - trmpg_set_app_pde(): *p=%#x\n", *p);
		errcnt++;
	}
	p = (int*)pdt_addr;
	free(p);
	
	pt_addr = (unsigned int)malloc(4096);
	p = (int*)pt_addr;
	memset(p, 0, 4096);
	
	vmmap.phs_addr = 0x402000;
	vmmap.vir_addr = 0x480000;
	vmmap.sz_page = 3;
	trmpg_set_pt_vmmap(&vmmap, pt_addr);
	
	p += 0x80;
	for (i=0; i<3; i++) {
		if (*p != (0x402007 + 0x1000 * i)) {
			fprintf(stderr, "Failed - trmpg_set_pt_vmmap(): i=%#x, *p=%#x\n", i, *p);
			errcnt++;
		}
		p++;
	}
	
	free((int*)pt_addr);
	
	return errcnt;
}

int test_push_strings() {
	int i, errcnt=0;
	char* area_addr;
	unsigned int bottom_addr;
	int argoffset[16];
	static char* argv[] = {"abc", "defghi", "jklm"};
	char test_str[] = "Hello!";
	unsigned int rc;
	char* p;
	
	area_addr = malloc(64);
	bottom_addr = (unsigned int)(area_addr + 64);
	
	rc = trmpg_push_one_string(bottom_addr, test_str);
	if (rc != 7) {
		fprintf(stderr, "Failed - trmpg_push_one_string(): rc=%d\n", rc);
		errcnt++;
	}
	p = (char*)(bottom_addr - rc);
	if (strcmp(p, test_str)) {
		fprintf(stderr, "Failed - trmpg_push_one_string(): p=%s\n", p);
		errcnt++;
	}
	
	rc = trmpg_push_strings(argoffset, bottom_addr, 3, argv);
	
	if (rc != 16) {
		fprintf(stderr, "Failed - trmpg_push_strings(): rc=%d\n", rc);
		errcnt++;
	}
	if (argoffset[0] != 16) {
		fprintf(stderr, "Failed - trmpg_push_strings(): argoffset[0]=%d\n", argoffset[0]);
		errcnt++;
	}
	if (argoffset[1] != 12) {
		fprintf(stderr, "Failed - trmpg_push_strings(): argoffset[1]=%d\n", argoffset[1]);
		errcnt++;
	}
	if (argoffset[2] != 5) {
		fprintf(stderr, "Failed - trmpg_push_strings(): argoffset[2]=%d\n", argoffset[2]);
		errcnt++;
	}
	
	for (i=0; i<3; i++) {
		if (strcmp(argv[i], (char*)(bottom_addr - argoffset[i]))) {
			fprintf(stderr, "Failed - trmpg_push_strings(): argv[%d]=%s\n", i, argv[i]);
			errcnt++;
		}
	}
	free(area_addr);
	
	return errcnt;
}

int test_trmpg_push_strptrs() {
	int errcnt = 0;
	int rc;
	int argoffset[] = {14, 8, 4};
	unsigned int phs_bottom_addr;
	unsigned int* p_int;
	char* area_addr;
	area_addr = malloc(32);
	phs_bottom_addr = (unsigned int)(area_addr + 32);
	
	rc = trmpg_push_strptrs(phs_bottom_addr, 0x48000, 3, argoffset);
	
	if (rc != 16) {
		fprintf(stderr, "Failed - trmpg_push_strptrs(): rc=%d\n", rc);
		errcnt++;
	}
	p_int = (unsigned int*)(phs_bottom_addr - rc);
	if (*p_int != 0x48000 - 14) {
		fprintf(stderr, "Failed - trmpg_push_strptrs(): *p_int=%#x\n", *p_int);
		errcnt++;
	}
	p_int++;
	if (*p_int != 0x48000 - 8) {
		fprintf(stderr, "Failed - trmpg_push_strptrs(): *p_int=%#x\n", *p_int);
		errcnt++;
	}
	p_int++;
	if (*p_int != 0x48000 - 4) {
		fprintf(stderr, "Failed - trmpg_push_strptrs(): *p_int=%#x\n", *p_int);
		errcnt++;
	}
	
	free(area_addr);

	return errcnt;
}


int test_trmpg_prep_argenvs() {
	int errcnt=0;
	char* area_addr;
	unsigned int bottom_addr, rc;
	char* argv[] = {"abc", "defghi", "jklm"};
	char* envv[] = {"op=qr", "stu=vwx"};
	unsigned int pv_diff;
	unsigned int act_argvp, act_envvp;
	unsigned int *act_argv, *act_envv;
	unsigned int *p_int1;
	int act_argc;
	char *act_argv1, *act_envv1;
	
	area_addr = malloc(96);
	bottom_addr = (unsigned int)(area_addr + 96);
	pv_diff = bottom_addr - 0x48000;
	rc = trmpg_prep_argenvs(bottom_addr, 0x48000, 3, argv, 2, envv);
	
	p_int1 = (unsigned int*)(rc + pv_diff);
	act_argc = *p_int1;
	
	if (act_argc != 3) {
		fprintf(stderr, "Failed - trmpg_prep_argenvs(): act_argc=%d\n", act_argc);
		errcnt++;
	}
	
	act_argvp = *++p_int1;
	if (act_argvp != 0x47fc4) {
		fprintf(stderr, "Failed - trmpg_prep_argenvs(): act_argvp=%#x\n", act_argvp);
		errcnt++;
	}
	
	act_envvp = *++p_int1;
	if (act_envvp != 0x47fe4) {
		fprintf(stderr, "Failed - trmpg_prep_argenvs(): act_envvp=%#x\n", act_envvp);
		errcnt++;
	}

	act_argv = (unsigned int*)(act_argvp + pv_diff);
	if (*act_argv != 0x47fd4) {
		fprintf(stderr, "Failed - trmpg_prep_argenvs(): *act_argv=%#x\n", *act_argv);
		errcnt++;
	}

	act_envv = (unsigned int*)(act_envvp + pv_diff);
	if (*act_envv != 0x47ff2) {
		fprintf(stderr, "Failed - trmpg_prep_argenvs(): *act_envv=%#x\n", *act_envv);
		errcnt++;
	}

	
	act_argv1 = (char*)(act_argv[1] + pv_diff);
	if (strcmp("defghi", act_argv1)) {
		fprintf(stderr, "Failed - trmpg_prep_argenvs(): act_argv1=%s\n", act_argv1);
		errcnt++;
	}
	
	act_envv1 = (char*)(act_envv[1] + pv_diff);
	if (strcmp("stu=vwx", act_envv1)) {
		fprintf(stderr, "Failed - trmpg_prep_argenvs(): act_envv1=%s\n", act_envv1);
		errcnt++;
	}

	free(area_addr);

	return errcnt;
}

