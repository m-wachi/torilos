#ifndef MYTASK01_H
#define MYTASK01_H

#include "mypage01.h"
#include "trfile.h"
#include "trmempage.h"
#include "trtask.h"

#define TSK_APP_STACK_SIZE 			64*1024
#define TSK_APP_STACK_VIR_ADDR		0x480000
#define TSK_KNL_STACK_SIZE 			64*1024
#define TSK_KNL_STACK_VIR_ADDR		0x4A0000

#define TSK_MAX_ALC_PHS_ADDR 	8

void tsk_init();

TRTSK_QUEENTRY* tsk_new_task(int cs, int ds_ss, int eip, int cr3, char* cwd);
void tsk_set_tss_segregs(TSS32* tss, int cs, int ds_ss);
void tsk_lock_tskques();
void tsk_unlock_tskques();
void tsk_farjmp_curtask();
void tsk_add_to_que(TRTSK_QUEENTRY* que_hd, TRTSK_QUEENTRY* tskent);
void tsk_remove_from_que(TRTSK_QUEENTRY* tskent);
TRTSK_QUEENTRY* tsk_alloc_new_task();
TRTSK_TASK* tsk_new_task_knl(int eip, int cr3, 
						   int app_stk_size, int knl_stk_size);
TRTSK_TASK* tsk_new_task_appelf2(unsigned int prg_load_phs_addr, int sz_prg,
							  int sz_pg_app_stk, int sz_pg_knl_stk,
							  int argc, char** argv,
							  int envc, char** envp,
							  char* cwd);
int tsk_do_fork(int cr3, int ebp);
void tsk_clone_fdtbl(TFL_HND_FILE** dst_fdtbl, TFL_HND_FILE** src_fdtbl);

TRTSK_TASK* tsk_new_task_knl_default(int eip, int cr3);
void tsk_remove_from_running(TRTSK_QUEENTRY* p_entry);
void tsk_remove_from_waitkbd(TRTSK_QUEENTRY* remove_ent);
void tsk_tss_init(TSS32* tss);
void tsk_init_cur_task();
TRTSK_TASK* tsk_get_cur_task();
void tsk_switch();
void tsk_set_switch_interval(unsigned int itvl);
unsigned int tsk_get_switch_interval();
TRTSK_QUEENTRY* tsk_curtsk_next();
void tsk_add_to_sleep(TRTSK_QUEENTRY* p_entry, int sleep_time);
void tsk_add_to_running(TRTSK_QUEENTRY* p_entry);
void tsk_add_to_zonbie(TRTSK_QUEENTRY* p_entry);
void tsk_move_to(TRTSK_QUEENTRY* que_hd, TRTSK_QUEENTRY* tskent);
void tsk_terminate_curtask();
void tsk_zonbie_curtask(int sts);
int tsk_waitpid(int pid, int* status, int options);
void tsk_sleep_curtask(int timeout);
void tsk_waitkbd_curtask();
void tsk_wakeup_waitkbd();
void tsk_wakeup_in_que_old(TRTSK_QUEENTRY** pp_que);
void tsk_wakeup_in_que(TRTSK_QUEENTRY* pp_que);
void tsk_wakeup_locked_tasks(int lock_type);
void tsk_sleep_by_lock(int lock_type);
int tsk_set_signal(int pid, int signal);
TRTSK_QUEENTRY* tsk_lookup_all_task(int pid, int flg_zonbie);
TRTSK_QUEENTRY* tsk_lookup_task_in_que_old(int pid, TRTSK_QUEENTRY* que);
int tsk_count_free_tasks();
void tsk_alloc_stack_knl(TRTSK_TASK* task, int app_stk_size, int knl_stk_size);
unsigned int tsk_alloc_knlstack_app(TRTSK_TASK* task, int sz_pg_knl_stk);
void tsk_free_memory(TRTSK_TASK* task);
void tsk_debugout_tss(TSS32* tss);
void tsk_debug_running();
void tsk_dump_tasks(TRTSK_QUEENTRY* que, char* head_msg);

#endif

