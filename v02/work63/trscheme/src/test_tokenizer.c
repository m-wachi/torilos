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
#include "debug.h"
#include "io.h"
#include "test_tokenizer.h"
#include "primitive_procedures.h"

int test_tokenizer_main() {
	int errcnt = 0;

	errcnt += test_read_token();
	
	errcnt += test_read_string_token();
	
	errcnt += test_tokenize_sym_or_num();
	
	errcnt += test_tokenize_item();
	
	return errcnt;
}

int test_read_token() {
	int errcnt = 0;
	char tokenbuf[255];
	char c;
	
	setup_memory_reader("hello world)");
	
	read_token(tokenbuf);
	if (strcmp(tokenbuf, "hello")) {
		fprintf(stderr, "Failed - 1st read_token(): tokenbuf=%s\n", tokenbuf);
		errcnt++;
	}
	
	if (' ' != (c = io_peek())) {
		fprintf(stderr, "Failed - io_peek() after 1st read_token(): '%c'\n", c);
		errcnt++;
	}
	
	read_token(tokenbuf);
	if (strcmp(tokenbuf, "world")) {
		fprintf(stderr, "Failed - 2nd read_token(): tokenbuf=%s\n", tokenbuf);
		errcnt++;
	}
	
	if (')' != (c = io_peek())) {
		fprintf(stderr, "Failed - io_peek() after 2nd read_token(): '%c'\n", c);
		errcnt++;
	}
	
	read_token(tokenbuf);
	if (strlen(tokenbuf)) {
		fprintf(stderr, "Failed - 3rd read_token(): tokenbuf=%s\n", tokenbuf);
		errcnt++;
	}

	//
	// '\n' in the string
	//
	setup_memory_reader("hello\nworld\n)\n");
	
	read_token(tokenbuf);
	if (strcmp(tokenbuf, "hello")) {
		fprintf(stderr, "Failed - (\\n) 1st read_token(): tokenbuf=%s\n", tokenbuf);
		errcnt++;
	}
	
	read_token(tokenbuf);
	if (strcmp(tokenbuf, "world")) {
		fprintf(stderr, "Failed - (\\n) 2nd read_token(): tokenbuf=%s\n", tokenbuf);
		errcnt++;
	}
	
	if ('\n' != (c = io_peek())) {
		fprintf(stderr, "Failed - (\\n) io_peek() after 2nd read_token(): '%c'\n", c);
		errcnt++;
	}
	
	read_token(tokenbuf);
	if (strlen(tokenbuf)) {
		fprintf(stderr, "Failed - (\\n) 3rd read_token(): tokenbuf=%s\n", tokenbuf);
		errcnt++;
	}

	io_close();
	
	return errcnt;
}

int test_read_string_token() {
	int errcnt = 0;
	char tokenbuf[255];
	char c;
	
	setup_memory_reader("\"hello) world\" \"hey hey\"");
	
	read_string_token(tokenbuf);
	if (strcmp(tokenbuf, "hello) world")) {
		fprintf(stderr, "Failed - 1st read_string_token(): tokenbuf=%s\n", tokenbuf);
		errcnt++;
	}
	
	if ('"' != (c = io_peek())) {
		fprintf(stderr, "Failed - io_peek() after 2nd read_token(): '%c'\n", c);
		errcnt++;
	}
	
	read_string_token(tokenbuf);
	if (strcmp(tokenbuf, "hey hey")) {
		fprintf(stderr, "Failed - 2nd read_string_token(): tokenbuf=%s\n", tokenbuf);
		errcnt++;
	}
	
	io_close();
	
	return errcnt;
}


