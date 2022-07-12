#ifndef MYMM01_H
#define MYMM01_H

#define PG_MNG_TBL_MAX     128
#define PG_MNG_DTL_TBL_MAX 128
#define MM_PGSIZE      4096

/* PG_MNG_ENTRY.sts */
#define MM_FREE 0   /* free */
#define MM_P_USED 1	/* partly used */
#define MM_F_USED 2 /* fully used */
#define MM_UNUSE -1 /* this entry isn't used */

/* PG_MNG_DTL_ENTRY.sts */
#define MM_DTL_FREE 0   /* free */
#define MM_DTL_USED 1	/* partly used */
#define MM_DTL_UNUSE -1 /* this entry isn't used */

#define MM_MALLOC_KNL_ST_ADDR 0x00000000
#define MM_MALLOC_KNL_ED_ADDR 0x00400000
#define MM_MALLOC_APP_ST_ADDR 0x00400000
#define MM_MALLOC_APP_ED_ADDR 0xffffffff

#define E820_ENTRY_ADDR 0x00040010
#define E820_CNT_ADDR   0x00040000

typedef struct e820entry {
	unsigned long long addr;    /* start of memory segment */
	unsigned long long size;    /* size of memory segment */
	unsigned long type;		    /* type of memory segment */
} E820ENTRY;

typedef struct pg_mng_entry {
	unsigned int addr;
	int pages;
	int sts;
	int dtl;
	int next;
} PG_MNG_ENTRY;

typedef struct pg_mng_dtl_entry {
	unsigned int addr;
	int size;
	int sts;
	int dtl;
	int next;
} PG_MNG_DTL_ENTRY;

typedef struct pg_mngr {
	PG_MNG_ENTRY pg_mng_tbl[PG_MNG_TBL_MAX];
	int cur_size;
	PG_MNG_DTL_ENTRY pg_mng_dtl_tbl[PG_MNG_DTL_TBL_MAX];
	int cur_dtl_size;
} PG_MNGR;

void mm_pg_init(int size);
int mm_init_alloc(unsigned int addr, int pages);

unsigned int mm_malloc_pg2(int size, unsigned int from_addr, 
						   unsigned int end_addr);

unsigned int mm_malloc_pg_knl(int size);
unsigned int mm_malloc_pg_app(int size);

int mm_separate_with_addr(int idx, unsigned int start_addr, int pages);
int mm_separate_entry(int idx, int pages);

int mm_lookup_next_entry();

void mm_free(unsigned int addr);

void mm_setup();
void mm_sprint_entry(PG_MNG_ENTRY* pg_mng_entry, char* buff);

void mm_print_pg_mngr();

PG_MNGR* mm_get_pg_mngr();

#endif
