/*
 * $Author: m-wachi $
 * $Rev: 196 $
 */
#ifndef DEBUG_H
#define DEBUG_H
#include "objects.h"

void print_object(OBJECT* obj, int indent);
void print_cell(CELL* cell, int indent);
void print_symbol(SYMBOL* sym);
void print_number(INTEGER* num);
void print_string(STRING* str);
void print_bool(BOOLEAN* bool);
void print_procedure(PROCEDURE* proc);
void print_primitive_procedure(PRIMITIVE_PROCEDURE* prim_proc);
void print_error(ERROR* err);
void print_env(FRAME* env);

#endif
