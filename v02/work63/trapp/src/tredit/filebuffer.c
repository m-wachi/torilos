#include "filebuffer.h"
#include <string.h>
#include <stdlib.h>

static LINE_BUFFER ml_linebuffer_hd;
static BUFFER_CURSOR ml_cursor;

void init_buffer() {
	ml_linebuffer_hd.next = &ml_linebuffer_hd;
	ml_linebuffer_hd.prev = &ml_linebuffer_hd;
}


/**
 * @breif insert line
 * @param line the line which will be followed after new line. 
 * @return new line buffer
 */
LINE_BUFFER* insert_line(LINE_BUFFER* line) {
	LINE_BUFFER* new_line;
	LINE_BUFFER* prev_line;
	new_line = new_line_buffer();
	
	prev_line = line->prev;
	
	new_line->prev = prev_line;
	new_line->next = prev_line->next;
	prev_line->next->prev = new_line;
	prev_line->next = new_line;
	
	return new_line;
}

// for new line
LINE_BUFFER* new_line_buffer() {
	LINE_BUFFER* new_line;
	
	new_line = malloc(sizeof(LINE_BUFFER));
	new_line->chars[0] = 0;
	new_line->cnt_char = 0;
	return new_line;
}

// for creating new file.
void new_buffer() {
	LINE_BUFFER* new_line;
	new_line = insert_line(&ml_linebuffer_hd);
	ml_cursor.cur_line = new_line;
	ml_cursor.x_pos = 0;
}

int insert_char(char c) {
	int i;
	LINE_BUFFER* cur_lb;
	cur_lb = ml_cursor.cur_line;
	
	if (MAX_LINE_CHARS <= cur_lb->cnt_char)
		return -1;
		
	cur_lb->cnt_char++;
	cur_lb->chars[cur_lb->cnt_char] = 0;
	for (i=cur_lb->cnt_char; ml_cursor.x_pos<i; i--) {
		cur_lb->chars[i] = cur_lb->chars[i-1];
	}
	cur_lb->chars[ml_cursor.x_pos++] = c;
	return 0;
}

/**
 * @brief handle '\n'
 */
LINE_BUFFER* handle_lf() {
	LINE_BUFFER *cur_line, *new_line;
	char* p_char;
	
	insert_char('\n');
	
	cur_line = ml_cursor.cur_line;
	
	new_line = insert_line(cur_line->next);
	
	p_char = ml_cursor.cur_line->chars;
	
	p_char += ml_cursor.x_pos;
	
	strcpy(new_line->chars, p_char);
	
	cur_line->cnt_char = ml_cursor.x_pos;
	
	cur_line->chars[ml_cursor.x_pos] = 0;
	
	new_line->cnt_char = strlen(new_line->chars);
	
	ml_cursor.cur_line = new_line;
	ml_cursor.x_pos = 0;
	return new_line;
}

LINE_BUFFER* get_first_line() {
	return get_next_line(&ml_linebuffer_hd);
}


LINE_BUFFER* get_next_line(LINE_BUFFER* lb) {
	LINE_BUFFER* next_lb;
	
	next_lb = lb->next;
	
	if (next_lb == &ml_linebuffer_hd) {
		return NULL;
	}
	
	return next_lb;
}

/**
 * @brief move cursor left
 * @return 1: move success, 0: move failed
 */
int cursor_left() {
	if (ml_cursor.x_pos) {
		ml_cursor.x_pos--;
		return 1;
	}
	return 0;
}

/**
 * @brief move cursor right
 * @return 1: move success, 0: move failed
 */
int cursor_right() {
	char cur_char;
	
	cur_char = ml_cursor.cur_line->chars[ml_cursor.x_pos]; 
	
	//if (0 == cur_char || '\n' == cur_char) return;
	if (0 == cur_char) return 0;
	
	ml_cursor.x_pos++;
	return 1;
}

/**
 * @brief delete character at current cursor position
 */
void delete_char() {
	int i;
	LINE_BUFFER* cur_lb;
	if (!ml_cursor.cur_line->chars[ml_cursor.x_pos]) return;
	
	cur_lb = ml_cursor.cur_line;

	for (i=ml_cursor.x_pos; i<cur_lb->cnt_char; i++) {
		cur_lb->chars[i] = cur_lb->chars[i+1];
	}
	cur_lb->cnt_char--;

}

int get_cursor_x_pos() {
	return ml_cursor.x_pos;
}

LINE_BUFFER* get_cursor_line() {
	return ml_cursor.cur_line;
}

/**
 * @breif move cursor to previous line
 */
int cursor_up() {
	LINE_BUFFER* line;
	
	line = ml_cursor.cur_line->prev; 
	if (line == &ml_linebuffer_hd) return 0;
	
	ml_cursor.cur_line = line;
	
	/*
	if (line->cnt_char - 1 < ml_cursor.x_pos)
		ml_cursor.x_pos = line->cnt_char -1;
	*/
	adjust_x_pos();
	
	return 1;
}

/**
 * @breif move cursor to next line
 * @return 1: move success, 0: move failed
 */
int cursor_down() {
	LINE_BUFFER* line;
	
	line = ml_cursor.cur_line->next; 
	if (line == &ml_linebuffer_hd) return 0;
	
	ml_cursor.cur_line = line;
	
	adjust_x_pos();
	return 1;
}

void adjust_x_pos() {
	LINE_BUFFER* line;
	
	line = ml_cursor.cur_line;
	
	if (line->cnt_char < ml_cursor.x_pos)
		ml_cursor.x_pos = line->cnt_char;
	
	if (line->cnt_char > 0) {
		if ('\n' == line->chars[ml_cursor.x_pos-1])
			ml_cursor.x_pos--;
	}
}
