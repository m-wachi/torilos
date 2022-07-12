#include <torilos/trtask.h>
#include <torilos/mylib01.h>
#include <torilos/debug01.h>
#include <torilos/trutil.h>

/**
 * @brief initialize que-head
 * @param que_hd que-head
 */
void trtsk_que_head_init(TRTSK_QUEENTRY* que_hd) {
	
	que_hd->next = que_hd;
	que_hd->prev = que_hd;
}



/**
 * @brief add task to task-que
 * @param que_hd task-que
 * @param tskent task-que-entry
 */
void trtsk_add_to_que(TRTSK_QUEENTRY* que_hd, TRTSK_QUEENTRY* tskent) {
	TRTSK_QUEENTRY* tskent_tail;
	
	tskent_tail = que_hd->prev;
	
	tskent->prev = tskent_tail;
	tskent->next = tskent_tail->next;
	tskent_tail->next->prev = tskent;
	tskent_tail->next = tskent;
	
}
/**
 * @brief remove task from task-que
 * @param tskent task-que-entry
 */
void trtsk_remove_from_que(TRTSK_QUEENTRY* tskent) {
	
	tskent->prev->next = tskent->next;
	tskent->next->prev = tskent->prev;
	
	tskent->next = tskent->prev = 0;
	
}


/**
 * @brief count tasks
 * @param que task-que-head to count
 * @return count of tasks
 */
int trtsk_count_tasks(TRTSK_QUEENTRY* que_hd) {
	TRTSK_QUEENTRY* p_tskent;
	int cnt=0;
	
	p_tskent = que_hd->next;
	
	while (p_tskent != que_hd) {
		cnt++;
		p_tskent = p_tskent->next;
	}
	
	return cnt;
}

/**
 * @brief look up task in the que by process-id
 * @param pid process id
 * @return pid task, 0 if not found
 */
TRTSK_QUEENTRY* trtsk_lookup_task_in_que(int pid, TRTSK_QUEENTRY* que) {
	TRTSK_QUEENTRY* p_tskent;

	p_tskent = que->next;
	
	while (p_tskent != que) {
		if (pid == p_tskent->task.process_id)
			return p_tskent;
		p_tskent = p_tskent->next;
	}
	
	return 0;
}

/**
 * @brief check if que is empty
 * @param que_hd que-head
 * @return 0: not empty, !=0: empty
 */
int trtsk_is_empty_que(TRTSK_QUEENTRY* que_hd) {
	return (que_hd->next == que_hd);
}

/**
 * @brief check if que has only one entry
 * @param que_hd que-head
 * @return 0: que entry count is not 1, !=0: que entry count is 1
 */
int trtsk_is_one_entry_que(TRTSK_QUEENTRY* que_hd) {
	return (que_hd->next->next == que_hd);
}

/**
 * @brief join 2 ques
 * @param que1_hd task-que to be add
 * @param que2_hd task-que to add
 * 
 * after joinning, que1 has all entry and que2 has no entry.
 */
void trtsk_join_ques(TRTSK_QUEENTRY* que1_hd, TRTSK_QUEENTRY* que2_hd) {
	que1_hd->prev->next = que2_hd->next;
	que2_hd->next->prev = que1_hd->prev;
	
	que2_hd->prev->next = que1_hd;
	que1_hd->prev = que2_hd->prev;
	trtsk_que_head_init(que2_hd);
}


void trtsk_dump_tasks(TRTSK_QUEENTRY* que_hd, char* head_msg) {
	char buff[80];
	char buff2[80];
	TRTSK_QUEENTRY* p_entry;

	p_entry = que_hd->next;
	debug_puts(head_msg);

	while (p_entry != que_hd) {
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



