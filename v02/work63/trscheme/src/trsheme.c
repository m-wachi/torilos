/*
 * $Rev: 204 $
 * $Author: m-wachi $
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
#include "io.h"

void fancy_file_readloop(char* filename);
void fancy_stdin_readloop();

int main(int argc, char** argv) {
	//int i, j;
	
	puts("trscheme start.");
	
	//i = 3; j = 20;
	
	//sprintf(buff, "i=%d, j=%d\n", i, j);
	//puts(buff);
	
	objects_init();
	
	test_main();

	if (argc == 1) {
		fancy_stdin_readloop();
	}
	if (argc > 1) {
		if (strcmp(argv[1], "-testonly"))
			fancy_file_readloop(argv[1]);
	}
	
	
	puts("trscheme end.");
	exit(0);
}

void fancy_file_readloop(char* filename) {
	char c;
	FRAME* env;
	OBJECT *exp, *obj;
	int rc;
	
	if ((rc = setup_file_reader(filename))) {
		fprintf(stderr, "couldn't open file %s\n", filename);
		return;
	}
	env = setup_environment();

	c = io_peek();
	while (c) {
		exp = tokenize_item();
		print_object(exp, 0);
		obj = eval(exp, env);
		print_object(obj, 0);
		puts("");
		
		c = io_peek();
	}
	io_close();
	free(env);
}

void fancy_stdin_readloop() {
	char c;
	FRAME* env;
	OBJECT *exp, *obj;
	int rc;
	
	if ((rc = setup_stdin_reader())) {
		return;
	}
	env = setup_environment();

#ifndef TORILOS
	printf("> ");fflush(stdout);
#endif 
	c = io_peek();
	while (c) {
		exp = tokenize_item();
		//print_object(exp, 0);
		obj = eval(exp, env);
		print_object(obj, 0);
		printf("\n> ");fflush(stdout);
		
		c = io_peek();
	}
	io_close();
	free(env);
}
