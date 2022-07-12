/*
 * $Author: m-wachi $
 * $Rev: 210 $
 */
#ifndef PRIMITIVE_PROCEDURES_H
#define PRIMITIVE_PROCEDURES_H

#include "objects.h"

typedef struct prim_proc_map_item_t {
	char* proc_name;
	OBJECT* (*c_function)(CELL* lst_param);
} PRIM_PROC_MAP_ITEM;

typedef struct prim_proc_map {
	int item_count;
	PRIM_PROC_MAP_ITEM tbl[100];
} PRIM_PROC_MAP;

void setup_prim_proc_env(FRAME* env);
void add_prim_proc(FRAME* env, char* proc_name, OBJECT* (*c_function)(CELL*));
int is_primitive_procedure_name(char* proc_name);
OBJECT* (*lookup_primitive_procedure(char* proc_name))(CELL*);
int is_primitive_procedure(OBJECT* procedure);
int is_primitive_procedure(OBJECT* procedure);
OBJECT* apply_primitive_procedure(OBJECT* procedure, CELL* arguments);
OBJECT* plus_impl(CELL* lst_param);
OBJECT* minus_impl(CELL* lst_param);
OBJECT* mul_impl(CELL* lst_param);
OBJECT* div_impl(CELL* lst_param);
OBJECT* calc_int(CELL* lst_param, int init, int calc_type);
int is_pair(OBJECT* obj);
OBJECT* is_null_impl(CELL* lst_param);
int is_null(CELL* cell);
OBJECT* is_equal_number_impl(CELL* lst_param);
int equal_comparator(int num1, int num2);
OBJECT* is_less_than_impl(CELL* lst_param);
int less_than_comparator(int num1, int num2);
OBJECT* is_greater_than_impl(CELL* lst_param);
int greater_than_comparator(int num1, int num2);
OBJECT* is_greater_equal_impl(CELL* lst_param);
int greater_equal_comparator(int num1, int num2);
OBJECT* is_less_equal_impl(CELL* lst_param);
int less_equal_comparator(int num1, int num2);
OBJECT* comparator_impl(CELL* lst_param, int (comparator_func)(int, int));
OBJECT* cons_impl(CELL* lst_param);
CELL* cons(OBJECT* obj1, OBJECT* obj2);
OBJECT* car_impl(CELL* lst_param);
OBJECT* car(OBJECT* obj);
OBJECT* cdr_impl(CELL* lst_param);
OBJECT* cdr(OBJECT* obj);
OBJECT* cadr(OBJECT* obj);
OBJECT* cddr(OBJECT* obj);
OBJECT* is_eq_impl(CELL* lst_param);
int is_eq(OBJECT* obj1, OBJECT* obj2);

#endif
