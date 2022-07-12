/**
 * @file mytask01.c
 * @brief Task Management Module.
 */

#include <torilos/mytask01.h>
#include <torilos/dsctbl01.h>
#include <torilos/debug01.h>
#include <torilos/mymm01.h>
#include <torilos/elf.h>
#include <torilos/myasmfunc01.h>
#include <torilos/trlock.h>
#include <torilos/mylib01.h>
#include <torilos/trutil.h>
#include <torilos/trmempage.h>
#include <torilos/trmempage_mdl.h>
#include <torilos/myutil01.h>
#include <torilos/trpmemmgr_mdl.h>

extern unsigned long g_ts_timeout;
extern unsigned long g_time_counter;

void asm_fork_start_child();

static unsigned int ml_ts_interval;

static TRTSK_QUEENTRY ml_ary_task[TRTSK_MAXTASKS];
static TRTSK_QUEENTRY* ml_cur_task_ent;
static TRTSK_QUEENTRY ml_free_tasks;
static TRTSK_QUEENTRY ml_running_tasks;
static TRTSK_QUEENTRY ml_sleep_tasks;

static TRTSK_QUEENTRY ml_waitkbd_tasks;
static TRTSK_QUEENTRY ml_zonbie_tasks;
static TRTSK_QUEENTRY ml_waitpid_tasks;

static TRTSK_QUEENTRY ml_wait_by_lock[TLCK_CNT_LOCK_TYPE];

static int ml_tskque_lock;

static int ml_next_pid;

/**
 * @brief mytask01 init
 */
void tsk_init() {
	
	int i;
	TRTSK_QUEENTRY* prev;
	SEGMENT_DESCRIPTOR* gdt = (SEGMENT_DESCRIPTOR*) SEGDSC_ADDR;
	prev = (TRTSK_QUEENTRY*)0;

	for(i=0; i<TRTSK_MAXTASKS; i++) {

		set_segmdesc(gdt + TRTSK_GDT0 + i, 103, (int) &(ml_ary_task[i].task.tss), 
					 AR_TSS32);

		ml_ary_task[i].task.sel = (TRTSK_GDT0 + i) * 8;
		ml_ary_task[i].task.status = TRTSK_STS_DEAD;
		ml_ary_task[i].task.signal_flag = 0;
		ml_ary_task[i].task.process_id = -1;
		ml_ary_task[i].task.parent_pid = -1;
		tsk_tss_init(&(ml_ary_task[i].task.tss));
	}
		
	ml_ary_task[0].next = &(ml_ary_task[1]);
	ml_ary_task[0].prev = &ml_free_tasks;
	for(i=1; i<TRTSK_MAXTASKS-1; i++) {
		ml_ary_task[i].prev = &(ml_ary_task[i-1]);
		ml_ary_task[i].next = &(ml_ary_task[i+1]);
	}
	ml_ary_task[TRTSK_MAXTASKS-1].prev = &(ml_ary_task[TRTSK_MAXTASKS-2]);
	ml_ary_task[TRTSK_MAXTASKS-1].next = &ml_free_tasks;


	ml_cur_task_ent = (TRTSK_QUEENTRY*)0;
	ml_free_tasks.next = &(ml_ary_task[0]);
	ml_free_tasks.prev = &(ml_ary_task[TRTSK_MAXTASKS-1]); 
	trtsk_que_head_init(&ml_running_tasks);
	trtsk_que_head_init(&ml_sleep_tasks);
	trtsk_que_head_init(&ml_waitkbd_tasks);
	trtsk_que_head_init(&ml_zonbie_tasks);
	trtsk_que_head_init(&ml_waitpid_tasks);
	
	for (i=0; i<TLCK_CNT_LOCK_TYPE; i++)
		trtsk_que_head_init(&ml_wait_by_lock[i]);
	
	ml_ts_interval = 0;
	ml_next_pid = 0;
	ml_tskque_lock = 0;
}

/**
 * @brief alloc new task
 * @param cs cs selector
 * @param ds_ss ds, es, fs, gs, ss selector
 * @param eip
 * @param cr3
 * @param cwd current-working-directory path-name
 * @return allocated new task
 */
