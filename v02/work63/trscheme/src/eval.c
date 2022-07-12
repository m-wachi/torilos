/*
 * $Author: m-wachi $
 * $Rev: 196 $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trscheme_config.h"
#include "objects.h"
#include "eval.h"
#include "env.h"
#include "primitive_procedures.h"
#include "debug.h"

//int g_indent;

OBJECT* eval(OBJECT* exp, FRAME* env) {
	OBJECT* result;
	CELL* arguments;
	
	if (is_self_evaluating(exp)) {
		return exp;
	} 
	else if (is_variable(exp)) {
		return lookup_variable_value((SYMBOL*)exp, env);
	}
	else if (is_quoted(exp)) {
		return text_of_quotation(exp);
	}
	else if (is_assignment(exp)) {
		return eval_assignment(exp, env);
	}
	else if (is_definition(exp)) {
		eval_definition(exp, env);
	}
	else if (is_if(exp)) {
		return eval_if(exp, env);
	}
	else if (is_lambda(exp)) {
		return make_procedure(lambda_parameters(exp), lambda_body(exp), env);
	}
	else if (is_begin(exp)) {
		return eval_sequence(begin_actions(exp), env);
	}
	else if (is_application(exp)) {
		//g_indent +=4;
		arguments = (CELL*)list_of_values((CELL*)operands(exp), env); 
		result = apply(eval(operator(exp), env), arguments);
		/*
		printf("apply-operator:");
		print_object(operator(exp), g_indent);
		puts("");
		printf("apply-operands:");
		print_object(operands(exp), g_indent);
		puts("");
		printf("apply-arguments:");
		print_object(arguments, g_indent);
		puts("");
		printf("result:");
		print_object(result, g_indent);
		puts("");
		printf("env:");
		print_env(env);
		g_indent -=4;
		*/
		return result;
	}

	return NULL;
}

OBJECT* eval_if(OBJECT* exp, FRAME* env) {
	BOOLEAN* pred_result;
	
	pred_result = (BOOLEAN*)eval(if_predicate(exp), env);
	if (pred_result->type != OBJTYPE_BOOL 
			||	pred_result->value) {
		return eval(if_consequent(exp), env);
	}
	return eval(if_alternative(exp), env);
}

OBJECT* eval_assignment(OBJECT* exp, FRAME* env) {
	return set_variable_value(env, assignment_variable(exp), 
			eval(assignment_value(exp), env));
}

OBJECT* eval_definition(OBJECT* exp, FRAME* env) {
	define_variable(env, definition_variable(exp)->symbol, eval(definition_value(exp), env));
	return NULL;
}

OBJECT* eval_sequence(OBJECT* exps, FRAME* env) {
	CELL* cell;
	OBJECT* ret_obj;
	
	cell = (CELL*)exps;
	
	while (!is_null(cell)) {
		ret_obj = eval(cell->p_car, env);
		cell = (CELL*)cell->p_cdr;
	}
	return ret_obj;
}

OBJECT* list_of_values(CELL* exps, FRAME* env) {
	if (is_null(exps))
		return (OBJECT*)cons(NULL, NULL);

	return (OBJECT*)cons(eval(exps->p_car, env), 
			list_of_values((CELL*)exps->p_cdr, env));
	
}

int is_quoted(OBJECT* exp) {
	return is_tagged_list(exp, "quote");
}

int is_tagged_list(OBJECT* exp, char* tag) {
	CELL* cell;
	SYMBOL* sym;
	
	if (OBJTYPE_CELL != exp->type)
		return 0;
	
	cell = (CELL*) exp;
	if (OBJTYPE_SYMBOL != cell->p_car->type)
		return 0;
	
	sym = (SYMBOL*) cell->p_car;
	if (strcmp(tag, sym->symbol))
		return 0;
	
	return 1;
}

int is_self_evaluating(OBJECT* exp) {
	switch (exp->type) {
	case OBJTYPE_STRING:
	case OBJTYPE_INTEGER:
	case OBJTYPE_BOOL:
		return 1;
	}
	return 0;
}

int is_variable(OBJECT* exp) {
	return exp->type == OBJTYPE_SYMBOL;
}

