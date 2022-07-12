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
#include "eval.h"
#include "primitive_procedures.h"
#include "env.h"
#include "debug.h"
#include "test.h"
#include "test_prim_proc.h"
#include "test_io.h"
#include "test_tokenizer.h"

//extern int g_indent;

int test_main() {
	int errcnt = 0;

	errcnt += test_io_main();
	
#ifndef TORILOS	
	errcnt += test_tokenizer_main();
	
	//errcnt += test_tokenizer();
	
	errcnt += test_env();
	
	errcnt += test_assignment();

	errcnt += test_begin();

	errcnt += test_prim_proc_main();
	
	errcnt += test_prim_proc_exec();
	
	errcnt += test_if();
	
	errcnt += test_combine_proc_exec();

	errcnt += test_recursive();
#endif
	
	if (errcnt)
		return -1;
		
	puts("OK - test passed.");
	return 0;
}
/*
int test_tokenizer() {
	int errcnt = 0;
	char test_str1[] = "(+ 2 30)";
	char test_str2[] = "(+ 2 (+ 4 5) 30 (- 3 2))";
	char test_str3[] = "(define var01 \"abcdef\")";
	char* test_str;
	char* p;
	CELL* root;
	CELL *cell1, *cell2;
	OBJECT* obj1;
	SYMBOL* sym1;
	BOOLEAN* bool;
	HND_TOKENIZER hnd_tknz;
	root = create_cell(NULL, NULL);

	p = test_str1;

	hnd_tknz.p = test_str1;
	cell1 = (CELL*)tokenize_item_old(&hnd_tknz);
	if (hnd_tknz.p != test_str1 + 8) {
		fprintf(stderr, "Failed - tokenize_item(): wrong p address(%d)", p - test_str1);
		print_object(cadr((OBJECT*)root), 0);
		errcnt++;
	}
	
	
	
	obj1 = cadr((OBJECT*)cell1);
	if (obj1->type != OBJTYPE_NUMBER) {
		fprintf(stderr, "Failed - tokenize_item(): 2nd element is not NUMBER. type=%d\n", obj1->type);
		print_object(obj1, 0);
		errcnt++;
	}

	cell1 = (CELL*)cddr((OBJECT*)cell1);
	if (cell1->p_cdr != NULL) {
		fprintf(stderr, "Failed - tokenize_item(): 3rd element's cdr is not NULL.\n");
		print_object((OBJECT*)cell1, 0);
		errcnt++;
	}

	hnd_tknz.p = test_str2;
	cell1 = (CELL*)tokenize_item_old(&hnd_tknz);
	//print_object((OBJECT*)cell1, 0);
	cell2 = (CELL*)car(cddr((OBJECT*)cell1));
	if (cell2->type != OBJTYPE_CELL) {
		fprintf(stderr, "Failed - tokenize_item(): 3rd element's car is not list.\n");
		print_object((OBJECT*)cell2, 0);
		errcnt++;
	}
	
	hnd_tknz.p = test_str3;
	cell1 = (CELL*)tokenize_item_old(&hnd_tknz);
	//print_object((OBJECT*)cell1, 0);
	obj1 = car((OBJECT*)cell1);
	if (obj1->type != OBJTYPE_SYMBOL) {
		fprintf(stderr, "Failed - tokenize_item(): 1st element is not SYMBOL.\n");
		print_object(obj1, 0);
		errcnt++;
	}
	sym1 = (SYMBOL*)obj1;
	if (strcmp(sym1->symbol, "define")) {
		fprintf(stderr, "Failed - tokenize_item(): 1st element's symbol is wrong. - %s\n", sym1->symbol);
		errcnt++;
	}
	
	
	test_str = "(1)";
	hnd_tknz.p = test_str;
	cell1 = (CELL*)tokenize_item_old(&hnd_tknz);
	//print_object((OBJECT*)cell1, 0);
	if (cell1->type != OBJTYPE_CELL) {
		fprintf(stderr, "Failed - tokenize_item(%s): return object isn't CELL. type=%d\n",
				test_str, cell1->type);
		errcnt++;
	}
	if (cell1->p_cdr != NULL) {
		fprintf(stderr, "Failed - tokenize_item(%s): not terminated correctly. cdr=%#x\n",
				test_str, (int)cell1->p_cdr);
		errcnt++;
	}
	
	//
	// quote(') test.
	//
	test_str = "'abc";
	hnd_tknz.p = test_str;
	cell1 = (CELL*)tokenize_item_old(&hnd_tknz);
	//print_object((OBJECT*)cell1, 0);
	if (cell1->type != OBJTYPE_CELL) {
		fprintf(stderr, "Failed - tokenize_item(%s): return object isn't CELL. type=%d\n",
				test_str, cell1->type);
		errcnt++;
	}
	sym1 = (SYMBOL*)car((OBJECT*)cell1);
	if (sym1->type != OBJTYPE_SYMBOL || strcmp(sym1->symbol, "quote")) {
		fprintf(stderr, "Failed - tokenize_item(%s): 1st element isn't quote. type=%d, symbol=%s\n",
				test_str, sym1->type, sym1->symbol);
		errcnt++;
	}
	sym1 = (SYMBOL*)cadr((OBJECT*)cell1);
	if (sym1->type != OBJTYPE_SYMBOL || strcmp(sym1->symbol, "abc")) {
		fprintf(stderr, "Failed - tokenize_item(%s): 2nd element isn't abc. type=%d, symbol=%s\n",
				test_str, sym1->type, sym1->symbol);
		errcnt++;
	}
	
	//
	// combine quote and the others
	//
	test_str = "(list 'abc 5 'def)";
	hnd_tknz.p = test_str;
	cell1 = (CELL*)tokenize_item_old(&hnd_tknz);
	//print_object((OBJECT*)cell1, 0);
	obj1 = cadr((OBJECT*)cell1);
	if (obj1->type != OBJTYPE_CELL) {
		fprintf(stderr, "Failed - tokenize_item(%s): 2nd element isn't quoted-string. type=%d\n",
				test_str, obj1->type);
		errcnt++;
	}
	sym1 = (SYMBOL*)car(obj1);
	if (sym1->type != OBJTYPE_SYMBOL || strcmp(sym1->symbol, "quote")) {
		fprintf(stderr, "Failed - tokenize_item(%s): 1st element isn't quote. type=%d, symbol=%s\n",
				test_str, sym1->type, sym1->symbol);
		errcnt++;
	}
	
	//
	// quoted list
	//
	test_str = "'(abc 5 def)";
	hnd_tknz.p = test_str;
	cell1 = (CELL*)tokenize_item_old(&hnd_tknz);
	//print_object((OBJECT*)cell1, 0);
	sym1 = (SYMBOL*)car((OBJECT*)cell1);
	if (sym1->type != OBJTYPE_SYMBOL || strcmp(sym1->symbol, "quote")) {
		fprintf(stderr, "Failed - tokenize_item(%s): 1st element isn't quote. type=%d, symbol=%s\n",
				test_str, sym1->type, sym1->symbol);
		errcnt++;
	}
	sym1 = (SYMBOL*)car(cadr((OBJECT*)cell1));
	if (sym1->type != OBJTYPE_SYMBOL || strcmp(sym1->symbol, "abc")) {
		fprintf(stderr, "Failed - tokenize_item(%s): 2nd element's car isn't abc. type=%d, symbol=%s\n",
				test_str, sym1->type, sym1->symbol);
		errcnt++;
	}
	
	test_str = "''abc";
	hnd_tknz.p = test_str;
	cell1 = (CELL*)tokenize_item_old(&hnd_tknz);
	//print_object((OBJECT*)cell1, 0);
	sym1 = (SYMBOL*)car(cadr((OBJECT*)cell1));
	if (sym1->type != OBJTYPE_SYMBOL || strcmp(sym1->symbol, "quote")) {
		fprintf(stderr, "Failed - tokenize_item(%s): 2nd element's car isn't quote. type=%d, symbol=%s\n",
				test_str, sym1->type, sym1->symbol);
		errcnt++;
	}
	
	//
	// object start from '#'
	//
	test_str = "#t";
	hnd_tknz.p = test_str;
	bool = (BOOLEAN*) tokenize_item_old(&hnd_tknz);
	if (bool->type != OBJTYPE_BOOL) {
		fprintf(stderr, "Failed - tokenize_item(%s): object is not BOOLEAN. type=%d\n",
				test_str, bool->type);
		errcnt++;
	}
	if (!bool->value) {
		fprintf(stderr, "Failed - tokenize_item(%s): object is not #t. value=%d\n",
				test_str, bool->value);
		errcnt++;
	}
	
	return errcnt;
}*/

