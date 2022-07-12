/*
 * $Author: m-wachi $
 * $Rev: 196 $
 */
#ifndef EVAL_H
#define EVAL_H


OBJECT* eval(OBJECT* exp, FRAME* env);
OBJECT* eval_if(OBJECT* exp, FRAME* env);
OBJECT* eval_assignment(OBJECT* exp, FRAME* env);
OBJECT* eval_definition(OBJECT* exp, FRAME* env);
OBJECT* eval_sequence(OBJECT* exps, FRAME* env);
OBJECT* list_of_values(CELL* exp, FRAME* env);
//int is_last_exp(OBJECT* obj);
int is_quoted(OBJECT* exp);
int is_tagged_list(OBJECT* exp, char* tag);
int is_self_evaluating(OBJECT* exp);
int is_variable(OBJECT* exp);
int is_assignment(OBJECT* exp);
int is_definition(OBJECT* exp);
int is_if(OBJECT* exp);
int is_lambda(OBJECT* exp);
int is_begin(OBJECT* exp);
int is_application(OBJECT* exp);
OBJECT* apply(OBJECT* procedure, CELL* arguments);
CELL* operands(OBJECT* exp);
OBJECT* operator(OBJECT* exp);
OBJECT* lambda_parameters(OBJECT* exp);
OBJECT* lambda_body(OBJECT* exp);
OBJECT* make_lambda(OBJECT* parameters, OBJECT* body);
OBJECT* make_procedure(OBJECT* parameters, OBJECT* body, FRAME* env);
int is_compound_procedure(OBJECT* exp);
OBJECT* procedure_parameters(PROCEDURE* procedure);
OBJECT* procedure_body(PROCEDURE* procedure);
FRAME* procedure_environment(PROCEDURE* procedure);
SYMBOL* assignment_variable(OBJECT* exp);
OBJECT* assignment_value(OBJECT* exp);
SYMBOL* definition_variable(OBJECT* exp);
OBJECT* definition_value(OBJECT* exp);
OBJECT* begin_actions(OBJECT* exp);
OBJECT* text_of_quotation(OBJECT* exp);
OBJECT* if_predicate(OBJECT* exp);
OBJECT* if_consequent(OBJECT* exp);
OBJECT* if_alternative(OBJECT* exp);
#endif
