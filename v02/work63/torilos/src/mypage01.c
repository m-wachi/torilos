/**
 * @file mypage01.c
 * @brief Virtual Memory utilities module.
 * 
 * These function are used for PDT/PT operation mainly.
 */

#include <torilos/mypage01.h>
#include <torilos/trmempage.h>

#include <torilos/elf.h>
#include <torilos/debug01.h>
#include <torilos/trutil.h>
#include <torilos/myasmfunc01.h>
#include <torilos/mylib01.h>
#include <torilos/trmempage_mdl.h>

void pag_setup_paging() {

	unsigned int pde;
	unsigned int* pdt;
	unsigned int pte;
	unsigned int* pt;

	int i;

	int low3bit;
	low3bit = 0x03; //U/S off

	pt = (unsigned int*) TRMPG_FIRST_PT_ADDR;

	/* 1st pte 0-4M */
	pte = low3bit;
	for(i=0; i<1024; i++){
		*pt = pte;
		pt++;
		pte += 0x1000;
	}

	/* 2nd pte 4-8M */
	pte = 0x400000 + low3bit;
	pt = (unsigned int*) 0x282000;
	for(i=0; i<1024; i++){
		*pt = pte;
		pt++;
		pte += 0x1000;
	}

	// enable VESA VRAM address (0xe0000000)
	pte = 0xe0000000 + low3bit;
	pt = (unsigned int*) TRMPG_VRAM_PT_ADDR;
	// enable first 75 entries.
	for(i=0; i<75; i++){
		*pt = pte;
		pt++;
		pte += 0x1000;
	}

	for(i=0; i<1024-75; i++){
		*pt++ = 0;
	}

	// first 2 entries
	pdt = (unsigned int*) 0x280000;
	pde = TRMPG_FIRST_PT_ADDR + low3bit;
	*pdt = pde;
	pdt++;
	pde = 0x282000 + low3bit;
	*pdt = pde;
	pdt++;

	//others initialize 0
	for(i=2; i<1024; i++) {
		*pdt = 0;
		pdt++;
	}

	// VESA VRAM PDTE set
	pdt = (unsigned int*) 0x280e00;
	pde = TRMPG_VRAM_PT_ADDR + low3bit;
	*pdt = pde;

	load_cr3(PROCESS0_PDT);

}