int test_env() {
	int errcnt = 0;
	FRAME* env;
	SYMBOL *sym_var01, *sym_var02, *sym_var03;
	OBJECT* obj;
	INTEGER* num1;
	CELL *exp1;
	STRING* str1;
	char string_val[] = "var02 string";
	char* test_str;
	char test_str4[] = "(define var03 8)";
	PROCEDURE* proc1;
	//HND_TOKENIZER hnd_tknz;
	
	env = create_empty_environment();
	
	sym_var01 = create_symbol("var01");
	
	add_bindings_to_frame(env, sym_var01->symbol, (OBJECT*)create_integer(6));
	
	num1 = (INTEGER*)lookup_variable_value(sym_var01, env);
	if (num1->type != OBJTYPE_INTEGER || num1->value != 6) {
		fprintf(stderr, "Failed - add_bindings_to_frame/lookup_variable_value(): object value is not 6. type=%d, value=%d\n", num1->type, num1->value);
		print_env(env);
		errcnt++;
	}
	sym_var02 = create_symbol("var02");
	define_variable(env, sym_var02->symbol, (OBJECT*)create_string(string_val));
	str1 = (STRING*)lookup_variable_value(sym_var02, env);
	if (str1->type != OBJTYPE_STRING || strcmp(str1->p_str, string_val)) {
		fprintf(stderr, "Failed - define_variable(): object value is not \"var02 string\". type=%d, p_str=%s\n", str1->type, str1->p_str);
		print_env(env);
		errcnt++;
	}
	
	set_variable_value(env, sym_var01, (OBJECT*)create_integer(7));
	num1 = (INTEGER*)lookup_variable_value(sym_var01, env);
	if (num1->type != OBJTYPE_INTEGER || num1->value != 7) {
		fprintf(stderr, "Failed - add_bindings_to_frame/lookup_variable_value(): object value is not 7. type=%d, value=%d\n", num1->type, num1->value);
		print_env(env);
		errcnt++;
	}

	exp1 = (CELL*)quick_tokenize_test(test_str4);
	//print_object(cadr((OBJECT*)exp1), 0);
	eval((OBJECT*)exp1, env);
	sym_var03 = create_symbol("var03");
	num1 = (INTEGER*)lookup_variable_value(sym_var03, env);
	if (num1->type != OBJTYPE_INTEGER || num1->value != 8) {
		fprintf(stderr, "Failed - eval(\"%s\"): return object is not 8. type=%d, value=%d\n", 
				test_str4, num1->type, num1->value);
		print_env(env);
		errcnt++;
	}
	
	//TODO fix the bug - explanation is folloing line. (glibc bug??)
	// you can pass the test if you use the test expression (1). 
	// but you would see if you use the test expression (2) 
	// or same string-length expression.
	// (1) test_str = "(define (myadd x y ) (+ x y))";
	// (2) test_str = "(define (myadd x y) (+ x y))";
	test_str = "(define (myadd x y ) (+ x y))";
	exp1 = (CELL*)quick_tokenize_test(test_str);
	obj = eval((OBJECT*)exp1, env);
	strcpy(sym_var01->symbol, "myadd"); 
	proc1 = (PROCEDURE*)lookup_variable_value(sym_var01, env);
	if (proc1->type != OBJTYPE_PROCEDURE) {
		fprintf(stderr, "Failed - eval(\"%s\"): return object is not PROCEDURE. type=%d\n", 
				test_str, proc1->type);
		print_env(env);
		errcnt++;
	}
	sym_var02 = (SYMBOL*)proc1->parameters->p_car; 
	if (sym_var02->type != OBJTYPE_SYMBOL || strcmp(sym_var02->symbol, "x")) {
		fprintf(stderr, "Failed - eval(\"%s\"): 1st parameter of the procedure is not \"x\".\n",
				test_str); 
		print_object((OBJECT*)proc1->parameters, 0);
		errcnt++;
	}

	return errcnt;
}

