#ifndef TREDIT_H
#define TREDIT_H

#define COL8_BLACK        0
#define COL8_RED          1
#define COL8_GREEN        2
#define COL8_YELLOW       3
#define COL8_BLUE         4
#define COL8_SKYBLUE      6
#define COL8_WHITE        7
#define COL8_DARKGREEN    10
#define COL8_DARKBLUE     12
#define COL8_DARK_SKYBLUE 14

int save_to_file();
void move_cursor_absolute(int fd, int x, int y);
void clear_line(int fd, int y);

#endif
