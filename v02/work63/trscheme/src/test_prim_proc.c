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
#include "eval.h"
#include "primitive_procedures.h"
#include "env.h"
#include "debug.h"
#include "test.h"
#include "test_prim_proc.h"
#include "test_tokenizer.h"

int test_prim_proc_main() {
	int errcnt = 0;
	
	errcnt += test_is_null();

	errcnt += test_plus_impl();
	
	errcnt += test_minus_impl();
	
	errcnt += test_cons_impl();
	
	errcnt += test_car_impl();
	
	errcnt += test_cdr_impl();
	
	errcnt += test_is_eq_impl();
	
	errcnt += test_number_comparator();
	
	return errcnt;
}

int test_is_null() {
	int errcnt = 0;
	CELL *args, *test_list;
	BOOLEAN* bool;
	
	test_list = create_cell(NULL, NULL);
	//args = create_cell((OBJECT*)test_list, NULL);
	args = create_cell((OBJECT*)test_list, (OBJECT*)create_cell(NULL, NULL));
	
	bool = (BOOLEAN*)is_null_impl(args);
	if (bool->type != OBJTYPE_BOOL || !(bool->value)) {
		fprintf(stderr, "Failed - is_null_impl(): return object is not #t.\n");
		errcnt++;
	}
	
	test_list->p_car = (OBJECT*)create_integer(4);	
	bool = (BOOLEAN*)is_null_impl(args);
	if (bool->type != OBJTYPE_BOOL || bool->value) {
		fprintf(stderr, "Failed - is_null_impl(): return object is not #f.\n");
		print_object((OBJECT*)bool, 0);
		errcnt++;
	}
	return errcnt;
}

int test_plus_impl() {
	int errcnt = 0;
	char test_str2[] = "(3 4 5)";
	CELL* args;
	INTEGER *num1;
	
	args = (CELL*)quick_tokenize_test(test_str2);
	
	num1 = (INTEGER*)plus_impl(args);
	//print_object(obj, 0);
	if (num1->type != OBJTYPE_INTEGER) {
		fprintf(stderr, "Failed - plus_impl(): object is not NUMBER. type=%d\n", num1->type);
		print_object((OBJECT*)num1, 0);
		errcnt++;
	}
	if (num1->value != 12) {
		fprintf(stderr, "Failed - plus_impl(): object value is not 12. value=%d\n", num1->value);
		print_object((OBJECT*)num1, 0);
		errcnt++;
	}
	return errcnt;
}

int test_minus_impl() {
	int errcnt = 0;
	CELL* args;
	INTEGER *num1;
	char* test_str;
	
	test_str = "(10 2 3)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	num1 = (INTEGER*)minus_impl(args);
	//print_object(obj, 0);
	
	errcnt += int_test("minus_impl", test_str, num1, 5);
	
	errcnt += int_test("minus_impl","(car args)", (INTEGER*)car((OBJECT*)args), 10); 
	
	test_str = "(10)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	num1 = (INTEGER*)minus_impl(args);
	//print_object(obj, 0);
	
	errcnt += int_test("minus_impl",test_str, num1, -10);

	return errcnt;

}

int int_test(char* funcname, char* argexp, INTEGER* actual, int expected) {
	int errcnt = 0;
	if (actual->type != OBJTYPE_INTEGER) {
		fprintf(stderr, "Failed - %s%s: object is not INTEGER. type=%d\n", 
				funcname, argexp, actual->type);
		print_object((OBJECT*)actual, 0);
		errcnt++;
	}
	if (actual->value != expected) {
		fprintf(stderr, "Failed - %s%s: object value is expected %d. actual value=%d\n", 
				funcname, argexp, expected, actual->value);
		errcnt++;
	}
	return errcnt;
}

int test_cons_impl() {
	int errcnt = 0;
	char* test_str;
	OBJECT* result;
	INTEGER* num;
	CELL* args;
	
	test_str = "(1 (2 3))";
	args = (CELL*)quick_tokenize_test(test_str);
	
	result = cons_impl(args);
	//print_object(result, 0);
	num = (INTEGER*)car(cddr(result));

	errcnt += int_test("caddr of cons_impl", test_str, num, 3);
	
	test_str = "(1 2)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	result = cons_impl(args);
	//print_object(result, 0);
	num = (INTEGER*)cdr(result);
	
	errcnt += int_test("cdr of cons_impl", test_str, num, 2);
	
	test_str = "(1)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	result = cons_impl(args);
	//print_object(result, 0);
	if (result->type != OBJTYPE_ERROR) {
		fprintf(stderr, "cons_impl%s should return error\n", test_str);
		errcnt++;
	}
	test_str = "(1 2 3)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	result = cons_impl(args);
	//print_object(result, 0);
	if (result->type != OBJTYPE_ERROR) {
		fprintf(stderr, "cons_impl%s should return error\n", test_str);
		errcnt++;
	}

	return errcnt;
}

