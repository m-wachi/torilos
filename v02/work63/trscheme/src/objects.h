/*
 * $Author: m-wachi $
 * $Rev: 196 $
 */
#ifndef CELL_H
#define CELL_H

//#define CLTYPE_LISTHEAD -1

#define OBJTYPE_CELL		0
#define OBJTYPE_SYMBOL	1
#define OBJTYPE_INTEGER	2
#define OBJTYPE_STRING	3
#define OBJTYPE_BOOL		4
#define OBJTYPE_ERROR	5
#define OBJTYPE_PRIMITIVE_PROCEDURE	6
#define OBJTYPE_PROCEDURE	7
#define OBJTYPE_REAL		8

typedef struct obj_t {
	int type;
	void* data;
} OBJECT;

typedef struct cell_t {
	int type;
	OBJECT* p_car;
	OBJECT* p_cdr;
} CELL;

typedef struct sym_t {
	int type;
	char* symbol;
} SYMBOL;

typedef struct integer_t {
	int type;
	int value;
} INTEGER;

typedef struct real_t {
	int type;
	double value; 
} REAL;

typedef struct string_t {
	int type;
	char* p_str;
} STRING;

typedef struct procedure_t {
	int type;
	CELL* parameters;
	CELL* body;
	struct frame_t* env;
} PROCEDURE;

typedef struct primitive_procedure_t {
	int type;
	OBJECT* (*c_function)(CELL* lst_param);
} PRIMITIVE_PROCEDURE;


typedef struct boolean_t {
	int type;
	int value;
} BOOLEAN;

typedef struct error_t {
	int type;
	char* msg;
} ERROR;

typedef struct env_entry_t {
	char* symbol;
	OBJECT* value;
	struct env_entry_t* next;
} ENV_ENTRY;


typedef struct frame_t {
	ENV_ENTRY* first_entry;
	struct frame_t* base_env;
} FRAME;
void objects_init();
CELL* create_cell(OBJECT* obj1, OBJECT* obj2);
SYMBOL* create_symbol(char* symbol);
INTEGER* create_integer(int value);
REAL* create_real(double value);
STRING* create_string(char* s);
ERROR* create_error(char* msg);
PROCEDURE* create_procedure(CELL* parameters, CELL* body, FRAME* env);
PRIMITIVE_PROCEDURE* create_primitive_procedure(OBJECT* (*c_function)(CELL*));
ENV_ENTRY* create_env_entry(char* symbol, OBJECT* value);
FRAME* create_frame();
BOOLEAN* get_true();
BOOLEAN* get_false();
#endif 
