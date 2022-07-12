#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <torilos/trtask.h>

int test_lookup_task_in_que();

int test_add_remove_count_lookup();
int check_tskent(TRTSK_QUEENTRY* tskent, int exp_next_id,	int exp_prev_id, 
		char* func_name, char* ent_id);
int check_task_count(TRTSK_QUEENTRY* que_hd, int exp_cnt);
int test_join_ques();

int main(int argc, char** argv) {
	int errcnt = 0;
	
	/*
	 * add_to_que test
	 */
	errcnt += test_add_remove_count_lookup();
	//errcnt += test_lookup_task_in_que();
	
	/*
	 * join_ques test
	 */
	errcnt += test_join_ques();
	
	
	if (errcnt)
		return -1;
		
	puts("OK - test passed.");
	return 0;
}

int test_add_remove_count_lookup() {
	int errcnt = 0, i;
	TRTSK_QUEENTRY tskent[5];
	TRTSK_QUEENTRY que_hd;
	
	que_hd.task.process_id = -1;
	for (i=0; i<5; i++) {
		tskent[i].task.process_id = i;
	}
	
	trtsk_que_head_init(&que_hd);
	errcnt += check_task_count(&que_hd, 0);
	if (!trtsk_is_empty_que(&que_hd)) {
		fprintf(stderr, "Failed - trtsk_is_empty_que()\n");
		errcnt++;
	}
	if ((trtsk_lookup_task_in_que(0, &que_hd))) {
		fprintf(stderr, "Failed - trtsk_lookup_task_in_que(): something found\n");
		errcnt++;
	}
	
	trtsk_add_to_que(&que_hd, &tskent[0]);
	if (trtsk_is_empty_que(&que_hd)) {
		fprintf(stderr, "Failed - trtsk_is_empty_que()\n");
		errcnt++;
	}
	errcnt += check_tskent(&que_hd, 0, 0, "trtsk_add_to_que", "que_id");
	errcnt += check_tskent(&tskent[0], -1, -1, "trtsk_add_to_que", "tskent[0]");	
	errcnt += check_task_count(&que_hd, 1);
	if (&tskent[0] != trtsk_lookup_task_in_que(0, &que_hd)) {
		fprintf(stderr, "Failed - trtsk_lookup_task_in_que()\n");
		errcnt++;
	}

	trtsk_add_to_que(&que_hd, &tskent[1]);
	errcnt += check_tskent(&que_hd, 0, 1, "trtsk_add_to_que", "que_id");
	errcnt += check_tskent(&tskent[0], 1, -1, "trtsk_add_to_que", "tskent[0]");
	errcnt += check_tskent(&tskent[1], -1, 0, "trtsk_add_to_que", "tskent[1]");
	errcnt += check_task_count(&que_hd, 2);

	trtsk_add_to_que(&que_hd, &tskent[2]);
	if (&tskent[1] != trtsk_lookup_task_in_que(1, &que_hd)) {
		fprintf(stderr, "Failed - trtsk_lookup_task_in_que()\n");
		errcnt++;
	}
	
	trtsk_remove_from_que(&tskent[1]);
	if (tskent[1].next || tskent[1].prev) {
		fprintf(stderr, "Failed - trtsk_remove_from_que(): next or prev is not NULL\n");
		errcnt++;
	}
	errcnt += check_task_count(&que_hd, 2);
	errcnt += check_tskent(&tskent[0], 2, -1, "trtsk_add_to_que", "tskent[0]");
	errcnt += check_tskent(&tskent[2], -1, 0, "trtsk_add_to_que", "tskent[2]");
	if (trtsk_lookup_task_in_que(1, &que_hd)) {
		fprintf(stderr, "Failed - trtsk_lookup_task_in_que()\n");
		errcnt++;
	}
	if (&tskent[2] != trtsk_lookup_task_in_que(2, &que_hd)) {
		fprintf(stderr, "Failed - trtsk_lookup_task_in_que()\n");
		errcnt++;
	}
	
	return errcnt;
}

// task entry 'next', 'prev' test
int check_tskent(TRTSK_QUEENTRY* tskent, int exp_next_id, int exp_prev_id, 
		char* func_name, char* ent_id) {
	int errcnt = 0;
	
	if (tskent->next->task.process_id != exp_next_id) {
		fprintf(stderr, "Failed - %s(): %s.next process_id=%d\n",
				func_name, ent_id, tskent->next->task.process_id);
		errcnt++;
	}
	if (tskent->prev->task.process_id != exp_prev_id) {
		fprintf(stderr, "Failed - %s(): %s.prev process_id=%d\n",
				func_name, ent_id, tskent->prev->task.process_id);
		errcnt++;
	}
	return errcnt;
}

// que-entry count test.
int check_task_count(TRTSK_QUEENTRY* que_hd, int exp_cnt) {
	int errcnt = 0;
	int cnt = 0;
	cnt = trtsk_count_tasks(que_hd);
	
	if (cnt != exp_cnt) {
		fprintf(stderr, "Failed - trtsk_count_tasks(): expect=%d, actual=%d\n",
				exp_cnt, cnt);
		errcnt++;
	}
	return errcnt;
}


int test_join_ques() {
	int errcnt = 0, i;
	TRTSK_QUEENTRY tskent[6];
	TRTSK_QUEENTRY que1_hd, que2_hd;
	
	que1_hd.task.process_id = -1;
	que2_hd.task.process_id = -2;
	
	for (i=0; i<6; i++) {
		tskent[i].task.process_id = i;
	}
	trtsk_que_head_init(&que1_hd);
	trtsk_que_head_init(&que2_hd);
	
	trtsk_add_to_que(&que1_hd, &tskent[0]);
	trtsk_add_to_que(&que1_hd, &tskent[2]);
	trtsk_add_to_que(&que1_hd, &tskent[4]);

	trtsk_join_ques(&que1_hd, &que2_hd);
	errcnt += check_task_count(&que1_hd, 3);
	
	
	trtsk_add_to_que(&que2_hd, &tskent[1]);
	trtsk_add_to_que(&que2_hd, &tskent[3]);
	trtsk_add_to_que(&que2_hd, &tskent[5]);
	
	trtsk_join_ques(&que1_hd, &que2_hd);
	errcnt += check_task_count(&que1_hd, 6);
	errcnt += check_task_count(&que2_hd, 0);
	errcnt += check_tskent(&que1_hd, 0, 5, "trtsk_join_ques", "que_hd");
	errcnt += check_tskent(&tskent[4], 1, 2, "trtsk_join_ques", "taskent[4]");
	errcnt += check_tskent(&tskent[1], 3, 4, "trtsk_join_ques", "taskent[1]");
	errcnt += check_tskent(&tskent[5], -1, 3, "trtsk_join_ques", "taskent[5]");
	
	return errcnt;
}