TRTSK_QUEENTRY* tsk_new_task(int cs, int ds_ss, int eip, int cr3, 
		char* cwd) {
	TRTSK_QUEENTRY* new;
	
	if (!(new = tsk_alloc_new_task())) return 0;
	
	tfl_init_userfd(new->task.fdtbl);
	
	tsk_tss_init(&new->task.tss);

	new->task.status = TRTSK_STS_RUNNING;

	tsk_set_tss_segregs(&new->task.tss, cs, ds_ss);
	
	new->task.tss.eip = eip;
	new->task.tss.cr3 = cr3;

	new->task.esp0_top = 0;
	new->task.kthr_stkarea_addr = 0;
	new->task.vminf = 0;

	new->task.process_id = ml_next_pid++;
	new->task.signal_flag = 0;
	
	my_strcpy(new->task.cwd, cwd);
	
	return new;
	
}

/**
 * @brief set segment regsiter value to TSS
 * @param tss
 * @param cs
 * @param ds_ss   ds, es, fs, gs, ss
 * 
 * ss0 is always set 8
 */  
void tsk_set_tss_segregs(TSS32* tss, int cs, int ds_ss) {
	tss->ss0 = 8;
	tss->es = ds_ss;
	tss->cs = cs;
	tss->ss = ds_ss;
	tss->ds = ds_ss;
	tss->fs = ds_ss;
	tss->gs = ds_ss;
}

/**
 * @brief lock task-ques 
 * 
 * if it couldn't lock, switch to next task
 */
void tsk_lock_tskques() {
	
	while(tlck_swap(&ml_tskque_lock)) {
		if (trtsk_is_one_entry_que(&ml_running_tasks))
			myutil_panic("trtsk_tskques_lock: There is ONLY ONE TASK, but couldn't get a tskque_lock!");
		
		tsk_curtsk_next();
		tsk_farjmp_curtask();
	}
}

/**
 * @brief unlock task-ques 
 */
void tsk_unlock_tskques() {
	ml_tskque_lock = 0;
}


/**
 * @brief farjmp ml_cur_task_ent task
 */
void tsk_farjmp_curtask() {
	farjmp(0, ml_cur_task_ent->task.sel);
}

/**
 * @brief add task to task-que with locking
 * @param que_hd task-que
 * @param tskent task-que-entry
 */
void tsk_add_to_que(TRTSK_QUEENTRY* que_hd, TRTSK_QUEENTRY* tskent) {
	tsk_lock_tskques();
	trtsk_add_to_que(que_hd, tskent);
	tsk_unlock_tskques();
}

/**
 * @brief remove task from task-que with locking
 * @param tskent task-que-entry
 */
void tsk_remove_from_que(TRTSK_QUEENTRY* tskent) {
	tsk_lock_tskques();
	trtsk_remove_from_que(tskent);
	tsk_unlock_tskques();
}
/**
 * @brief alloc task-entry from free entries
 * @return allocated task-entry
 */
TRTSK_QUEENTRY* tsk_alloc_new_task() {
	TRTSK_QUEENTRY* p_tskent;
	
	if (trtsk_is_empty_que(&ml_free_tasks)) // no available task entry
		return 0;
	
	//tlck_stop_intr();
	p_tskent = ml_free_tasks.next;

	
	tsk_remove_from_que(p_tskent);

	
	p_tskent->task.exit_status = -1;
	//tlck_allow_intr();
	
	return p_tskent;
}

TRTSK_TASK* tsk_new_task_knl_default(int eip, int cr3) {
	return tsk_new_task_knl(eip, cr3, 
						TSK_APP_STACK_SIZE, TSK_KNL_STACK_SIZE);
}


/**
 * @brief alloc new kernel task
 * @param eip
 * @param cr3
 * @param app_stk_size (for esp)
 * @param knl_stk_size kernel stack size (for esp0)
 * @return 
 */
TRTSK_TASK* tsk_new_task_knl(int eip, int cr3, 
						   int app_stk_size, int knl_stk_size) {
	TRTSK_QUEENTRY* tsk_ent;
	
	tsk_ent = tsk_new_task(2*8, 1*8, eip, cr3, 0);

	tsk_alloc_stack_knl(&tsk_ent->task, app_stk_size, knl_stk_size);

	tsk_add_to_running(tsk_ent);
	
	return &tsk_ent->task;
}

/**
 * @brief body procedure of 'fork()'
 * @param cr3 cr3 register value
 * @param ebp ebp register value
 * @return process id of child task
 */