int test_car_impl() {
	int errcnt = 0;
	char* test_str;
	INTEGER* result;
	CELL* args;
	OBJECT* obj;
	
	test_str = "((1 2))";
	args = (CELL*)quick_tokenize_test(test_str);
	
	result = (INTEGER*)car_impl(args);
	//print_object((OBJECT*)result, 0);

	errcnt += int_test("car_impl", test_str, result, 1);
	
	test_str = "((1 2) 3)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	obj = car_impl(args);
	//print_object(obj, 0);
	if (obj->type != OBJTYPE_ERROR) {
		fprintf(stderr, "car_impl%s should return error\n", test_str);
		errcnt++;
	}
	
	test_str = "(1)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	obj = car_impl(args);
	//print_object(obj, 0);
	if (obj->type != OBJTYPE_ERROR) {
		fprintf(stderr, "car_impl%s should return error\n", test_str);
		errcnt++;
	}

	return errcnt;
}

int test_cdr_impl() {
	int errcnt = 0;
	char* test_str;
	CELL* result;
	CELL* args;
	OBJECT* obj;
	
	test_str = "((1 2))";
	args = (CELL*)quick_tokenize_test(test_str);
	
	result = (CELL*)cdr_impl(args);
	//print_object((OBJECT*)result, 0);
	if (result->type != OBJTYPE_CELL) {
		fprintf(stderr, "Failed - cdr_impl(1 2): return value must be CELL\n");
		errcnt++;
	}
	errcnt += int_test("car of cdr_impl", test_str, 
			(INTEGER*)car((OBJECT*)result), 2);
	
	test_str = "((1 2) 3)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	obj = cdr_impl(args);
	//print_object(obj, 0);
	if (obj->type != OBJTYPE_ERROR) {
		fprintf(stderr, "cdr_impl%s should return error\n", test_str);
		errcnt++;
	}
	
	test_str = "(1)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	obj = cdr_impl(args);
	//print_object(obj, 0);
	if (obj->type != OBJTYPE_ERROR) {
		fprintf(stderr, "cdr_impl%s should return error\n", test_str);
		errcnt++;
	}
	
	test_str = "(())";
	args = (CELL*)quick_tokenize_test(test_str);
	
	obj = cdr_impl(args);
	//print_object(obj, 0);
	if (obj->type != OBJTYPE_ERROR) {
		fprintf(stderr, "car_impl%s should return error\n", test_str);
		errcnt++;
	}
	

	return errcnt;
}

int test_is_eq_impl() {
	int errcnt = 0;
	char* test_str;
	BOOLEAN* result;
	CELL* args;
	
	test_str = "(a a)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	result = (BOOLEAN*)is_eq_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_eq_impl", test_str, result, 1);
	
	test_str = "(a b)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	result = (BOOLEAN*)is_eq_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_eq_impl", test_str, result, 0);
	
	test_str = "(() ())";
	args = (CELL*)quick_tokenize_test(test_str);
	
	result = (BOOLEAN*)is_eq_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_eq_impl", test_str, result, 1);
	
	return errcnt;
}


int bool_test(char* funcname, char* argexp, BOOLEAN* actual, int expected) {
	int errcnt = 0;
	char* exp_str;
	
	if (actual->type != OBJTYPE_BOOL) {
		fprintf(stderr, "Failed - %s%s: object is not BOOLEAN. type=%d\n", 
				funcname, argexp, actual->type);
		print_object((OBJECT*)actual, 0);
		errcnt++;
	}
	if (expected == 0)
		exp_str = "#f";
	else
		exp_str = "#t";
	
	if (actual->value != expected) {
		fprintf(stderr, "Failed - %s%s: object value is expected %s. actual value=%d\n", 
				funcname, argexp, exp_str, actual->value);
		errcnt++;
	}
	return errcnt;
}

int test_number_comparator() {
	int errcnt = 0;
	char* test_str;
	BOOLEAN* result;
	CELL* args;
	
	test_str = "(1 1)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	result = (BOOLEAN*)is_equal_number_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_equal_number_impl", test_str, result, 1);
	
	result = (BOOLEAN*)is_less_than_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_less_number_impl", test_str, result, 0);
	
	result = (BOOLEAN*)is_greater_than_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_greater_number_impl", test_str, result, 0);
	
	result = (BOOLEAN*)is_greater_equal_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_greater_equal_impl", test_str, result, 1);
	
	result = (BOOLEAN*)is_less_equal_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_less_equal_impl", test_str, result, 1);
	
	test_str = "(1 2)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	result = (BOOLEAN*)is_equal_number_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_equal_number_impl", test_str, result, 0);
	
	result = (BOOLEAN*)is_less_than_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_less_number_impl", test_str, result, 1);
	
	result = (BOOLEAN*)is_greater_than_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_greater_number_impl", test_str, result, 0);
	
	result = (BOOLEAN*)is_greater_equal_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_greater_equal_impl", test_str, result, 0);
	
	result = (BOOLEAN*)is_less_equal_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_less_equal_impl", test_str, result, 1);
	
	test_str = "(2 1)";
	args = (CELL*)quick_tokenize_test(test_str);
	
	result = (BOOLEAN*)is_equal_number_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_equal_number_impl", test_str, result, 0);
	
	result = (BOOLEAN*)is_less_than_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_less_number_impl", test_str, result, 0);
	
	result = (BOOLEAN*)is_greater_than_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_greater_number_impl", test_str, result, 1);
	
	result = (BOOLEAN*)is_greater_equal_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_greater_equal_impl", test_str, result, 1);
	
	result = (BOOLEAN*)is_less_equal_impl(args);
	//print_object((OBJECT*)result, 0);
	errcnt += bool_test("is_less_equal_impl", test_str, result, 0);
	

	return errcnt;
}
