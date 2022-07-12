/**
 * @file mymm01.c
 * @brief Memory management module.
 * 
 * Memory management functions are defined.
 * Mainly Phisical memory management.
 */ 

#include <torilos/mymm01.h>
#include <torilos/debug01.h>
#include <torilos/mylib01.h>
#include <torilos/trutil.h>

static PG_MNGR pg_mngr;	///< page manager


/**
 * @brief initialize pg_mngr
 * @param size memory size(MB)
 */
void mm_pg_init(int size) {
	int pg_size;

	pg_size = size * 1024 * 1024 / MM_PGSIZE; 

	pg_mngr.pg_mng_tbl[0].addr = 0x0;
	pg_mngr.pg_mng_tbl[0].pages = pg_size;
	pg_mngr.pg_mng_tbl[0].sts = MM_FREE;
	pg_mngr.pg_mng_tbl[0].dtl = -1;
	pg_mngr.pg_mng_tbl[0].next = 1;
	pg_mngr.cur_size = 2;
	pg_mngr.cur_dtl_size = 0;
	pg_mngr.pg_mng_tbl[1].addr = 0x0;
	pg_mngr.pg_mng_tbl[1].next = -1;

}
/**
 * @brief allocate a page(4kb) with address
 * 
 * use this function to disable specified address
 * (ex. 0x0a0000 memory hole.)
 *  
 * @param addr    address
 * @param pages    size(kb)
 * @return !=-1: address, -1: error
 */
int mm_init_alloc(unsigned int addr, int pages) {
	unsigned int last_addr = 0, next_addr, n2_addr;
	int i, last_idx=-1, next_pages, n2_pages, last_pages;
	int next_entry, n2_entry;
	PG_MNG_ENTRY* pg_mng_tbl;

	/* 4k unit addr error check */
	if (addr % MM_PGSIZE != 0) {
		return -1;
	}

	pg_mng_tbl = pg_mngr.pg_mng_tbl;
	i = 0;
	while (pg_mng_tbl[i].next != -1) {
		if (pg_mng_tbl[i].addr <= addr) {
			last_addr = pg_mng_tbl[i].addr;
			last_idx = i;
		} else {
			break;
		}			
		i = pg_mng_tbl[i].next;
	}

	if (last_idx == -1) 
		return -2;   /* failed to look up */

	if (pg_mng_tbl[last_idx].sts != MM_FREE)
		return -3;	/* already used */


	if (last_addr == addr) {
		last_pages = pages;
		next_addr = addr + pages * MM_PGSIZE;
		next_pages = pg_mng_tbl[last_idx].pages - pages;

		next_entry = mm_lookup_next_entry(pg_mngr);

		pg_mng_tbl[next_entry].addr = next_addr;
		pg_mng_tbl[next_entry].pages = next_pages;
		pg_mng_tbl[next_entry].sts = MM_FREE;
		pg_mng_tbl[next_entry].dtl = -1;
		pg_mng_tbl[next_entry].next = pg_mng_tbl[last_idx].next;

		pg_mng_tbl[last_idx].pages = last_pages;
		pg_mng_tbl[last_idx].sts = MM_F_USED;
		pg_mng_tbl[last_idx].dtl = -1;
		pg_mng_tbl[last_idx].next = next_entry;
		
	}
	else {
		unsigned int maxaddr = pg_mng_tbl[last_idx].addr + 
			pg_mng_tbl[last_idx].pages * MM_PGSIZE;

		if (maxaddr < addr) return -4; /* over installed phisical addrss */


		last_pages = (addr - pg_mng_tbl[last_idx].addr) / MM_PGSIZE;
		next_addr = addr;
		next_pages = pages;
		n2_addr = addr + pages * MM_PGSIZE;
		n2_pages = pg_mng_tbl[last_idx].pages - last_pages - pages;

		next_entry = mm_lookup_next_entry(pg_mngr);

		if (n2_pages > 0) {
			n2_entry = mm_lookup_next_entry(pg_mngr);
			pg_mng_tbl[n2_entry].addr = n2_addr;
			pg_mng_tbl[n2_entry].pages = n2_pages;
			pg_mng_tbl[n2_entry].sts = MM_FREE;
			pg_mng_tbl[n2_entry].dtl = -1;
			pg_mng_tbl[n2_entry].next = pg_mng_tbl[last_idx].next;
		}
		else
			n2_entry = pg_mng_tbl[last_idx].next;

		pg_mng_tbl[next_entry].addr = next_addr;
		pg_mng_tbl[next_entry].pages = next_pages;
		pg_mng_tbl[next_entry].sts = MM_F_USED;
		pg_mng_tbl[next_entry].dtl = -1;
		pg_mng_tbl[next_entry].next = n2_entry;
		
		pg_mng_tbl[last_idx].pages = last_pages;
		pg_mng_tbl[last_idx].sts = MM_FREE;
		pg_mng_tbl[last_idx].dtl = -1;
		pg_mng_tbl[last_idx].next = next_entry;
		
	}

	return 0;
}