int tsk_do_fork(int cr3, int ebp) {
	
	TRTSK_QUEENTRY* p_new_tskent;
	TRMPG_VM_INF* new_vminf;
	TRMPG_VM_INF* src_vminf;
	
	int sz_pg_knl_stk = 2;
	
	debug_puts("tsk_do_fork() started.");

	src_vminf = ml_cur_task_ent->task.vminf;
	
	// clone vminf
	new_vminf = tmpgm_clone_appelf_pdt(src_vminf);
	
	// allocate new task struct
	if (!(p_new_tskent = tsk_alloc_new_task())) return -1;

	// clone file table
	tsk_clone_fdtbl(p_new_tskent->task.fdtbl, ml_cur_task_ent->task.fdtbl);

	p_new_tskent->task.status = TRTSK_STS_RUNNING;

	tsk_tss_init(&p_new_tskent->task.tss);
	
	//set segment registers
	tsk_set_tss_segregs(&p_new_tskent->task.tss, 0x10, 8);
	
	p_new_tskent->task.tss.esp0 = new_vminf->vmmap_hd[TRMPG_VMMAP_KNLSTK].next->vir_addr + MM_PGSIZE * sz_pg_knl_stk;

	p_new_tskent->task.tss.cr3 = new_vminf->pdt_addr;
	p_new_tskent->task.tss.eip = (int)asm_fork_start_child;
	p_new_tskent->task.tss.eflags = 0x00000202;
	p_new_tskent->task.tss.eax = 0;	//child process return value should be 0.
	p_new_tskent->task.tss.ebp = ebp;
	
	debug_puts("set TSS done.");

	tsk_debugout_tss(&p_new_tskent->task.tss);
	
	p_new_tskent->task.process_id = ml_next_pid++;
	p_new_tskent->task.signal_flag = 0;
	p_new_tskent->task.parent_pid = ml_cur_task_ent->task.process_id;
	
	my_strcpy(p_new_tskent->task.cwd, ml_cur_task_ent->task.cwd);

	p_new_tskent->task.vminf = new_vminf;
	
	tsk_add_to_running(p_new_tskent);

	debug_puts("tsk_do_fork() end.");
	return p_new_tskent->task.process_id;
}

/**
 * @brief clone fd-table for 'fork()'
 * @param dst_fdtbl  destination fd-table
 * @param src_fdtbl	source fd-table
 */
void tsk_clone_fdtbl(TFL_HND_FILE** dst_fdtbl, TFL_HND_FILE** src_fdtbl) {
	int i;
	
	for (i=0; i<TFL_MAX_USERFD; i++) {
		if (src_fdtbl[i]) {
			src_fdtbl[i]->ref_count++;
			dst_fdtbl[i] = src_fdtbl[i];
		} else {
			dst_fdtbl[i] = 0;
		}
	}
}

/**
 * @brief remove task from running-que
 * @param remove_ent task-entry to be removed from running-que
 */
void tsk_remove_from_running(TRTSK_QUEENTRY* remove_ent) {
	tsk_remove_from_que(remove_ent);
}	

/**
 * @brief remove task from sleep-quememlock_
 * @param remove_ent task-entry to be removed from sleep-que
 */
void tsk_remove_from_sleep(TRTSK_QUEENTRY* remove_ent) {
	tsk_remove_from_que(remove_ent);
}	


/*
 * tss init
 */
void tsk_tss_init(TSS32* tss) {
	tss->backlink = 0;
	tss->cr3 = 0;
	tss->eflags = 0x00000202;
	tss->eip = 0;
	tss->eax = 0;
	tss->ecx = 0;
	tss->edx = 0;
	tss->ebx = 0;
	tss->esp = 0;
	tss->ebp = 0;
	tss->esi = 0;
	tss->edi = 0;
	tss->es = 0;
	tss->cs = 0;
	tss->ss = 0;
	tss->ds = 0;
	tss->fs = 0;
	tss->gs = 0;
	tss->ldtr = 0;
	tss->iomap = 0x40000000;
}


/*
 * set cur_task
 * expected calling only once after first tsk_new_task
 */
void tsk_init_cur_task() {
	
	if (trtsk_is_empty_que(&ml_running_tasks)) {
		debug_puts("ERROR! - tsk_init_cur_task.");
		while(1)
			io_hlt();
	}

	ml_cur_task_ent = ml_running_tasks.next;
}

/*
 * set cur_task
 */
