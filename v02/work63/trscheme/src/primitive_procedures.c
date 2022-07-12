/*
 * $Author: m-wachi $
 * $Rev: 210 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trscheme_config.h"
#include "objects.h"
#include "tokenizer.h"
#include "primitive_procedures.h"
#include "env.h"
#include "eval.h"

extern BOOLEAN g_true, g_false;

static PRIM_PROC_MAP ml_prim_proc_map;

void setup_prim_proc_env(FRAME* env) {
	SYMBOL* prim_proc_tag;
	
	prim_proc_tag = create_symbol("primitive");
	
	add_prim_proc(env, "null?", is_null_impl);
	add_prim_proc(env, "+", plus_impl);
	add_prim_proc(env, "-", minus_impl);
	add_prim_proc(env, "*", mul_impl);
	add_prim_proc(env, "/", div_impl);
	add_prim_proc(env, "=", is_equal_number_impl);
	add_prim_proc(env, "<", is_less_than_impl);
	add_prim_proc(env, ">", is_greater_than_impl);
	add_prim_proc(env, ">=", is_greater_equal_impl);
	add_prim_proc(env, "<=", is_less_equal_impl);
	add_prim_proc(env, "cons", cons_impl);
	add_prim_proc(env, "car", car_impl);
	add_prim_proc(env, "cdr", cdr_impl);
	add_prim_proc(env, "eq?", is_eq_impl);
}

void add_prim_proc(FRAME* env, char* proc_name, OBJECT* (*c_function)(CELL*)) {
	SYMBOL* prim_proc_tag;
	PRIMITIVE_PROCEDURE* prim_proc;
	CELL* prim_proc_list;
	
	prim_proc_tag = create_symbol("primitive");
	
	prim_proc = create_primitive_procedure(c_function);
	prim_proc_list = cons((OBJECT*)prim_proc_tag, (OBJECT*)cons((OBJECT*)prim_proc, NULL));  
	add_bindings_to_frame(env, proc_name, (OBJECT*)prim_proc_list);
	
}


int is_primitive_procedure_name(char* proc_name) {
	
	if (lookup_primitive_procedure(proc_name))
		return 1;
	return 0;
	
}

OBJECT* (*lookup_primitive_procedure(char* proc_name))(CELL*) {
	int i;
	PRIM_PROC_MAP_ITEM* map_item;
	
	for (i=0; i<ml_prim_proc_map.item_count; i++) {
		map_item = &ml_prim_proc_map.tbl[i];
		if (!strcmp(proc_name, map_item->proc_name)) {
			return map_item->c_function;
		}
	}
	return NULL;
}

int is_primitive_procedure(OBJECT* exp) {
	return is_tagged_list(exp, "primitive");
}


OBJECT* apply_primitive_procedure(OBJECT* procedure, CELL* arguments) {
	OBJECT* obj;
	PRIMITIVE_PROCEDURE* prim_proc;
	
	obj = cadr(procedure);
	if (obj->type != OBJTYPE_PRIMITIVE_PROCEDURE) {
		return (OBJECT*)create_error("couldn't get primitive-procedure - APPLY-PRIMITIVE-PROCEDURE");
	}
	prim_proc = (PRIMITIVE_PROCEDURE*)obj;
	
	return prim_proc->c_function(arguments);
}


OBJECT* plus_impl(CELL* lst_param) {
	
	return calc_int(lst_param, 0, 0);
}

OBJECT* minus_impl(CELL* lst_param) {
	INTEGER* init_num;
	
	//if (!lst_param->p_cdr) {
	if (is_null((CELL*)cdr((OBJECT*)lst_param))) {
		return calc_int(lst_param, 0, 1);
	}
	
	init_num = (INTEGER*)car((OBJECT*)lst_param);
	return calc_int((CELL*)cdr((OBJECT*)lst_param), init_num->value, 1);
}

OBJECT* mul_impl(CELL* lst_param) {
	return calc_int(lst_param, 1, 2);
}

OBJECT* div_impl(CELL* lst_param) {
	INTEGER* init_num;
	
	if (!lst_param->p_cdr) {
		return calc_int(lst_param, 1, 3);
	}
	
	init_num = (INTEGER*)car((OBJECT*)lst_param);
	return calc_int((CELL*)cdr((OBJECT*)lst_param), init_num->value, 3);
}


/**
 * @brief calculate '+', '-', '*', '/' for integer
 * @param lst_param procedure arguments
 * @param init initial value
 * @param calc_type  0: '+', 1:'-', 2: '*', 3: '/'
 */ 
