#include "keyboard.h"

static int ml_key_shift = 0;		
static int ml_key_ctrl = 0;

static char ml_keytable0[0x80] = {
	0x0,  0,  '1', '2', '3', '4', '5', '6',	//0x00
	'7', '8', '9', '0', '-', '^', 0x08, 0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',	//0x10
	'o', 'p', '@', '[', 0x0a, 0,  'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	//0x20
	':',  0,   0,  ']', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0,   '*', 	//0x30
	0x0, ' ',  0,   0,   0,   0,  0,    0,
	0x0,  0,   0,   0,   0,   0,  0,   '7',	//0x40
	'8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0x0,   0,   0,  0,	//0x50
	0,   0,   0,   0,   0,   0,   0,   0,
	0x0,  0,   0,   0,   0,   0,   0,   0,	//0x60
	0,   0,   0,   0,   0,   0,   0,   0,
	0,    0,   0,  0x5c, 0,   0,   0,   0,	//0x70
	0x0,  0,   0,   0,   0, 0x5c,  0,   0
};
static char ml_keytable1[0x80] = {
	0,    0,  '!',0x22, '#', '$', '%', '&',
	0x27,'(', ')', 0,   '=', '~',  0,   0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
	'O', 'P', '`', '{', 0x0a, 0,  'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', 
	'*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?',  0,  '*', 
	0x0, ' ',  0,   0,   0,   0,   0,   0,
	0x0,  0,   0,   0,   0,   0,   0,  '7',
	'8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0x0,  0,   0,   0,
	0x0,  0,   0,   0,   0,   0,   0,   0,
	0x0,  0,   0,   0,   0,   0,   0,   0,
	0x0,  0,   0,   0,   0,   0,   0,   0,
	0x0,  0,   0,   '_', 0,   0,   0,   0,
	0x0,  0,   0,   0,   0,  '|',  0,   0
};


int ctrl_key_sts() {
	return ml_key_ctrl;
}

int check_additional_key(unsigned char keycode) {
	
	switch (keycode) {
	case 0x2a:
		ml_key_shift = 1; return 1;
	case 0xaa:
		ml_key_shift = 0; return 1;
	case 0x1d:
		ml_key_ctrl = 1; return 1;
	case 0x9d:
		ml_key_ctrl = 0; return 1;
	}
	return 0;
}


unsigned char kbdkey2char(unsigned char keycode) {

	if (0x80 > keycode) {
		if (ml_key_shift)
			return ml_keytable1[keycode];
		return ml_keytable0[keycode];
		
	}
	return 0;

}