void tsk_set_cur_task(TRTSK_TASK* task) {
	TRTSK_QUEENTRY* p_entry;
	
	p_entry = ml_running_tasks.next;

	while (p_entry->next != &ml_running_tasks) {
		if ((task = &(p_entry->task))) {
			ml_cur_task_ent = p_entry;
			break;
		}
		p_entry = p_entry->next;
	}
}

/*
 * get current task
 */
TRTSK_TASK* tsk_get_cur_task() {
	return &ml_cur_task_ent->task;
}


/**
 * @brief switch task
 * 
 * you can enable task switching by
 * setting ts_interval > 0 and vice versa.
 */
void tsk_switch() {
	
	if (!ml_ts_interval) 
		return;

	if (!trtsk_is_empty_que(&ml_sleep_tasks)) {
		if (ml_sleep_tasks.next->timeout < g_time_counter) {
			TRTSK_QUEENTRY* wakeup_ent;
			wakeup_ent = ml_sleep_tasks.next;
			tsk_remove_from_sleep(wakeup_ent);
			wakeup_ent->timeout = 0;
			tsk_add_to_running(wakeup_ent);
		}
	}


	if (g_time_counter > g_ts_timeout){
		if (tsk_curtsk_next()) {
			if (ml_cur_task_ent->task.signal_flag) {
				ml_cur_task_ent->task.tss.eip = (int)tsk_terminate_curtask;
			} 
			g_ts_timeout = g_time_counter + ml_ts_interval;
			//debug_varval01("tsk_switch:next_task", ml_cur_task_ent->task.sel, 1);
			//tlck_allow_intr();
			farjmp(0, ml_cur_task_ent->task.sel);
		}
	}
	
}

void tsk_set_switch_interval(unsigned int itvl) {
	ml_ts_interval = itvl;
}

unsigned int tsk_get_switch_interval() {
	return ml_ts_interval;
}



/**
 * @brief switch current task to next task
 * @return 0: not switched, 1: switched next task
 */
TRTSK_QUEENTRY* tsk_curtsk_next() {
	TRTSK_QUEENTRY* prev_task = 0;
	
	// do nothing if the number of task is 1.
	if (trtsk_is_one_entry_que(&ml_running_tasks))
		return 0;
	
	prev_task = ml_cur_task_ent;
	if (ml_cur_task_ent->next != &ml_running_tasks)
		ml_cur_task_ent = ml_cur_task_ent->next;
	else
		ml_cur_task_ent = ml_running_tasks.next;
	return prev_task;
}

void tsk_add_to_sleep(TRTSK_QUEENTRY* p_entry, int sleep_time) {

	p_entry->timeout = g_time_counter + sleep_time;
	p_entry->task.status = TRTSK_STS_SLEEP;
	tsk_add_to_que(&ml_sleep_tasks, p_entry);

}

void tsk_add_to_running(TRTSK_QUEENTRY* p_entry) {
	tsk_add_to_que(&ml_running_tasks, p_entry);
}

/**
 * @brief add task to 'zonbie' task-que
 * @param p_entry task-que-entry
 */
void tsk_add_to_zonbie(TRTSK_QUEENTRY* p_entry) {

	p_entry->task.status = TRTSK_STS_ZONBIE;
	tsk_add_to_que(&ml_zonbie_tasks, p_entry);
	
}

/**
 * @brief move task from current que to other que
 * @param que_hd move destination que
 * @param tskent task-que-entry to move
 */ 
void tsk_move_to(TRTSK_QUEENTRY* que_hd, TRTSK_QUEENTRY* tskent) {
	tsk_lock_tskques();
	trtsk_remove_from_que(tskent);
	trtsk_add_to_que(que_hd, tskent);
	tsk_unlock_tskques();
}


/**
 * @brief terminate task
 * 
 */
void tsk_terminate_curtask() {
	TRTSK_QUEENTRY* remove_ent;

	if (trtsk_is_one_entry_que(&ml_running_tasks))
		myutil_panic("tsk_terminate_curtask: cannot terminate");

	remove_ent = tsk_curtsk_next();

	tlck_stop_intr();
	//TODO re-think about locking and que operation
	tsk_remove_from_running(remove_ent);
	
	// free phisical memory
	tsk_free_memory(&remove_ent->task);

	trmpg_free_vminf(remove_ent->task.vminf);
	
	tlck_allow_intr();

	debug_varval01("tsk_terminate_curtask next task.sel", ml_cur_task_ent->task.sel, 1);
	farjmp(0, ml_cur_task_ent->task.sel);
	
}