int test_assignment() {
	int errcnt=0;
	char* test_str;
	FRAME* env;
	OBJECT* obj1;
	SYMBOL* sym;
	INTEGER* num;
	
	env = setup_environment();
	
	obj1 = quick_tokenize_test("(define var01 2)");
	eval(obj1, env);
	//print_env(env);
	
	test_str = "(set! var01 3)";
	obj1 = quick_tokenize_test(test_str);
	
	eval(obj1, env);
	
	sym = create_symbol("var01");
	num = (INTEGER*)lookup_variable_value(sym, env);
	if (num->type != OBJTYPE_INTEGER || num->value != 3) {
		fprintf(stderr, "Failed - eval(\"%s\"): return object is not 3.\n", 
				test_str);
		print_env(env);
		errcnt++;
	}

	return errcnt;
}

int test_begin() {
	int errcnt=0;
	char* test_str;
	FRAME* env;
	OBJECT* obj1;
	SYMBOL* sym;
	INTEGER *num, *n_var01;
	
	env = setup_environment();
	
	test_str = "(begin (define var01 3) (set! var01 (+ 3 4)) (+ 4 5))";
	obj1 = quick_tokenize_test(test_str);
	num = (INTEGER*)eval(obj1, env);
	if (num->type != OBJTYPE_INTEGER || num->value != 9) {
		fprintf(stderr, "Failed - eval(\"%s\"): return object is not 9.\n", 
				test_str);
		print_env(env);
		errcnt++;
	}

	sym = create_symbol("var01");
	n_var01 = (INTEGER*)lookup_variable_value(sym, env);
	if (n_var01->type != OBJTYPE_INTEGER || n_var01->value != 7) {
		fprintf(stderr, "Failed - eval(\"%s\"): var01 is not 7.\n", 
				test_str);
		print_env(env);
		errcnt++;
	}

	return errcnt;
}