OBJECT* calc_int(CELL* lst_param, int init, int calc_type) {
	INTEGER* num;
	INTEGER* arg;
	
	num = create_integer(init);
	while(lst_param) {
		if (!(lst_param->p_car || lst_param->p_cdr)) {
			break;
		}
		if (lst_param->p_car->type != OBJTYPE_INTEGER) {
			return (OBJECT*)create_error("+ expects type <number> as arguments.");
		}
		arg = (INTEGER*)lst_param->p_car;
		switch (calc_type) {
		case 0:
			num->value += arg->value; break;
		case 1:
			num->value -= arg->value; break;
		case 2:
			num->value *= arg->value; break;
		case 3:
			num->value /= arg->value; break;
		}
		lst_param = (CELL*)lst_param->p_cdr;
	}
	return (OBJECT*)num;
	
}


int is_pair(OBJECT* obj) {
	return obj->type == OBJTYPE_CELL;
}

OBJECT* is_null_impl(CELL* lst_param) {
	OBJECT* obj;
	CELL* cell;

	if (!is_null((CELL*)cdr((OBJECT*)lst_param))) {
		return (OBJECT*)create_error("expect 1 argument. - NULL?");
	}
	
	obj = lst_param->p_car;

	if (obj->type != OBJTYPE_CELL)
		return (OBJECT*)get_false();
	
	cell = (CELL*)obj;
	if (is_null(cell))
		return (OBJECT*)get_true(); 
	
	return (OBJECT*)get_false();

}

int is_null(CELL* cell) {
	return !(cell->p_car || cell->p_cdr);
}


//'=' procedure
OBJECT* is_equal_number_impl(CELL* lst_param) {
	return comparator_impl(lst_param, equal_comparator);
}

int equal_comparator(int num1, int num2) {
	return num1 == num2;
}

//'<' procedure
OBJECT* is_less_than_impl(CELL* lst_param) {
	return comparator_impl(lst_param, less_than_comparator);
}
int less_than_comparator(int num1, int num2) {
	return num1 < num2;
}


//'>' procedure
OBJECT* is_greater_than_impl(CELL* lst_param) {
	return comparator_impl(lst_param, greater_than_comparator);
}

int greater_than_comparator(int num1, int num2) {
	return num1 > num2;
}


//'>=' procedure
OBJECT* is_greater_equal_impl(CELL* lst_param) {
	return comparator_impl(lst_param, greater_equal_comparator);
}

int greater_equal_comparator(int num1, int num2) {
	return num1 >= num2;
}

//'<=' procedure
OBJECT* is_less_equal_impl(CELL* lst_param) {
	return comparator_impl(lst_param, less_equal_comparator);
}

int less_equal_comparator(int num1, int num2) {
	return num1 <= num2;
}

//comparator('=', '>', '<', '>=') impl
OBJECT* comparator_impl(CELL* lst_param, int (comparator_func)(int, int)) {
	INTEGER *num1, *num2;
	CELL* cdr_cell;
	
	num1 = (INTEGER*)car((OBJECT*)lst_param);
	if (num1->type != OBJTYPE_INTEGER) 
		return (OBJECT*)create_error("1st parameter is not a NUMBER.");
	
	cdr_cell = (CELL*)cdr((OBJECT*)lst_param);
	num2 = (INTEGER*)car((OBJECT*)cdr_cell);
	if (num2->type != OBJTYPE_INTEGER) 
		return (OBJECT*)create_error("2nd parameter is not a NUMBER.");
	
	if (!comparator_func(num1->value,  num2->value)) {
		return (OBJECT*)get_false();
	}
	if (!is_null((CELL*)cdr((OBJECT*)cdr_cell)))
		return comparator_impl(cdr_cell, comparator_func);
	
	return (OBJECT*)get_true();
	
}