/**
 * @brief 'zonbie' current task
 * 
 * (for system call 0x01)
 */
void tsk_zonbie_curtask(int sts) {
	TRTSK_QUEENTRY* remove_ent;
	short *p_sts_info, *p_sts_code;
	int *p_exit_status;
	unsigned int saved_ts_itvl;
	
	if (trtsk_is_one_entry_que(&ml_running_tasks))
		myutil_panic("tsk_zonbie_curtask: cannot terminate");

	//tlck_stop_intr();
	
	remove_ent = tsk_curtsk_next();

	p_exit_status = &(remove_ent->task.exit_status);
	p_sts_info = (short*) p_exit_status;
	p_sts_code = p_sts_info + 1;
	*p_sts_code = 0;
	*p_sts_info = sts & 0xffff;
	
	saved_ts_itvl = tsk_get_switch_interval();
	tsk_set_switch_interval(0);
	remove_ent->task.status = TRTSK_STS_ZONBIE;
	//tsk_remove_from_running(remove_ent);
	//tsk_add_to_zonbie(remove_ent);
	tsk_move_to(&ml_zonbie_tasks, remove_ent);
	tsk_wakeup_in_que(&ml_waitpid_tasks);
	
	// free phisical memory
	tsk_free_memory(&remove_ent->task);
	trmpg_free_vminf(remove_ent->task.vminf);
	
	//tlck_allow_intr();
	tsk_set_switch_interval(saved_ts_itvl);

	debug_varval01("tsk_terminate_curtask next task.sel", ml_cur_task_ent->task.sel, 1);
	farjmp(0, ml_cur_task_ent->task.sel);
	
}

//XXX implement status and options works
/**
 * @breif waitpid() implementation
 * @param pid process id to wait
 * @param status	see 'wait.h' in newlib
 * @param options
 * 
 * warning: status and options parameter doesn't work properly
 * at the moment.
 */
int tsk_waitpid(int pid, int* status, int options) {
	TRTSK_QUEENTRY* tskent_current;
	TRTSK_QUEENTRY* tskent_child;
	unsigned int saved_ts_itvl;
	
	if (!(tskent_child = tsk_lookup_all_task(pid, 1))) {
		return -1;
	}

	while(1) {
		if (tskent_child->task.status == TRTSK_STS_ZONBIE) {
			tlck_stop_intr();
			*status = tskent_child->task.exit_status;
			tskent_child->task.status = TRTSK_STS_DEAD;
			tsk_move_to(&ml_free_tasks, tskent_child);
			break;
		}
		tskent_current = tsk_curtsk_next();
		tskent_current->task.status = TRTSK_STS_WAITPID;
		
		saved_ts_itvl = tsk_get_switch_interval();
		tsk_set_switch_interval(0);
		tsk_move_to(&ml_waitpid_tasks, tskent_current);
		
		tsk_set_switch_interval(saved_ts_itvl);
		
		tsk_farjmp_curtask();
	}
	return pid;
}

/**
 * @brief sleep task until timeout
 * (for system call 0x0x)
 * @param timeout
 */
void tsk_sleep_curtask(int timeout) {
	TRTSK_QUEENTRY* sleep_ent;
	unsigned int saved_ts_itvl;

	//tlck_stop_intr();
	
	if (trtsk_is_one_entry_que(&ml_running_tasks)) {
		debug_puts("ERROR: cannot sleep");
		return;
	}

	//sleep_ent = ml_cur_task_ent;
	//TODO re-think about locking and que operation
	//tsk_remove_from_running(sleep_ent);

	//ml_cur_task_ent = sleep_ent->next;
	//if(!ml_cur_task_ent)
		//ml_cur_task_ent = ml_running_tasks.next;

	//tsk_add_to_sleep(sleep_ent, timeout);

	//tlck_allow_intr();

	
	sleep_ent = tsk_curtsk_next();
	sleep_ent->timeout = g_time_counter + timeout;
	
	saved_ts_itvl = tsk_get_switch_interval();
	tsk_set_switch_interval(0);
	sleep_ent->task.status = TRTSK_STS_SLEEP;
	tsk_move_to(&ml_sleep_tasks, sleep_ent);
	tsk_set_switch_interval(saved_ts_itvl);
	
	tsk_farjmp_curtask();
	
}