int test_prim_proc_exec() {
	int errcnt=0;
	FRAME* env;
	INTEGER* num1;
	OBJECT* exp1;
	BOOLEAN* bool;
	char* test_str;
	
	env = setup_environment();
	//print_env(env);
	test_str = "(+ 2 3)";
	exp1 = quick_tokenize_test(test_str);
	num1 = (INTEGER*)eval((OBJECT*)exp1, env);
//	printf("eval: %s ->", test_str);
	if (num1->type != OBJTYPE_INTEGER || num1->value != 5) {
		fprintf(stderr, "Failed - eval(\"%s\"): return object is not 5. value=%d\n", 
				test_str, num1->value);
		print_object((OBJECT*)num1, 0);
		errcnt++;
	}


	test_str = "(+ 2 (+ 3 4) 5)";
	exp1 = quick_tokenize_test(test_str);
	num1 = (INTEGER*)eval(exp1, env);
	//printf("eval: %s -> ", test_str);
	//print_object(num1, 0);
	//puts("");
	if (num1->type != OBJTYPE_INTEGER || num1->value != 14) {
		fprintf(stderr, "Failed - eval(\"%s\"): return object is not 14. value=%d\n", 
				test_str, num1->value);
		print_object((OBJECT*)num1, 0);
		errcnt++;
	}
	
	test_str = "(null? '(a))";
	exp1 = quick_tokenize_test(test_str);
	//print_object(exp1, 0);
	bool = (BOOLEAN*)eval(exp1, env);
	//printf("eval: %s -> ", test_str);
	//print_object(exp1, 0);
	//puts("");
	if (bool->type != OBJTYPE_BOOL || bool->value != 0) {
		fprintf(stderr, "Failed - eval(\"%s\"): return object is not #f. value=%d\n", 
				test_str, bool->value);
		print_object((OBJECT*)bool, 0);
		errcnt++;
	}

	return errcnt;
}

