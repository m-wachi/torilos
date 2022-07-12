/*
 * $Author: m-wachi $
 * $Rev: 205 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trscheme_config.h"
#include "objects.h"
#include "tokenizer.h"
#include "primitive_procedures.h"
#include "debug.h"
#include "io.h"

OBJECT* tokenize_item() {
	SYMBOL* sym;
	char buff[256];
	CELL *cell, *prev_cell, *hd_cell;
	OBJECT* obj;
	char c;
	
	move_to_next_token();
	
	c = io_peek();
	switch (c) {
	case ')':
		return NULL;
	case '(':
		c = io_read_next();
		
		hd_cell = create_cell(NULL, NULL);
		prev_cell = hd_cell;
		while (c != ')') {
			if (!c) {
				return (OBJECT*)create_error("couldn't tokenize.");
			}
			obj = tokenize_item();
			if (obj) {
				prev_cell->p_car = obj;
				prev_cell->p_cdr = (OBJECT*) create_cell(NULL, NULL);
				prev_cell = (CELL*)prev_cell->p_cdr;
			}
			c = io_peek();
		}
		io_read_next();
		return (OBJECT*)hd_cell;
	case '\'': 
		io_read_next();
		move_to_next_token();
		
		obj = tokenize_item();
		
		cell= create_cell(obj, (OBJECT*)create_cell(NULL, NULL));
		sym = create_symbol("quote");
		cell = create_cell((OBJECT*)sym, (OBJECT*)cell);
		return (OBJECT*)cell;
	case '"':
		read_string_token(buff);
		return (OBJECT*)create_string(buff);
	case '#':
		c = io_read_next();
		if (c == 't') {
			io_read_next();
			move_to_next_token();
			
			return (OBJECT*)get_true();
		} 
		else if (c == 'f') {
			io_read_next();
			move_to_next_token();
			
			return (OBJECT*)get_false();
		}
		return (OBJECT*)create_error("couldn't tokenize. unimplemented object starting #");
		
	default:
		return tokenize_symbol_or_number();
	}
	return NULL;
}

OBJECT* tokenize_symbol_or_number() {
	INTEGER* integer;
	REAL* real;
	char buff[128];
	
	read_token(buff);
	
	if (is_integer(buff)) {
		integer = create_integer(atoi(buff));
		return (OBJECT*)integer;
	}
	if (is_real(buff)) {
		//TODO comment-out following.
#ifndef TORILOS
		real = create_real(atof(buff));
#endif
		return (OBJECT*)real;
	}
	return (OBJECT*)create_symbol(buff);
}

void read_token(char* tokenbuf) {
	char* p_tokenbuf;
	char c;
	
	move_to_next_token();
	p_tokenbuf = tokenbuf;
	
	for(c=io_peek(); c != 0 && c != ' ' && c != ')' && c != '(' && c != '\n'; c=io_read_next()) {
		*p_tokenbuf++ = c;
	}
	
	//move_to_next_token();
	*p_tokenbuf = 0;
	
}

int is_integer(char* s) {
	while(*s) {
		if (!is_0to9(*s)) return 0;
		s++;
	}
	return 1;
}

int is_real(char* s) {
	int cnt_period = 0;
	
	while(*s) {
		if (is_0to9(*s)) {
			s++;
			continue;
		}
		if (*s == '.' && cnt_period == 0) {
			s++; cnt_period++;
			continue;
		}
		return 0;
	}
	return 1;
}

int is_0to9(char c) {
	return ('0' <= c && c <= '9');  
}

void read_string_token(char* tokenbuf) {
	char* p_tokenbuf;
	char c;
	
	c = io_read_next();
	
	p_tokenbuf = tokenbuf;
	
	while (c != 0 && c != '"') {
		*p_tokenbuf++ = c;
		c = io_read_next();
	}
	
	*p_tokenbuf = 0;
	if (c == '"')
		io_read_next();

	move_to_next_token();
}

void move_to_next_token() {
	char c;
	
	c = io_peek();
	while (c == ' ' || c == '\n')
		c = io_read_next();
		
}

