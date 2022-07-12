#ifndef FILEBUFFER_H
#define FILEBUFFER_H

#define MAX_LINE_CHARS	255

typedef struct line_buffer {
	char chars[MAX_LINE_CHARS+1];
	int cnt_char;
	struct line_buffer *next, *prev;
	int free;
} LINE_BUFFER;

typedef struct buffer_cursor {
	LINE_BUFFER* cur_line;
	int x_pos;
} BUFFER_CURSOR;

void init_buffer();
LINE_BUFFER* insert_line(LINE_BUFFER* line);
LINE_BUFFER* new_line_buffer();
void new_buffer();
int insert_char(char c);
LINE_BUFFER* handle_lf();
LINE_BUFFER* get_first_line();
LINE_BUFFER* get_next_line(LINE_BUFFER* lb);
int cursor_left();
int cursor_right();
void delete_char();
int get_cursor_x_pos();
LINE_BUFFER* get_cursor_line();
int cursor_up();
int cursor_down();
void adjust_x_pos();

#endif