int is_assignment(OBJECT* exp) {
	
	return is_tagged_list(exp, "set!");
}

int is_definition(OBJECT* exp) {
	return is_tagged_list(exp, "define");
}

int is_if(OBJECT* exp) {
	return is_tagged_list(exp, "if");
}

int is_lambda(OBJECT* exp) {
	
	return is_tagged_list(exp, "lambda");
}

int is_begin(OBJECT* exp) {
	return is_tagged_list(exp, "begin");
}

int is_application(OBJECT* exp) {
	return is_pair(exp);
}

OBJECT* apply(OBJECT* procedure, CELL* arguments) {
	OBJECT* result;
	//printf("apply-procedure:");
	//print_object(procedure, 4);
	//printf("apply-argument:");
	//print_object(arguments, 4);
	
	result = NULL;
	
	if (is_primitive_procedure(procedure))
		result = apply_primitive_procedure(procedure, arguments);
	else if (is_compound_procedure(procedure)) {
		result = eval_sequence(procedure_body((PROCEDURE*)procedure), 
				extend_environment(
						procedure_environment((PROCEDURE*)procedure), 
						(CELL*)procedure_parameters((PROCEDURE*)procedure), 
						arguments));
	}
	if (result) {
		//printf("result:");
		//print_object(result, 4);
		//puts("");
		return result;
	}
	return (OBJECT*)create_error("Unknown procedure type -- APPLY");
}

CELL* operands(OBJECT* exp) {
	return (CELL*)cdr(exp);
}

OBJECT* operator(OBJECT* exp) {
	return car(exp);
}


OBJECT* lambda_parameters(OBJECT* exp) {
	return ((CELL*)((CELL*)exp)->p_cdr)->p_car;
}

OBJECT* lambda_body(OBJECT* exp) {
	return ((CELL*)((CELL*)exp)->p_cdr)->p_cdr;
}

OBJECT* make_lambda(OBJECT* parameters, OBJECT* body) {
	SYMBOL* sym;
	
	sym = create_symbol("lambda");

	return (OBJECT*)cons((OBJECT*)sym, (OBJECT*)cons(parameters, body)); 
}

OBJECT* make_procedure(OBJECT* parameters, OBJECT* body, FRAME* env) {
	return (OBJECT*)create_procedure((CELL*)parameters, (CELL*)body, env); 
}

int is_compound_procedure(OBJECT* exp) {
	return exp->type == OBJTYPE_PROCEDURE;
}

OBJECT* procedure_parameters(PROCEDURE* procedure) {
	return (OBJECT*)procedure->parameters;
}

OBJECT* procedure_body(PROCEDURE* procedure) {
	return (OBJECT*)procedure->body;
}

FRAME* procedure_environment(PROCEDURE* procedure) {
	return procedure->env;
}

SYMBOL* assignment_variable(OBJECT* exp) {
	return (SYMBOL*)cadr(exp);
}

OBJECT* assignment_value(OBJECT* exp) {
	return (OBJECT*)car(cddr(exp));
}

SYMBOL* definition_variable(OBJECT* exp) {
	CELL* cell;
	OBJECT* obj;
	
	cell = (CELL*)exp;
	
	obj = cadr(exp);
	if (OBJTYPE_SYMBOL == obj->type) {
		return (SYMBOL*)obj;
	}
	
	return (SYMBOL*)car(obj);
}

OBJECT* definition_value(OBJECT* exp) {
	if (OBJTYPE_SYMBOL == cadr(exp)->type) {
		return car(cddr(exp));
	}
	return make_lambda(cdr(cadr(exp)), cddr(exp));
	
}

OBJECT* begin_actions(OBJECT* exp) {
	return cdr(exp);
}

OBJECT* text_of_quotation(OBJECT* exp) {
	return cadr(exp);
}

OBJECT* if_predicate(OBJECT* exp) {
	return cadr(exp);
}

OBJECT* if_consequent(OBJECT* exp) {
	return car(cddr(exp));
}

OBJECT* if_alternative(OBJECT* exp) {
	OBJECT* obj;
	if ((obj = cdr(cddr(exp)))) {
		return car(obj);
	}
	return (OBJECT*)get_false();
}


