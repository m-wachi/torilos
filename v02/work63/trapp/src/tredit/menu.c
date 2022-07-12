#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>

#include "tredit.h"
#include "keyboard.h"
#include "menu.h"

extern char g_save_filename[256];

static int ml_cur_focus = 0;
static int ml_menu_mode = 0; //0: menu_selection, 1: save_file
static char* ml_p_savefile;

static char ml_save_filename[256];

void init_menu(int fd) {
	move_cursor_absolute(fd, 0, 0);
	draw_string_for_menu(fd, " File  Help ", 0);
}

/**
 * @brief draw character for menu
 * @param fd fd of tty
 * @param c character
 * @param sw 0: normal, 1: focus
 */
void draw_char_for_menu(int fd, char c, int sw) {
	char cmdbuf[4];
	
	cmdbuf[0] = 'p';
	cmdbuf[1] = c;
	if (sw) {
		cmdbuf[2] = COL8_WHITE;
		cmdbuf[3] = COL8_BLUE;
	} else {
		cmdbuf[2] = COL8_DARKBLUE;
		cmdbuf[3] = COL8_SKYBLUE;
	}
	write(fd, cmdbuf, 4);
}

void draw_string_for_menu(int fd, char* str, int sw) {
	int i;
	
	for (i=0; i<strlen(str); i++) {
		draw_char_for_menu(fd, str[i], sw);
	}
	
}

void draw_file_menu(int fd) {
	
	move_cursor_absolute(fd, 0, 0);
	
	if (ml_cur_focus == 0)
		draw_string_for_menu(fd, " File ", 1);
	else 
		draw_string_for_menu(fd, " File ", 0);
	
	move_cursor_absolute(fd, 0, 1);
	if (ml_cur_focus == 1)
		draw_string_for_menu(fd, " Save ", 1);
	else
		draw_string_for_menu(fd, " Save ", 0);
	
	move_cursor_absolute(fd, 0, 2);
	if (ml_cur_focus == 2)
		draw_string_for_menu(fd, " Exit ", 1);
	else
		draw_string_for_menu(fd, " Exit ", 0);
	
}

void menu_dialog_cursor_left(int fd) {
	char cmdbuf[4];
	
	cmdbuf[0] = 'r';
	cmdbuf[1] = -1;
	cmdbuf[2] = 0;
	write(fd, cmdbuf, 4);

}

/**
 * @brief keyboard event handler for menu
 * @param fd fd of tty
 * @param keycode keycode
 * @return 1: end menu mode, 2: end app, 0: others
 */ 
int menu_handle_kbdevent(int fd, unsigned char keycode) {
	
	if (ml_menu_mode == 1) {
		return menu_handle_kbd_for_save(fd, keycode);
	}
	
	switch (keycode) {
	case 0x01: //ESC key
		return 1;
	case 0x1c: //return key
		if (ml_cur_focus == 2) return 2;
		if (ml_cur_focus == 1) {
			menu_show_filesave_dialog(fd);
		}
		break;
	case 0x48: // down cursor
		ml_cur_focus--;
		draw_file_menu(fd);
		
		break;
	case 0x50: // down cursor
		ml_cur_focus++;
		draw_file_menu(fd);
		break;
	}
	return 0;
}

void menu_show_filesave_dialog(int fd) {
	char menu_buff[256];
	
	memset(ml_save_filename, 0, sizeof(ml_save_filename));
	strcpy(ml_save_filename, g_save_filename);
	ml_p_savefile = ml_save_filename;
	ml_p_savefile += strlen(ml_save_filename);
	
	move_cursor_absolute(fd, 10, 5);

	draw_string_for_menu(fd, "  type save filename    ", 0);

	strcpy(menu_buff, "                    ");
	strncpy(menu_buff, ml_save_filename, strlen(ml_save_filename));
	
	move_cursor_absolute(fd, 10, 6);
	draw_string_for_menu(fd, "  ", 0);
	//draw_string_for_menu(fd,   "                    ", 1);
	draw_string_for_menu(fd, menu_buff, 1);
	draw_string_for_menu(fd,                       "  ", 0);
	
	move_cursor_absolute(fd, 10, 7);
	draw_string_for_menu(fd, "                        ", 0);
	
	move_cursor_absolute(fd, 12  + strlen(ml_save_filename), 6);

	ml_menu_mode = 1;
}

void menu_clear_filesave_dialog(int fd) {
	clear_line(fd, 5);
	clear_line(fd, 6);
	clear_line(fd, 7);
}

/**
 * @brief keyboard handler for save dialog mode
 * @return 1: end save mode, 0: others
 */
int menu_handle_kbd_for_save(int fd, unsigned char keycode) {
	unsigned char chrcode;
	
	if (keycode == 0x1c)	{   // return key
		strcpy(g_save_filename, ml_save_filename);
		save_to_file();
		menu_clear_filesave_dialog(fd);
		ml_cur_focus = 0;
		ml_menu_mode = 0;
		return 1;
	} else if (keycode == 0x0e) {// backspace key
		if (ml_p_savefile != ml_save_filename) {
			ml_p_savefile--;
			*ml_p_savefile = 0;
			menu_dialog_cursor_left(fd);
			draw_char_for_menu(fd, ' ', 1);
			menu_dialog_cursor_left(fd);
		}
		return 0;
	}
	
	chrcode = kbdkey2char(keycode);
	if (chrcode) {
		draw_char_for_menu(fd, chrcode, 1);
		*ml_p_savefile = chrcode;
		//ml_save_filename[0] = chrcode;
		//*ml_save_filename = chrcode;
		ml_p_savefile++;
	}
	return 0;
}
