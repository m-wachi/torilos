#ifndef TRTASK_H
#define TRTASK_H

#include "trmempage.h"
#include "trfile.h"

#define TRTSK_MAXTASKS    1000
#define TRTSK_GDT0        5

#define TRTSK_STS_DEAD		0
#define TRTSK_STS_RUNNING	1
#define TRTSK_STS_SLEEP		2
#define TRTSK_STS_WAITKBD	3
#define TRTSK_STS_ZONBIE	4
#define TRTSK_STS_WAITPID	5
#define TRTSK_STS_WAITLOCK	6

typedef struct tss32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
} TSS32;

/**
 * @brief represented task
 */
typedef struct tsk_task {
	int process_id;	//!< Process ID
	int parent_pid;	//!< Parent Process ID	
	int sel;	//!< TSS segment selector
	int status;
	TSS32 tss;
	int esp0_top;				//!< used by kerlnel thread only
	int kthr_stkarea_addr;	//!< used by kerlnel thread only
	TRMPG_VM_INF* vminf;
	TFL_HND_FILE* fdtbl[16];
	int signal_flag;	//!< received signal holder
	/** @brief current-working-directory full-path-name */
	char cwd[128];
	int exit_status;		//!< parameter value of exit() 
} TRTSK_TASK;

typedef struct tsk_queentry {
	TRTSK_TASK task;
	struct tsk_queentry *prev;
	struct tsk_queentry *next;
	unsigned long timeout;
} TRTSK_QUEENTRY;

void trtsk_que_head_init(TRTSK_QUEENTRY* que_hd);
void trtsk_add_to_que(TRTSK_QUEENTRY* que_hd, TRTSK_QUEENTRY* tskent);
int trtsk_count_tasks(TRTSK_QUEENTRY* que_hd);
void trtsk_remove_from_que(TRTSK_QUEENTRY* tskent);
TRTSK_QUEENTRY* trtsk_lookup_task_in_que(int pid, TRTSK_QUEENTRY* que);
int trtsk_is_empty_que(TRTSK_QUEENTRY* que_hd);
int trtsk_is_one_entry_que(TRTSK_QUEENTRY* que_hd);
void trtsk_join_ques(TRTSK_QUEENTRY* que1_hd, TRTSK_QUEENTRY* que2_hd);
void trtsk_dump_tasks(TRTSK_QUEENTRY* que_hd, char* head_msg);

#endif

