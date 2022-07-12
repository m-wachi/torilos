/*
 * $Author: m-wachi $
 * $Rev: 196 $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trscheme_config.h"
#include "objects.h"
#include "analyze.h"
#include "primitive_procedures.h"

void analyze_list(CELL* list) {
	OBJECT* obj;
	SYMBOL* sym;
	OBJECT* (*c_func)(CELL*);
	
	obj = list->p_car;

	switch (obj->type) {
	case OBJTYPE_SYMBOL:
		sym = (SYMBOL*) obj;
		if (!(c_func = lookup_primitive_procedure(sym->symbol))) {
			obj = (OBJECT*)create_primitive_procedure(c_func);
			list->p_car = obj;
			free(sym);
		}

	}
}

