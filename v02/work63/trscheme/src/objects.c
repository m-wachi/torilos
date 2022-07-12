/*
 * $Author: m-wachi $
 * $Rev: 196 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trscheme_config.h"
#include "objects.h"

static BOOLEAN ml_true, ml_false;

void objects_init() {
	ml_true.type = OBJTYPE_BOOL;
	ml_true.value = 1;
	ml_false.type = OBJTYPE_BOOL;
	ml_false.value = 0;
}

CELL* create_cell(OBJECT* obj1, OBJECT* obj2) {
	CELL* cell;
	cell = (CELL*)malloc(sizeof(CELL));
	cell->type = OBJTYPE_CELL;
	cell->p_car = obj1;
	cell->p_cdr = obj2;
	return cell;
}

SYMBOL* create_symbol(char* symbol) {
	SYMBOL* sym;
	sym = (SYMBOL*)malloc(sizeof(SYMBOL));
	sym->type = OBJTYPE_SYMBOL;
	sym->symbol = malloc(strlen(symbol));
	strcpy(sym->symbol, symbol);
	return sym;
}

INTEGER* create_integer(int value) {
	INTEGER* num;
	num = (INTEGER*)malloc(sizeof(INTEGER));
	num->type = OBJTYPE_INTEGER;
	num->value = value;
	return num;
}

REAL* create_real(double value) {
	REAL* real;
	real = (REAL*)malloc(sizeof(REAL));
	real->type = OBJTYPE_REAL;
	real->value = value;
	return real;
}

STRING* create_string(char* s) {
	STRING* str;
	str = (STRING*)malloc(sizeof(STRING));
	str->type = OBJTYPE_STRING;
	str->p_str = malloc(strlen(s)+1);
	strcpy(str->p_str, s);
	return str;
}

ERROR* create_error(char* msg) {
	ERROR* error;
	
	error = malloc(sizeof(ERROR));
	error->type = OBJTYPE_ERROR;
	error->msg = malloc(strlen(msg)+1);
	strcpy(error->msg, msg);
	
	return error;
}

PROCEDURE* create_procedure(CELL* parameters, CELL* body, FRAME* env) {
	PROCEDURE* proc;
	
	proc = malloc(sizeof(PROCEDURE));
	proc->type = OBJTYPE_PROCEDURE;
	proc->parameters = parameters;
	proc->body = body;
	proc->env = env;
	
	return proc;
}


PRIMITIVE_PROCEDURE* create_primitive_procedure(OBJECT* (*c_function)(CELL*)) {
	PRIMITIVE_PROCEDURE* prim_proc;
	prim_proc = malloc(sizeof(PRIMITIVE_PROCEDURE));
	prim_proc->type = OBJTYPE_PRIMITIVE_PROCEDURE;
	prim_proc->c_function = c_function;
	return prim_proc;
}

ENV_ENTRY* create_env_entry(char* symbol, OBJECT* value) {
	ENV_ENTRY* env_entry;
	
	env_entry = malloc(sizeof(ENV_ENTRY));
	env_entry->next = NULL;
	env_entry->symbol = malloc(strlen(symbol));
	strcpy(env_entry->symbol, symbol);
	env_entry->value = value;
	return env_entry;
}


FRAME* create_frame() {
	FRAME* env;
	env = malloc(sizeof(FRAME));
	env->base_env = NULL;
	env->first_entry = NULL;
	return env;
}

BOOLEAN* get_true() {
	return &ml_true; 
}

BOOLEAN* get_false() {
	return &ml_false; 
}