/**
 * @brief alloc memory in limited area
 * @param size       required size(byte)
 * @param start_addr limited area start address
 * @param end_addr   limited area end address
 * @return     <0: error, >0: allocated address
 */
unsigned int mm_malloc_pg2(int size, unsigned int start_addr, 
						   unsigned int end_addr) {

	unsigned int alc_addr, ent_end_addr, alc_end_addr;
	int i, pages;
	int idx_alc;
	PG_MNG_ENTRY* pg_mng_tbl;
	unsigned int try_alc_addr;

	pages = size / MM_PGSIZE;
	if (0 < size % MM_PGSIZE) pages++;

	pg_mng_tbl = pg_mngr.pg_mng_tbl;
	i = 0;
	alc_addr = -1;

	try_alc_addr = start_addr;

	alc_end_addr = try_alc_addr + pages * MM_PGSIZE;


	while(pg_mng_tbl[i].next != -1) {

		if (pg_mng_tbl[i].sts == MM_FREE) {
			if (try_alc_addr < pg_mng_tbl[i].addr) {
				try_alc_addr = pg_mng_tbl[i].addr;
				alc_end_addr = try_alc_addr + pages * MM_PGSIZE;
			}

			// check address over
			if (alc_end_addr > end_addr)
				return -2;


			ent_end_addr = pg_mng_tbl[i].addr 
				+ pg_mng_tbl[i].pages * MM_PGSIZE;
			
			if (pg_mng_tbl[i].addr <= try_alc_addr && 
				alc_end_addr <= ent_end_addr) {

				// separate entry
				if(0 > (idx_alc 
						= mm_separate_with_addr(i, try_alc_addr, pages))) 
					return idx_alc - 10;
					
				pg_mng_tbl[idx_alc].sts = MM_F_USED;
				alc_addr = pg_mng_tbl[idx_alc].addr;
				break;
				
			} 

		}
		i = pg_mng_tbl[i].next;
	}

	return alc_addr;

}

unsigned int mm_malloc_pg_knl(int size) {
	return mm_malloc_pg2(size, MM_MALLOC_KNL_ST_ADDR, MM_MALLOC_KNL_ED_ADDR);
}

unsigned int mm_malloc_pg_app(int size) {
	return mm_malloc_pg2(size, MM_MALLOC_APP_ST_ADDR, MM_MALLOC_APP_ED_ADDR);
}

int mm_separate_with_addr(int idx, unsigned int start_addr, int pages) {

	unsigned int ent_end_addr, alc_end_addr;
	int head_pages, head_size;
	int idx_head, idx_req;

	PG_MNG_ENTRY* pg_mng_tbl;

	pg_mng_tbl = pg_mngr.pg_mng_tbl;

	// check from addr
	if (start_addr < pg_mng_tbl[idx].addr) 
		return -1;

	alc_end_addr = start_addr + pages * MM_PGSIZE;

	ent_end_addr = pg_mng_tbl[idx].addr 
		+ pg_mng_tbl[idx].pages * MM_PGSIZE;

	// check end addr
	if (ent_end_addr < alc_end_addr)
		return -2;

	if (start_addr == pg_mng_tbl[idx].addr) {
		mm_separate_entry(idx, pages);
		return idx;
	}

	head_size = start_addr - pg_mng_tbl[idx].addr;
	head_pages = head_size / MM_PGSIZE;

	// separate lower address space
	if (mm_separate_entry(idx, head_pages))
		return -3;

	idx_head = idx;
	idx_req = pg_mng_tbl[idx_head].next;
	
	if (pages == pg_mng_tbl[idx_req].pages)
		return idx_req;

	// separate higer address space
	if (mm_separate_entry(idx_req, pages))
		return -4;

	return idx_req;

}



int mm_separate_entry(int idx, int pages) {

	PG_MNG_ENTRY* pg_mng_tbl;
	int next_entry;

	pg_mng_tbl = pg_mngr.pg_mng_tbl;

	if (pg_mng_tbl[idx].pages <= pages)
		return -2; // couldn't separate

	next_entry = mm_lookup_next_entry();
	if (-1 == next_entry) 
		return -1;	/* no free entry */

	pg_mng_tbl[next_entry].addr = 
		pg_mng_tbl[idx].addr + pages * MM_PGSIZE;

	pg_mng_tbl[next_entry].pages = pg_mng_tbl[idx].pages - pages;
	pg_mng_tbl[next_entry].sts = pg_mng_tbl[idx].sts;
	pg_mng_tbl[next_entry].dtl = -1;
	pg_mng_tbl[next_entry].next = pg_mng_tbl[idx].next;
	pg_mng_tbl[idx].pages = pages;
	pg_mng_tbl[idx].next = next_entry;

	return 0;
}

