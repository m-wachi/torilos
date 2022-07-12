#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../filebuffer.h"

int test01();
int test02();
int test03();
int test04();
int test05();

int assert_test_line(LINE_BUFFER* lb, char* exp, char* testmsg);

int main(int argc, char** argv) {
	int errcnt = 0;
	
	init_buffer();
	
	errcnt += test01();
	
	errcnt += test02();
	
	errcnt += test03();
	
	errcnt += test04();
	
	errcnt += test05();
	
	if (errcnt)
		return -1;
		
	puts("OK - test passed.");
	return 0;
}

// basic test.
int test01() {
	int errcnt = 0;
	LINE_BUFFER* lb;
	
	new_buffer();
	
	lb = get_first_line();
	
	if (0 != strlen(lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - new_line_buffer()\n");
	}
	
	insert_char('a');
	insert_char('b');
	insert_char('c');
	
	if (3 != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - insert_char(): cnt_char=%d\n", lb->cnt_char);
	}
	
	if (strcmp("abc", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - insert_char(): chars=%s\n", lb->chars);
	}
	
	lb = handle_lf();
	
	insert_char('d');
	insert_char('e');
	
	if (2 != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - insert_char(): cnt_char=%d\n", lb->cnt_char);
	}
	
	if (strcmp("de", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - insert_char(): chars=%s\n", lb->chars);
	}
	
	
	lb = get_first_line();
	if (strcmp("abc\n", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - get_first_line(): chars=%s\n", lb->chars);
	}
	
	lb = get_next_line(lb);
	if (strcmp("de", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - get_next_line(): chars=%s\n", lb->chars);
	}
	
	lb = get_next_line(lb);
	if (lb) {
		errcnt+=1;
		fprintf(stderr, "Error - get_next_line(): lb should be NULL\n");
	}
	
	return errcnt;
}

// test move left, right and delete char
int test02() {
	int errcnt = 0, rc;
	LINE_BUFFER* lb;
	
	init_buffer();
	
	new_buffer();
	
	insert_char('a');
	insert_char('b');
	insert_char('c');
	
	cursor_left();
	cursor_left();
	
	insert_char('d');
	
	lb = get_first_line();
	
	if (4 != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_left(): cnt_char=%d\n", lb->cnt_char);
	}
	
	if (strcmp("adbc", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_left(): chars=%s\n", lb->chars);
	}

	cursor_left();
	rc = cursor_left();
	if (rc != 1) {
		errcnt+=1;
		fprintf(stderr, "Error - test02() cursor_left(): rc=%d\n", rc);
	}
	
	rc = cursor_left();
	if (rc != 0) {
		errcnt+=1;
		fprintf(stderr, "Error - test02() cursor_left(): rc=%d\n", rc);
	}

	insert_char('e');
	
	if (strcmp("eadbc", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_left(): chars=%s\n", lb->chars);
	}

	
	cursor_right();
	
	insert_char('f');
	
	if (6 != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_right(): cnt_char=%d\n", lb->cnt_char);
	}
	
	if (strcmp("eafdbc", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_right(): chars=%s\n", lb->chars);
	}

	cursor_right();
	delete_char();
	
	if (5 != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - test02() delete_char(): cnt_char=%d\n", lb->cnt_char);
	}
	
	if (strcmp("eafdc", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - test02() delete_char(): chars=%s\n", lb->chars);
	}
	
	cursor_left();
	cursor_left();
	delete_char();
	
	if (4 != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - test02() 2nd delete_char(): cnt_char=%d\n", lb->cnt_char);
	}
	
	if (strcmp("eadc", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - test02() 2nd delete_char(): chars=%s\n", lb->chars);
	}

	cursor_right();
	rc = cursor_right();
	if (rc != 1) {
		errcnt+=1;
		fprintf(stderr, "Error - test02() cursor_right(): rc=%d\n", rc);
	}
	
	rc = cursor_right();
	if (rc != 0) {
		errcnt+=1;
		fprintf(stderr, "Error - test02() cursor_right(): rc=%d\n", rc);
	}

	return errcnt;
}

int test03() {
	int errcnt = 0;
	int x_pos;
	LINE_BUFFER* lb;
	
	init_buffer();
	
	new_buffer();
	
	insert_char('a');
	insert_char('b');
	insert_char('c');
	insert_char('d');
	insert_char('e');
	
	cursor_left();
	cursor_left();
	
	handle_lf();
	
	lb = get_first_line();
	
	if (4 != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - handle_lf() (1): cnt_char=%d\n", lb->cnt_char);
	}
	
	if (strcmp("abc\n", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - handle_lf() (1): chars=%s\n", lb->chars);
	}
	
	lb = get_next_line(lb);

	if (2 != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - handle_lf() (2): cnt_char=%d\n", lb->cnt_char);
	}
	
	if (strcmp("de", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - handle_lf() (2): chars=%s\n", lb->chars);
	}
	
	insert_char('f');
	insert_char('g');
	insert_char('h');
	handle_lf();
	
	
	if (4 != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - handle_lf() (3): cnt_char=%d\n", lb->cnt_char);
	}
	
	if (strcmp("fgh\n", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - handle_lf() (3): chars=%s\n", lb->chars);
	}
	
	lb = get_next_line(lb);

	if (2 != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - handle_lf() (4): cnt_char=%d\n", lb->cnt_char);
	}
	
	if (strcmp("de", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - handle_lf() (4): chars=%s\n", lb->chars);
	}
	
	cursor_right();
	cursor_up();
	
	lb = get_cursor_line();
	
	if (4 != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_up() (1): cnt_char=%d\n", lb->cnt_char);
	}
	
	if (strcmp("fgh\n", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_up() (1): chars=%s\n", lb->chars);
	}
	
	x_pos = get_cursor_x_pos();
	if (1 != x_pos) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_up() (1): x_pos=%d\n", x_pos);
	}
	
	cursor_right();
	cursor_right();
	cursor_right();
	insert_char('i');
	insert_char('j');
	cursor_up();
	
	lb = get_cursor_line();
	
	if (4 != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_up() (2): cnt_char=%d\n", lb->cnt_char);
	}
	
	if (strcmp("abc\n", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_up() (2): chars=%s\n", lb->chars);
	}
	x_pos = get_cursor_x_pos();
	if (3 != x_pos) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_up() (2): x_pos=%d\n", x_pos);
	}
	
	cursor_down();
	cursor_down();
	lb = get_cursor_line();
	
	if (2 != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_down() (2): cnt_char=%d\n", lb->cnt_char);
	}
	
	if (strcmp("de", lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_down() (2): chars=%s\n", lb->chars);
	}
	x_pos = get_cursor_x_pos();
	if (2 != x_pos) {
		errcnt+=1;
		fprintf(stderr, "Error - cursor_down() (2): x_pos=%d\n", x_pos);
	}
	
	
	return errcnt;
}

// delete_char bug test
int test04() {
	int errcnt = 0;
	int x_pos;
	LINE_BUFFER* lb;

	init_buffer();
	
	new_buffer();
	
	insert_char('a');
	insert_char('b');
	insert_char('c');
	insert_char('d');
	insert_char('e');
	insert_char('f');
	insert_char('g');
	insert_char('h');
	
	cursor_left();
	cursor_left();
	cursor_left();
	
	lb = get_cursor_line();
	
	x_pos = get_cursor_x_pos();
	if (x_pos != 5) {
		errcnt+=1;
		fprintf(stderr, "Error - test04() get_cursor_x_pos(): x_pos=%d\n", x_pos);
	}
	
	delete_char();
	
	errcnt += assert_test_line(lb, "abcdegh", "delete_char() (3)");
	
	return errcnt;
	
}

// cursor up/down boundary test
int test05() {
	int errcnt=0;
	LINE_BUFFER* lb;
	int rc, x_pos;
	
	init_buffer();
	
	new_buffer();
	
	insert_char('a');
	insert_char('b');
	insert_char('c');
	insert_char('d');
	handle_lf();
	insert_char('e');
	insert_char('f');
	handle_lf();
	insert_char('g');
	insert_char('h');
	insert_char('i');
	
	rc = cursor_down();
	if (rc != 0) {
		errcnt++;
		fprintf(stderr, "Error - test05() 1st cursor_down(): rc=%d\n", rc);
	}
	
	rc = cursor_up();
	rc = cursor_up();
	if (rc != 1) {
		errcnt++;
		fprintf(stderr, "Error - test05() 1st cursor_up(): rc=%d\n", rc);
	}
	
	rc = cursor_up();
	if (rc != 0) {
		errcnt++;
		fprintf(stderr, "Error - test05() 2nd cursor_up(): rc=%d\n", rc);
	}
	
	rc = cursor_down();
	if (rc != 1) {
		errcnt++;
		fprintf(stderr, "Error - test05() 2nd cursor_down(): rc=%d\n", rc);
	}
	
	cursor_up();
	cursor_right(); cursor_right(); cursor_right(); cursor_right();
	cursor_down();
	
	x_pos = get_cursor_x_pos();
	if (x_pos != 2) {
		errcnt++;
		fprintf(stderr, "Error - test05() 3rd cursor_down(): x_pos=%d\n", x_pos);
	}
	
	
	return errcnt;
}

int assert_test_line(LINE_BUFFER* lb, char* exp, char* testmsg) {
	int exp_cnt_char;
	int errcnt = 0;
	
	exp_cnt_char = strlen(exp);
	
	if (exp_cnt_char != lb->cnt_char) {
		errcnt+=1;
		fprintf(stderr, "Error - %s: cnt_char=%d\n", testmsg, lb->cnt_char);
	}
	
	if (strcmp(exp, lb->chars)) {
		errcnt+=1;
		fprintf(stderr, "Error - %s: chars=%s\n", testmsg, lb->chars);
	}
	
	return errcnt;
}


