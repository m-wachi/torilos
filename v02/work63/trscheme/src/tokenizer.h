/*
 * $Author: m-wachi $
 * $Rev: 196 $
 */
#ifndef PARSE_H
#define PARSE_H

#include "objects.h"

typedef struct hnd_tokenizer {
	char *p;
}HND_TOKENIZER;


OBJECT* tokenize_item_old(HND_TOKENIZER* hnd_tknz);
void read_token(char* tokenbuf);
OBJECT* tokenize_item();
OBJECT* tokenize_symbol_or_number();
int is_integer(char* s);
int is_real(char* s);
int is_0to9(char c);
void read_string_token(char* tokenbuf);
void move_to_next_token();

#endif