void mm_free(unsigned int addr) {
	int i, next, previous;
	PG_MNG_ENTRY* pg_mng_tbl;

	
	pg_mng_tbl = pg_mngr.pg_mng_tbl;
	i = 0; previous = -1;

	while(pg_mng_tbl[i].next != -1) {
		/* not found parameter address */
		if (pg_mng_tbl[i].addr > addr) break;

		if (pg_mng_tbl[i].addr == addr && pg_mng_tbl[i].sts != MM_FREE) {

			pg_mng_tbl[i].sts = MM_FREE;

			/* unify next entry */
			next = pg_mng_tbl[i].next;
			if (pg_mng_tbl[next].sts == MM_FREE) {

				pg_mng_tbl[i].pages += pg_mng_tbl[next].pages;
				pg_mng_tbl[i].next = pg_mng_tbl[next].next;

				pg_mng_tbl[next].addr = 0;
				pg_mng_tbl[next].pages = 0;
				pg_mng_tbl[next].sts = MM_UNUSE;
			} 

			/* unify previous entry */
			if (pg_mng_tbl[previous].sts == MM_FREE && previous != -1) {
				pg_mng_tbl[previous].pages += pg_mng_tbl[i].pages;
				pg_mng_tbl[previous].next = pg_mng_tbl[i].next;

				pg_mng_tbl[i].addr = 0;
				pg_mng_tbl[i].pages = 0;
				pg_mng_tbl[i].sts = MM_UNUSE;
			}

			break;
		}
		previous = i;
		i = pg_mng_tbl[i].next;
	}

}

int mm_lookup_next_entry() {
	int i, next_entry=-1;
	PG_MNG_ENTRY* pg_mng_tbl;

	pg_mng_tbl = pg_mngr.pg_mng_tbl;
	/* look up UNUSE entry */
	for (i=0; i<pg_mngr.cur_size; i++) {
		if (pg_mng_tbl[i].sts == MM_UNUSE) {
			next_entry=i;
			break;
		}
	}

	/* found UNUSE entry */
	if (next_entry > -1)
		return next_entry;


	if (PG_MNG_TBL_MAX <= pg_mngr.cur_size + 1) {
		return -1;			/* error. no free entry */
	}

	/* size++ */
	next_entry = pg_mngr.cur_size++;
	return next_entry;
}

void mm_setup() {
	int i, cnt, max, rc;
	unsigned short *p_e820ent_cnt;
	unsigned int maxsize, addr4k;
	int pages, wk_size;
	E820ENTRY *e820map;
	
	/* e820entry set */
	e820map = (E820ENTRY *) E820_ENTRY_ADDR;

	p_e820ent_cnt = (unsigned short*) E820_CNT_ADDR;
	cnt = *p_e820ent_cnt;

	//search largest usable entry index.
	for (i=0; i<cnt; i++) {
		if (e820map[i].type == 1) {
			max = i;
		}
	}
			
	maxsize = e820map[max].addr + e820map[max].size;
			
	mm_pg_init(maxsize/1024/1024);

	// reserve I/O area
	mm_init_alloc(0x0, 16);

	// reserve memory-holl area
	// 0x000a0000 - 0x00100000
	mm_init_alloc(0xa0000, 96);

	// reserve kernel area
	// 0x00100000 - 0x00284000
	mm_init_alloc(0x100000, 0x184);


	for (i=0; i<cnt; i++) {
		if (e820map[i].type == 2) {
			addr4k = e820map[i].addr & 0xfffff000;

			wk_size = e820map[i].addr - addr4k +
				e820map[i].size;

			pages = wk_size / MM_PGSIZE;
			if (wk_size % MM_PGSIZE) pages++;

			rc = mm_init_alloc(addr4k, pages);
		}
	}

}


void mm_sprint_entry(PG_MNG_ENTRY* pg_mng_entry, char* buff) {
	char buff2[128];

	sprint_varval(buff, "addr", pg_mng_entry->addr, 1);
	
	sprint_varval(buff2, ", pages", pg_mng_entry->pages, 0);
	my_strcat(buff, buff2);
	
	sprint_varval(buff2, ", sts", pg_mng_entry->sts, 0);
	my_strcat(buff, buff2);

	sprint_varval(buff2, ", dtl", pg_mng_entry->dtl, 0);
	my_strcat(buff, buff2);

	sprint_varval(buff2, ", next", pg_mng_entry->next, 0);
	my_strcat(buff, buff2);

}


void mm_print_pg_mngr() {
	int i;
	char buf_head[256], buff[256];


	for(i=0; i<pg_mngr.cur_size; i++) {
		my_itoa(buf_head, i);
		my_strcat(buf_head, ": ");
		mm_sprint_entry(&(pg_mngr.pg_mng_tbl[i]), buff);
		my_strcat(buf_head, buff);
		debug_puts(buf_head);
	}
}