/**
 * @brief wait current task for keyboard-interrupt
 */ 
void tsk_waitkbd_curtask() {
	TRTSK_QUEENTRY* waittask_ent;

	tlck_stop_intr();

	if (trtsk_is_one_entry_que(&ml_running_tasks)) {
		debug_puts("ERROR: cannot sleep");
		return;
	}

	waittask_ent = tsk_curtsk_next();
	
	waittask_ent->task.status = TRTSK_STS_WAITKBD;
	
	tsk_move_to(&ml_waitkbd_tasks, waittask_ent);
	
	tsk_farjmp_curtask();
}


/**
 * @brief wake-up all tasks in 'wait keyboard interrupt' task-que
 */
void tsk_wakeup_waitkbd() {

	tsk_wakeup_in_que(&ml_waitkbd_tasks);
}

/**
 * @brief wake-up all tasks in task-que
 */
void tsk_wakeup_in_que(TRTSK_QUEENTRY* que_hd) {
	unsigned int saved_ts_itvl;
	TRTSK_QUEENTRY* tskent;
	
	if (!(trtsk_count_tasks(que_hd))) return;
	saved_ts_itvl = tsk_get_switch_interval();
	
	tsk_lock_tskques();

	tskent = que_hd->next;
	while(tskent != que_hd) {
		tskent->task.status = TRTSK_STS_RUNNING;
		tskent = tskent->next;
	}

	trtsk_join_ques(&ml_running_tasks, que_hd);
	tsk_unlock_tskques();
	tsk_set_switch_interval(saved_ts_itvl);

}


void tsk_wakeup_locked_tasks(int lock_type) {
	//debug_varval01("tsk_wakeup_locked_tasks() lock_type", lock_type, 1);
	//tlck_stop_intr();
	tsk_wakeup_in_que(&ml_wait_by_lock[lock_type]);
	//tsk_debug_running();
	//tlck_allow_intr();
}

void tsk_sleep_by_lock(int lock_type) {
	TRTSK_QUEENTRY* tskent;
	unsigned int saved_ts_itvl;
	
	tskent = tsk_curtsk_next();
	tskent->task.status = TRTSK_STS_WAITLOCK;
	saved_ts_itvl = tsk_get_switch_interval();
	tsk_move_to(&ml_wait_by_lock[lock_type], tskent);

	tsk_set_switch_interval(saved_ts_itvl);
	tsk_farjmp_curtask();
}

/**
 * @brief set signal to the task
 * @param pid process id to set signal
 * @param signal signal value
 * @return 0: success, -1: process not found
 */
int tsk_set_signal(int pid, int signal) {
	TRTSK_QUEENTRY* p_tskent;
	int rc = 0;
	
	tlck_stop_intr();
	
	if (pid == 0) return -1;	//pid 0 is idle task. never killed.
	
	p_tskent = tsk_lookup_all_task(pid, 0);
	if (p_tskent)
		p_tskent->task.signal_flag = signal;
	else
		rc = -1;

	debug_varval01("set_signal done. pid", pid, 1);
	
	tlck_allow_intr();
	
	return rc;
}

/**
 * @brief look up task by process-id
 * @param pid process id
 * @param flg_zonbie 0: exclude zonbie tasks, 1: include zonbie tasks
 * @return pid task, 0 if not found
 */
TRTSK_QUEENTRY* tsk_lookup_all_task(int pid, int flg_zonbie) {
	TRTSK_QUEENTRY* p_tskent;

	//TODO lookup not only runnning-que
	
	if ((p_tskent = trtsk_lookup_task_in_que(pid, &ml_running_tasks))) {
		return p_tskent;
	}
	
	if ((p_tskent = trtsk_lookup_task_in_que(pid, &ml_sleep_tasks))) {
		return p_tskent;
	}

	if (flg_zonbie) {
		if ((p_tskent = trtsk_lookup_task_in_que(pid, &ml_zonbie_tasks))) {
			return p_tskent;
		}
	}
	
	return 0;
}

/**
 * @brief look up task in the que by process-id
 * @param pid process id
 * @return pid task, 0 if not found
 */
