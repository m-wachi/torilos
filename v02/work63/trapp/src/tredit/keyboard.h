#ifndef KEYBOARD_H
#define KEYBOARD_H

int ctrl_key_sts();
int check_additional_key(unsigned char keycode);
unsigned char kbdkey2char(unsigned char keycode);

#endif
