#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>

#include "tredit.h"
#include "keyboard.h"
#include "filebuffer.h"
#include "menu.h"

int ioctl(int fd, int cmd, ...);
int main(int argc, char** argv);
void init(int fd);
void readloop(int fd);
int handle_kbdevent(int fd, unsigned char keycode, unsigned char prev_keycode);
int load_file();
int handle_cursor_key(int fd, int keycode, int prev_keycode);
int move_left(int fd);
void redraw_line(int fd, int y);
void redraw_after_current_line(int fd);
void redraw_all(int fd);
void draw_line_chars(int fd, LINE_BUFFER* line);
void clear_end_of_line(int fd);

static int ml_cur_line;
static int ml_mode; // 0: editor, 1: menu

char g_save_filename[256];

/**
 * @brief main() function
 * @param argc
 * @param argv
 * @return 
 */
int main(int argc, char** argv) {
	int rc=0, fd;
	
	puts("tredit start.");
	
	init_buffer();

	rc = open("/dev/tty0", O_RDWR, 0);
	
	if (rc < 0) {
		iprintf("Error - couldn't open file. rc=%d\n", rc);
		exit(0);
	}
	fd = rc;

	// set editor mode
	ioctl(fd, 7, 0);

	init(fd);
	
	new_buffer();
	
	if (argc == 2) {
		//ioctl(fd, 6, 0);
		//close(fd);
		//puts("argc=2");
		//puts(argv[1]);
		//exit(0);
		strcpy(g_save_filename, argv[1]);
		load_file();
		redraw_all(fd);
	}

	readloop(fd);
	
	ioctl(fd, 6, 0);

	close(fd);
	
	//save_to_file();
	
	puts("tredit end.");
	exit(0);
}

void init(int fd) {
	char cmdbuf[4];
	
	init_menu(fd);
	
	memset(g_save_filename, 0, sizeof(g_save_filename));
	strcpy(g_save_filename, "tredit.out");
	ml_mode = 0;
	ml_cur_line = 0;
	
	cmdbuf[0] = 'a';
	cmdbuf[1] = 0;
	cmdbuf[2] = 1;
	write(fd, cmdbuf, 4);
}

void readloop(int fd) {
	char keycode, prev_keycode=0;
	int rc;
	
	for (;;) {
		read(fd, &keycode, 1);
		if (ml_mode) { 
			rc = menu_handle_kbdevent(fd, keycode);
			if (1 == rc) {
				ml_mode = 0;
				redraw_all(fd);
				//move_cursor_absolute(fd, get_cursor_x_pos(), ml_cur_line+1);
			} else if (2 == rc) {
				break;
			}
		}
		else {
			if (handle_kbdevent(fd, keycode, prev_keycode)) break;
		}
		prev_keycode = keycode;
	}
}

int handle_kbdevent(int fd, unsigned char keycode, unsigned char prev_keycode) {
	unsigned char chrcode;
	
	if (check_additional_key(keycode)) return 0;
	
	if (handle_cursor_key(fd, keycode, prev_keycode)) return 0;
	
	if (0x53 == keycode) { //delete key
		delete_char();
		clear_end_of_line(fd);
		redraw_line(fd, ml_cur_line);
		return 0;
	}
	else if (0x0e == keycode) { //backspace key
		if (move_left(fd)) {
			delete_char();
			clear_end_of_line(fd);
			redraw_line(fd, ml_cur_line);
		}
		return 0;
	} else if (0x38 == keycode) { // left alt key
		draw_file_menu(fd);
		ml_mode = 1;
		move_cursor_absolute(fd, get_cursor_x_pos(), ml_cur_line+1);
	}

	
	chrcode = kbdkey2char(keycode);
	if (chrcode == 'd' && 1 == ctrl_key_sts()) {
		return 1;
	}
	else if (chrcode) {
		if (0x0a == chrcode) {	// '\n'
			handle_lf();
			clear_line(fd, ml_cur_line+1);
			cursor_up();
			redraw_line(fd, ml_cur_line);
			cursor_down();
			move_cursor_absolute(fd, 0, ++ml_cur_line+1);
			redraw_after_current_line(fd);
		} else {
			
			insert_char(chrcode);
			redraw_line(fd, ml_cur_line);
		}
	}
	return 0;
}

/**
 * @brief save to file
 * @return 
 */
int save_to_file() {
	LINE_BUFFER* line;
	int fd;
	
	fd = open(g_save_filename, O_CREAT, 0);
	if (fd < 0) {
		iprintf("Error - couldn't open file. fd=%d\n", fd);
		exit(0);
	}
	
	line = get_first_line();
	while(line) {
		write(fd, line->chars, line->cnt_char);
		line = get_next_line(line);
	}
	
	close(fd);
	
	return 0;
}

int load_file() {
	int fd;
	LINE_BUFFER *line;
	char *p_char;
	
	fd = open(g_save_filename, O_RDONLY, 0);
	if (fd < 0) {
		iprintf("Error - couldn't open file. fd=%d\n", fd);
		exit(0);
	}
	
	line = get_first_line();
	p_char = line->chars;
	
	while(read(fd, p_char, 1)) {
		if (*p_char == '\n') {
			*(p_char+1) = 0;
			line->cnt_char = strlen(line->chars);
			line = insert_line(line->next);
			p_char = line->chars;
		} else {
			p_char++;
		}
	}
	
	close(fd);
	
	return 0;
	
}


