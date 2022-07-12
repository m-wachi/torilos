#ifndef MENU_H
#define MENU_H

void init_menu(int fd);
void draw_char_for_menu(int fd, char c, int sw);
void draw_string_for_menu(int fd, char* str, int sw);
void draw_file_menu(int fd);
void menu_dialog_cursor_left(int fd);
int menu_handle_kbdevent(int fd, unsigned char keycode);
void menu_show_filesave_dialog(int fd);
void menu_clear_filesave_dialog(int fd);
int menu_handle_kbd_for_save(int fd, unsigned char keycode);

#endif