OBJECT* cons_impl(CELL* lst_param) {
	OBJECT* obj;
	CELL* cdr_cell;

	obj = car((OBJECT*)lst_param);
	cdr_cell = (CELL*)cdr((OBJECT*)lst_param);
	
	if (is_null(cdr_cell))
		return (OBJECT*)create_error("cons expects 2 arguments");

	if (!is_null((CELL*)cdr((OBJECT*)cdr_cell))) {
		return (OBJECT*)create_error("cons expects 2 arguments");
	}
	return (OBJECT*)cons(obj, car((OBJECT*)cdr_cell));
}

CELL* cons(OBJECT* obj1, OBJECT* obj2) {
	CELL* new_cell;
	
	new_cell = create_cell(obj1, obj2);
	return new_cell;
}

OBJECT* car_impl(CELL* lst_param) {
	OBJECT* obj;
	CELL* cdr_cell;

	obj = car((OBJECT*)lst_param);
	cdr_cell = (CELL*)cdr((OBJECT*)lst_param);
	
	if (!is_null(cdr_cell))
		return (OBJECT*)create_error("car expects 1 arguments.");

	if (obj->type != OBJTYPE_CELL) {
		return (OBJECT*)create_error("car argument must be pair.");
	}
	
	return car(obj);
}

OBJECT* car(OBJECT* obj) {
	return ((CELL*)obj)->p_car;
}


OBJECT* cdr_impl(CELL* lst_param) {
	CELL* obj;
	CELL* cdr_cell;

	obj = (CELL*)car((OBJECT*)lst_param);
	cdr_cell = (CELL*)cdr((OBJECT*)lst_param);
	
	if (!is_null(cdr_cell))
		return (OBJECT*)create_error("cdr expects 1 arguments.");

	if (!obj) {
		return (OBJECT*)create_error("cdr argument must be pair.");
	}
	
	if (obj->type != OBJTYPE_CELL) {
		return (OBJECT*)create_error("car argument must be pair.");
	}
	if (!obj->p_car &&  !obj->p_cdr) {
		return (OBJECT*)create_error("car argument must be pair.");
	}
	
	return cdr((OBJECT*)obj);
}

OBJECT* cdr(OBJECT* obj) {
	return ((CELL*)obj)->p_cdr;
}

OBJECT* cadr(OBJECT* obj) {
	return ((CELL*)((CELL*)obj)->p_cdr)->p_car;
}

OBJECT* cddr(OBJECT* obj) {
	return ((CELL*)((CELL*)obj)->p_cdr)->p_cdr;
}

OBJECT* is_eq_impl(CELL* lst_param) {
	OBJECT *obj1, *obj2;
	CELL* cdr_cell;
	
	obj1 = car((OBJECT*)lst_param);
	
	if (is_null(lst_param)) {
		return (OBJECT*)create_error("eq? expects 2 arguments.");
	}
	
	cdr_cell = (CELL*)cdr((OBJECT*)lst_param);
	obj2 = car((OBJECT*)cdr_cell);

	if (!obj2) {
		return (OBJECT*)create_error("eq? expects 2 arguments.");
	}
	
	if (!is_eq(obj1, obj2))
		return (OBJECT*)get_false();

	if (!is_null((CELL*)cdr((OBJECT*)cdr_cell)))
		return is_eq_impl(cdr_cell);
	
	return (OBJECT*)get_true();

}

int is_eq(OBJECT* obj1, OBJECT* obj2) {
	SYMBOL *sym1, *sym2;
	
	if (obj1 == obj2)
		return 1;
	
	if (obj1->type != obj2->type)
		return 0;
	
	if (obj1->type == OBJTYPE_SYMBOL) {
		sym1 = (SYMBOL*)obj1;
		sym2 = (SYMBOL*)obj2;
		if (strcmp(sym1->symbol, sym2->symbol))
			return 0;
		return 1;
	}
	
	if (obj1->type == OBJTYPE_CELL) {
		if (is_null((CELL*)obj1) && is_null((CELL*)obj2))
			return 1;
	}
	return 0;
}
