/*
 * $Author: m-wachi $
 * $Rev: 198 $
 */
#ifndef TEST_PRIM_PROC_H
#define TEST_PRIM_PROC_H
int test_prim_proc_main();
int test_is_null();
int test_plus_impl();
int test_minus_impl();
int int_test(char* funcname, char* argexp, INTEGER* actual, int expect);
int test_cons_impl();
int test_car_impl();
int test_cdr_impl();
int test_is_eq_impl();
int bool_test(char* funcname, char* argexp, BOOLEAN* actual, int expected);
int test_number_comparator();
#endif

