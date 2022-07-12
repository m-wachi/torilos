/*
 * $Author: m-wachi $
 * $Rev: 196 $
 */
#ifndef TEST_TOKENIZER_H
#define TEST_TOKENIZER_H

#include "objects.h"

int test_tokenizer_main();
int test_read_token();
int test_read_string_token();
int test_tokenize_item();
int test_tokenize_sym_or_num();
int sym_test(char* funcname, char* argexp, SYMBOL* actual, char* expected);
OBJECT* quick_tokenize_test(char* s);

#endif