/**
 * @brief handler for cursor key
 * @param fd fd of tty
 * @param keycode 
 * @param prev_keycode
 * @return 0: do nothing, 1: handle cursor-key
 */
int handle_cursor_key(int fd, int keycode, int prev_keycode) {
	char cmdbuf[4];
	int x_pos;
	
	if (0xe0 == prev_keycode) {
		switch (keycode) {
		case 0x48:	// up allow
			if (cursor_up()) {
				if (ml_cur_line > 0) ml_cur_line--;
				x_pos = get_cursor_x_pos();
				move_cursor_absolute(fd, x_pos, ml_cur_line+1);
			}
			return 1;
		case 0x4b:	// left allow
			move_left(fd);
			return 1;
		case 0x4d:	// right allow
			if (cursor_right()) {
				cmdbuf[0] = 'r';
				cmdbuf[1] = 1;
				cmdbuf[2] = 0;
				write(fd, cmdbuf, 4);
			}
			return 1;
		case 0x50:	// down allow
			if (cursor_down()) {
				ml_cur_line++;
				x_pos = get_cursor_x_pos();
				move_cursor_absolute(fd, x_pos, ml_cur_line+1);
			}
			return 1;
		}
	}
	return 0;
}

int move_left(int fd) {
	int rc = 0;
	char cmdbuf[4];
	
	if (cursor_left()) {
		cmdbuf[0] = 'r';
		cmdbuf[1] = -1;
		cmdbuf[2] = 0;
		write(fd, cmdbuf, 4);
		rc = 1;
	}
	return rc;
}

/**
 * @brief redraw line
 * @param fd fd of tty
 * @param line_no (not screen y-pos, file-line)
 */
void redraw_line(int fd, int line_no) {
	LINE_BUFFER* line;
	
	// move line head
	move_cursor_absolute(fd, 0, line_no+1);
	
	line = get_cursor_line();
	
	draw_line_chars(fd, line);
	// move back to current position
	move_cursor_absolute(fd, get_cursor_x_pos(), line_no + 1);
}

/**
 * @brief redraw after current line
 * @param fd fd of tty
 */
void redraw_after_current_line(int fd) {
	LINE_BUFFER* line;
	int y;
	
	y = ml_cur_line;
	
	line = get_cursor_line();
	while(line) {
		clear_line(fd, y+1);
		// move line head
		move_cursor_absolute(fd, 0, y+1);
		
		draw_line_chars(fd, line);
		line = get_next_line(line);
		y++;
	}
	// move back to current position
	move_cursor_absolute(fd, get_cursor_x_pos(), ml_cur_line + 1);
}

/**
 * @brief refresh all lines
 * @param fd fd of tty
 */
void redraw_all(int fd) {
	LINE_BUFFER* line;
	int y;

	y = 0;

	line = get_first_line();
	while(line) {
		clear_line(fd, y+1);
		// move line head
		move_cursor_absolute(fd, 0, y+1);
		
		draw_line_chars(fd, line);
		line = get_next_line(line);
		y++;
	}
	// move back to current position
	move_cursor_absolute(fd, get_cursor_x_pos(), ml_cur_line + 1);
	
}


/**
 * @brief draw characters of LINE_BUFFER
 * @param fd fd of tty
 * @param line 
 */
void draw_line_chars(int fd, LINE_BUFFER* line) {
	char cmdbuf[4];
	int i;
	for (i=0; i<line->cnt_char; i++) {
		cmdbuf[0] = 'p';
		cmdbuf[1] = line->chars[i];
		if (cmdbuf[1] == '\n') cmdbuf[1] = ' ';
		cmdbuf[2] = COL8_YELLOW;
		cmdbuf[3] = COL8_DARKBLUE;
		write(fd, cmdbuf, 4);
	}
}


/**
 * @breif clear one character at the end of line
 * @param fd fd of tty
 */
void clear_end_of_line(int fd) {
	char cmdbuf[4];
	LINE_BUFFER* line;
	
	line = get_cursor_line();
	
	cmdbuf[0] = 'a';
	cmdbuf[1] = line->cnt_char;
	cmdbuf[2] = ml_cur_line + 1;
	write(fd, cmdbuf, 4);
	
	cmdbuf[0] = 'p';
	cmdbuf[1] = ' ';
	cmdbuf[2] = COL8_YELLOW;
	cmdbuf[3] = COL8_DARKBLUE;
	write(fd, cmdbuf, 4);
	
	cmdbuf[0] = 'a';
	cmdbuf[1] = get_cursor_x_pos();
	cmdbuf[2] = ml_cur_line + 1;
	write(fd, cmdbuf, 4);

}

void clear_line(int fd, int y) {
	char cmdbuf[4];
	
	cmdbuf[0] = 'c';
	cmdbuf[1] = y;
	cmdbuf[2] = 0;
	cmdbuf[3] = 0;
	write(fd, cmdbuf, 4);

}
/**
 * @brief move cursor by absolute positioning
 * @param fd fd of tty
 * @param x x-pos
 * @param y y-pos
 */
void move_cursor_absolute(int fd, int x, int y) {
	char cmdbuf[4];
	
	cmdbuf[0] = 'a';
	cmdbuf[1] = x;
	cmdbuf[2] = y;
	write(fd, cmdbuf, 4);
}