int test_if() {
	int errcnt = 0;
	char* test_str;
	OBJECT* exp1;
	INTEGER* num1;
	FRAME* env;

	env = setup_environment();
	
	test_str = "(if #t 2 3)";
	exp1 = quick_tokenize_test(test_str);
	num1 = (INTEGER*)eval(exp1, env);
	if (num1->type != OBJTYPE_INTEGER || num1->value != 2) {
		fprintf(stderr, "Failed - eval(\"%s\"): return object is not 2.\n", 
				test_str);
		errcnt++;
	}
	
	test_str = "(if (null? '(a)) (+ 3 1) (+ 4 2))";
	exp1 = quick_tokenize_test(test_str);
	//print_object(exp1, 0);
	num1 = (INTEGER*)eval(exp1, env);
	if (num1->type != OBJTYPE_INTEGER || num1->value != 6) {
		fprintf(stderr, "Failed - eval(\"%s\"): return object is not 6.\n", 
				test_str);
		print_object((OBJECT*)num1, 0);
		errcnt++;
	}
	

	return errcnt;
}

int test_combine_proc_exec() {
	int errcnt=0;
	char* test_str;
	CELL *exp1, *exp2;
	OBJECT* obj;
	FRAME* env;
	ENV_ENTRY* env_entry;
	//HND_TOKENIZER hnd_tknz;
	INTEGER* num1;
	
	env = setup_environment();
	
	
	//hnd_tknz.p = "(a b c)";
	//exp1 = (CELL*)tokenize_item_old(&hnd_tknz);
	exp1 = (CELL*)quick_tokenize_test("(a b c)");
	//hnd_tknz.p = "(2 3 4)";
	//exp2 = (CELL*)tokenize_item_old(&hnd_tknz);
	exp2 = (CELL*)quick_tokenize_test("(2 3 4)");
	env_entry = make_param_entries(exp1, exp2);
	if (strcmp("a", env_entry->symbol)) {
		fprintf(stderr, "Failed - make_param_entries(). 1st entry symbol=%s.\n", 
				env_entry->symbol);
		errcnt++;
	}
	env_entry =  env_entry->next->next;
	if (strcmp("c", env_entry->symbol)) {
		fprintf(stderr, "Failed - make_param_entries(). 3rd entry symbol=%s.\n", 
				env_entry->symbol);
		errcnt++;
	}
	num1 = (INTEGER*)env_entry->value;
	if (num1->type != OBJTYPE_INTEGER || num1->value != 4) {
		fprintf(stderr, "Failed - make_param_entries(). 3rd entry value=%d.\n", 
				num1->value);
		errcnt++;
	}
	if (env_entry->next) {
		fprintf(stderr, "Failed - make_param_entries(). 3rd entry is not terminated correctly.\n");
		errcnt++;
	}
	
	
	// --------- warning! see To-Do task in the test_env() ----------
	test_str = "(define (myadd x y ) (+ x y))";
	exp1 = (CELL*)quick_tokenize_test(test_str);
	//print_object(exp1->p_cdr, 0);
	//puts("");
	eval((OBJECT*)exp1, env);
	//print_env(env);
	test_str = "(myadd 6 7)";
	obj = quick_tokenize_test(test_str);
	//print_object(obj, 0);
	//puts("");
	num1 =(INTEGER*) eval(obj, env);
	//print_object(num1, 0);
	//puts("");
	if (num1->type != OBJTYPE_INTEGER || num1->value != 13) {
		fprintf(stderr, "Failed - eval(%s). result=%d\n", 
				test_str, num1->value);
		errcnt++;
	}

	return errcnt;
}

int test_recursive() {
	int errcnt=0;
	char* test_str;
	CELL* exp1;
	FRAME* env;
	INTEGER* num1;
	
	env = setup_environment();
	
	test_str = "(define (sum-to x) (if (= 0 x) 0 (+ x (sum-to (- x 1)))))";
	
	exp1 = (CELL*)quick_tokenize_test(test_str);
	
	eval((OBJECT*)exp1, env);
	//print_env(env);
	test_str = "(sum-to 5)";
	
	//g_indent = 0;
	
	exp1 = (CELL*)quick_tokenize_test(test_str);
	//print_object(exp1, 0);
	num1 =(INTEGER*) eval((OBJECT*)exp1, env);
	//print_object(num1, 0);
	//puts("");
	if (num1->type != OBJTYPE_INTEGER || num1->value != 15) {
		fprintf(stderr, "Failed - eval(%s). result=%d\n", 
				test_str, num1->value);
		errcnt++;
	}
	
	free(env);
	return errcnt;
}