TRTSK_QUEENTRY* tsk_lookup_task_in_que_old(int pid, TRTSK_QUEENTRY* que) {
	TRTSK_QUEENTRY* p_tskent;

	p_tskent = que;
	
	while (p_tskent) {
		if (pid == p_tskent->task.process_id)
			return p_tskent;
		p_tskent = p_tskent->next;
	}
	
	return 0;
}

/**
 * @brief count free tasks
 * @return count of free tasks
 */
int tsk_count_free_tasks() {
	
	return trtsk_count_tasks(&ml_free_tasks);
}

/**
 * @brief count zonbie tasks
 * @return count of free tasks
 */
int tsk_count_zonbie_tasks() {
	
	//return tsk_count_tasks_old(ml_zonbie_tasks);
	return trtsk_count_tasks(&ml_zonbie_tasks);
}

/**
 * @brief allocate process stack for kernel thread
 */
void tsk_alloc_stack_knl(TRTSK_TASK* task, int kthr_stk_size, int knl_stk_size) {
	char buff[80], buff2[80];
	
	if(kthr_stk_size) {
		//task->kthr_stkarea_addr =  mm_malloc_pg_knl(kthr_stk_size);
		task->kthr_stkarea_addr =  tpmmm_malloc_pg_knl(kthr_stk_size);
		task->tss.esp = task->kthr_stkarea_addr + kthr_stk_size;
		
	}

	//XXX I think esp0 will be never used in kernel thread.(never switch privilege
	if (knl_stk_size) {
		//task->esp0_top = mm_malloc_pg_knl(knl_stk_size);
		task->esp0_top = tpmmm_malloc_pg_knl(knl_stk_size);
		task->tss.esp0 = task->esp0_top + knl_stk_size;
	}

	debug_puts("tsk_alloc_stack");
	sprint_varval(buff, "task->kthr_stkarea_addr", task->kthr_stkarea_addr, 1);
	sprint_varval(buff2, ", task->esp0_top", task->esp0_top, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);

}

/*
 * allocate kernel stack for app
 *
 * @task
 * @sz_pg_knl_stk  kernel stack page(4kb) size
 * @return !=0: sucess, =0: error or not alloc
 */
/*
unsigned int tsk_alloc_knlstack_app(TSK_TASK* task, int sz_pg_knl_stk) {
	char buff[80], buff2[80];
	unsigned int stk_area_phs_addr, stk_area_vir_addr, pt_addr;
	unsigned int ret = 0;
	TRMPG_VM_MAP_ITEM* vmmap;
	
	if (sz_pg_knl_stk) {
		stk_area_phs_addr = mm_malloc_pg_app(MM_PGSIZE * sz_pg_knl_stk);
		stk_area_vir_addr = TSK_KNL_STACK_VIR_ADDR;
		//task->esp0_top = stk_area_phs_addr;
		//
		// kernel stack area setting
		//
		
		if (!(vmmap = pag_add_vmmap(task->vminf, TRMPG_VMMAP_KNLSTK, stk_area_phs_addr, 
				stk_area_vir_addr, sz_pg_knl_stk)))
			return 0;
		
		pt_addr = task->vminf->pdt_addr + 0x1000;
		// set page-table according to vmmap
		debug_puts("tsk_alloc_knlstack_app()");
		debug_varval01("  vmmap->sz_page", vmmap->sz_page, 0);
		debug_varval01("  pt_addr", pt_addr, 1);
		
		pag_set_pt_vmmap(vmmap, pt_addr);
		
		
		
		task->tss.esp0 = stk_area_vir_addr + MM_PGSIZE * sz_pg_knl_stk;
		ret = stk_area_phs_addr;
		debug_varval01("  stk_area_phs_addr", stk_area_phs_addr, 1);
		debug_varval01("  esp0", task->tss.esp0, 1);
		debug_varval01("  pdt_addr", task->vminf->pdt_addr, 1);
	}
	
	
	debug_puts("tsk_alloc_knlstack_app");
	sprint_varval(buff, "task->kthr_stkarea_addr", task->kthr_stkarea_addr, 1);
	sprint_varval(buff2, ", task->esp0_top", task->esp0_top, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);

	return ret;
}
*/

/**
 * @brief free process stack and code
 */