int test_tokenize_item() {
	int errcnt = 0;
	//char test_str1[] = 
	char test_str2[] = "(+ 2 (+ 4 5) 30 (- 3 2))";
	char test_str3[] = "(define var01 \"abcdef\")";
	char* test_str;
	CELL *cell1, *cell2;
	OBJECT* obj1;
	SYMBOL* sym1;
	BOOLEAN* bool;
	char c;
	
	test_str = "(+ 2 30)";
	cell1 = (CELL*)quick_tokenize_test(test_str);
	obj1 = cadr((OBJECT*)cell1);
	if (obj1->type != OBJTYPE_INTEGER) {
		fprintf(stderr, "Failed - tokenize_item(): 2nd element is not NUMBER. type=%d\n", obj1->type);
		print_object(obj1, 0);
		errcnt++;
	}

	cell1 = (CELL*)cddr((OBJECT*)cell1);
	cell2 = (CELL*)cdr((OBJECT*)cell1);
	if (!(cell1->p_cdr) || cell2->p_car || cell2->p_cdr) {
		fprintf(stderr, "Failed - tokenize_item(%s): cdddr return incorrectly.\n",
				test_str);
		errcnt++;
	}

	cell1 = (CELL*)quick_tokenize_test(test_str2);
	//print_object((OBJECT*)cell1, 0);
	cell2 = (CELL*)car(cddr((OBJECT*)cell1));
	if (cell2->type != OBJTYPE_CELL) {
		fprintf(stderr, "Failed - tokenize_item(): 3rd element's car is not list.\n");
		print_object((OBJECT*)cell2, 0);
		errcnt++;
	}
	
	cell1 = (CELL*)quick_tokenize_test(test_str3);	
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
	cell1 = (CELL*)quick_tokenize_test(test_str);
	//print_object((OBJECT*)cell1, 0);
	if (cell1->type != OBJTYPE_CELL) {
		fprintf(stderr, "Failed - tokenize_item(%s): return object isn't CELL. type=%d\n",
				test_str, cell1->type);
		errcnt++;
	}
	cell2 = (CELL*)cdr((OBJECT*)cell1);
	if (!(cell1->p_cdr) || cell2->p_car || cell2->p_cdr ) {
		fprintf(stderr, "Failed - tokenize_item(%s): not terminated correctly. cdr=%#x\n",
				test_str, (int)cell1->p_cdr);
		errcnt++;
	}
	
	//
	// tokenizing stop timing test.
	//
	test_str = "   (define (func01 a) (+ a 2)) (func01 3) x";
	setup_memory_reader(test_str);
	cell1 = (CELL*)tokenize_item();
	sym1 = (SYMBOL*)car((OBJECT*)cell1);
	errcnt += sym_test("1st tokenize_item", test_str, sym1, "define");
	c = io_peek();
	if (' ' != c) {
		fprintf(stderr, "Failed - tokenize_item(%s): tokenezer stops incorrectly. '%c'=io_peek()\n",
				test_str, c);
		errcnt++;
	}
	cell1 = (CELL*)tokenize_item();
	sym1 = (SYMBOL*)car((OBJECT*)cell1);
	errcnt += sym_test("2nd tokenize_item", test_str, sym1, "func01");
	c = io_read_next();
	if ('x' != c) {
		fprintf(stderr, "Failed - tokenize_item(%s): tokenezer stops incorrectly. '%c'=io_peek()\n",
				test_str, c);
		errcnt++;
	}
	
	io_close();
	
	test_str = "  (func02 a b )";
	setup_memory_reader(test_str);
	cell1 = (CELL*)tokenize_item();
	cell2 = cddr(cell1);
	if (!cell2->p_cdr) {
		fprintf(stderr, "Failed - tokenize_item(%s): cddr should not be nil.", 
				test_str);
		errcnt++;
	}
	cell2 = cdr(cell2);
	if (cell2->p_car || cell2->p_cdr) {
		fprintf(stderr, "Failed - tokenize_item(%s): cdddr should be nil.", 
				test_str);
		errcnt++;
	}
	
	
	//
	// quote(') test.
	//
	test_str = "'abc";
	cell1 = (CELL*)quick_tokenize_test(test_str);
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
	cell1 = (CELL*)quick_tokenize_test(test_str);
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
	cell1 = (CELL*)quick_tokenize_test(test_str);
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
	
	test_str = "'()";
	cell1 = (CELL*)quick_tokenize_test(test_str);
	//print_object(cell1, 0);
	cell2 = (CELL*)cddr((OBJECT*)cell1);
	if (!(cell1->p_cdr) || cell2->p_car || cell2->p_cdr ) {
		fprintf(stderr, "Failed - tokenize_item(%s): not terminated correctly.\n",
				test_str);
		errcnt++;
	}
	
	test_str = "''abc";
	cell1 = (CELL*)quick_tokenize_test(test_str);
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
	bool = (BOOLEAN*) quick_tokenize_test(test_str);
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
}

int test_tokenize_sym_or_num() {
	int errcnt = 0;
	SYMBOL* sym1;
	INTEGER* int1;
	REAL* real;
	char* test_str;
	
	test_str = "define";
	sym1 = (SYMBOL*)quick_tokenize_test(test_str);
	if (sym1->type != OBJTYPE_SYMBOL || strcmp(sym1->symbol, "define")) {
		fprintf(stderr, "Failed - tokenize_symbol_or_number(%s): object is not 'define. symbol=%s\n",
				test_str, sym1->symbol);
		errcnt++;
	}
	
	int1 = (INTEGER*)quick_tokenize_test("123");
	if (int1->type != OBJTYPE_INTEGER || int1->value != 123) {
		fprintf(stderr, "Failed - tokenize_symbol_or_number(%s): object is not 123. value=%d\n",
				test_str, int1->value);
		errcnt++;
	}
	
	test_str = "123.4";
	if (!is_real(test_str)) {
		fprintf(stderr, "Failed - is_real(%s): return value is incorrect.\n",
				test_str);
		errcnt++;
	}
	real = (REAL*)quick_tokenize_test(test_str);
	if (real->type != OBJTYPE_REAL || real->value != 123.4) {
		fprintf(stderr, "Failed - tokenize_symbol_or_number(%s): object is not 123.4. value=%.2f\n",
				test_str, real->value);
		errcnt++;
	}
	
	test_str = "12.3.4";
	if (is_real(test_str)) {
		fprintf(stderr, "Failed - is_real(%s): return value is incorrect.\n",
				test_str);
		errcnt++;
	}
	
	
	return errcnt;
}

int sym_test(char* funcname, char* argexp, SYMBOL* actual, char* expected) {
	int errcnt = 0;
	if (actual->type != OBJTYPE_SYMBOL) {
		fprintf(stderr, "Failed - %s%s: object is not SYMBOL. type=%d\n", 
				funcname, argexp, actual->type);
		print_object((OBJECT*)actual, 0);
		errcnt++;
	}
	if (strcmp(actual->symbol, expected)) {
		fprintf(stderr, "Failed - %s%s: symbol is expected %s. actual=%s\n", 
				funcname, argexp, expected, actual->symbol);
		errcnt++;
	}
	return errcnt;
}

OBJECT* quick_tokenize_test(char* s) {
	OBJECT* retobj;
	setup_memory_reader(s);
	retobj = tokenize_item();
	io_close();
	return retobj;
}
