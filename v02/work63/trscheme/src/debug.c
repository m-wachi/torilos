/*
 * $Author: m-wachi $
 * $Rev: 204 $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trscheme_config.h"
#include "objects.h"
#include "debug.h"

void print_object(OBJECT* obj, int indent) {
	
	if (!obj) return; 
	
	switch (obj->type) {
	case OBJTYPE_SYMBOL:
		print_symbol((SYMBOL*)obj);
		break;
	case OBJTYPE_CELL:
		print_cell((CELL*)obj, indent);
		break;
	case OBJTYPE_INTEGER:
		print_number((INTEGER*)obj);
		break;
	case OBJTYPE_STRING:
		print_string((STRING*)obj);
		break;
	case OBJTYPE_BOOL:
		print_bool((BOOLEAN*)obj);
		break;
	case OBJTYPE_PROCEDURE:
		print_procedure((PROCEDURE*)obj);
		break;
	case OBJTYPE_PRIMITIVE_PROCEDURE:
		print_primitive_procedure((PRIMITIVE_PROCEDURE*)obj);
		break;
	case OBJTYPE_ERROR:
		print_error((ERROR*)obj);
	}
}

void print_cell(CELL* cell, int indent) {
	int i;
	
	printf("\n");
	for (i=0; i<indent; i++) {
		printf(" ");
	}
	printf("(");
	while(cell) {
		printf(" ");
		print_object(cell->p_car, indent + 4);
		cell = (CELL*)cell->p_cdr;
		if (cell && cell->type != OBJTYPE_CELL) {
			printf(" . ");
			print_object((OBJECT*)cell, indent + 4);
			break;
		}
	}
	printf(")\n");
	for (i=0; i<indent-4; i++) {
		printf(" ");
	}

}

void print_symbol(SYMBOL* sym) {
	printf(sym->symbol);
}

void print_number(INTEGER* num) {
	printf("%d", num->value);
}

void print_string(STRING* str) {
	printf("\"%s\"", str->p_str);
}

void print_bool(BOOLEAN* bool) {
	if (bool->value)
		printf("#t");
	else
		printf("#f");
}

void print_procedure(PROCEDURE* proc) {
	printf("PROCEDURE<%#x>", (int)proc);
}

void print_primitive_procedure(PRIMITIVE_PROCEDURE* prim_proc) {
	printf("PRIMITIVE PROCEDURE<%#x>", (int)prim_proc);
}

void print_error(ERROR* err) {
	printf("%s\n", err->msg);
}

void print_env(FRAME* env) {
	FRAME* frame;
	ENV_ENTRY* env_entry;
	
	int i;
	i = 1;
	for (frame=env; frame; frame=frame->base_env) {
		printf("%dth entries\n", i++);
		for (env_entry=frame->first_entry; env_entry; env_entry=env_entry->next) {
			printf("  %s:\t", env_entry->symbol);
			print_object(env_entry->value, 0);
			puts("");
		}
	}
}