void tsk_free_memory(TRTSK_TASK* task) {
	char buff[80], buff2[80];
	
	debug_puts("tsk_free_memory");

	if (task->esp0_top)
		//mm_free(task->esp0_top);
		tpmmm_free(task->esp0_top);

	if (task->kthr_stkarea_addr)
		//mm_free(task->kthr_stkarea_addr);
		tpmmm_free(task->kthr_stkarea_addr);

	sprint_varval(buff, "task->kthr_stkarea_addr", task->kthr_stkarea_addr, 1);
	sprint_varval(buff2, ", task->esp0_top", task->esp0_top, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);

	if (task->vminf)
		tmpgm_free_vminf(task->vminf);


}

void tsk_debugout_tss(TSS32* tss) {
	char buff[80];
	char buff2[80];

	sprint_varval(buff, "backlink", tss->backlink, 1);
	sprint_varval(buff2, ", esp0", tss->esp0, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", ss0", tss->ss0, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);

	sprint_varval(buff, "esp1", tss->esp1, 1);
	sprint_varval(buff2, ", ss1", tss->ss1, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", esp2", tss->esp2, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);
	
	sprint_varval(buff, "ss2", tss->ss2, 1);
	sprint_varval(buff2, ", cr3", tss->cr3, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", eip", tss->eip, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);

	sprint_varval(buff, "eflags", tss->eflags, 1);
	sprint_varval(buff2, ", eax", tss->eax, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", ecx", tss->ecx, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);

	sprint_varval(buff, "edx", tss->edx, 1);
	sprint_varval(buff2, ", ebx", tss->ebx, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", esp", tss->esp, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);

	sprint_varval(buff, "ebp", tss->ebp, 1);
	sprint_varval(buff2, ", esi", tss->esi, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", edi", tss->edi, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);

	sprint_varval(buff, "es", tss->es, 1);
	sprint_varval(buff2, ", cs", tss->cs, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", ss", tss->ss, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);

	sprint_varval(buff, "ds", tss->ds, 1);
	sprint_varval(buff2, ", fs", tss->fs, 1);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", gs", tss->gs, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);

	sprint_varval(buff, "ldtr", tss->ldtr, 1);
	sprint_varval(buff2, ", iomap", tss->iomap, 1);
	my_strcat(buff, buff2);
	debug_puts(buff);

}

void tsk_debug_running() {
	char buff[80];
	char buff2[80];

	debug_puts("number tasks");

	sprint_varval(buff, " running", trtsk_count_tasks(&ml_running_tasks), 0);
	sprint_varval(buff2, ", free", tsk_count_free_tasks(), 0);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", zonbie", tsk_count_zonbie_tasks(), 0);
	my_strcat(buff, buff2);
	debug_puts(buff);

	sprint_varval(buff, " waitkbds", trtsk_count_tasks(&ml_waitkbd_tasks), 0);
	sprint_varval(buff2, ", wait_mem", trtsk_count_tasks(&ml_wait_by_lock[TLCK_LOCK_MEM]), 0);
	my_strcat(buff, buff2);
	sprint_varval(buff2, ", wait_io", trtsk_count_tasks(&ml_wait_by_lock[TLCK_LOCK_IO]), 0);
	my_strcat(buff, buff2);
	debug_puts(buff);
	
	trtsk_dump_tasks(&ml_running_tasks, "runnings are ..");
	trtsk_dump_tasks(&ml_zonbie_tasks, "zonbies are ..");
	trtsk_dump_tasks(&ml_waitkbd_tasks, "waitkbds are ...");
	trtsk_dump_tasks(&ml_wait_by_lock[TLCK_LOCK_MEM], "wait_mem are ..");
	trtsk_dump_tasks(&ml_wait_by_lock[TLCK_LOCK_IO], "wait_io are ..");

}

void tsk_dump_tasks(TRTSK_QUEENTRY* que, char* head_msg) {
	char buff[80];
	char buff2[80];
	TRTSK_QUEENTRY* p_entry;

	p_entry = que;
	debug_puts(head_msg);

	while (p_entry) {
		sprint_varval(buff, "  pid", p_entry->task.process_id, 0);
		sprint_varval(buff2, ", ppid", p_entry->task.parent_pid, 0);
		my_strcat(buff, buff2);
		sprint_varval(buff2, ", sel", p_entry->task.sel, 1);
		my_strcat(buff, buff2);
		sprint_varval(buff2, ", status", p_entry->task.status, 1);
		my_strcat(buff, buff2);
		debug_puts(buff);
		p_entry = p_entry->next;
	}
}

