/*
 * $Author: m-wachi $
 * $Rev: 204 $
 */
#ifndef ENV_H
#define ENV_H

FRAME* create_empty_environment();
FRAME* enclosing_environment(FRAME* env);
void add_bindings_to_frame(FRAME* frame, char* var, OBJECT* value);
OBJECT* lookup_variable_value(SYMBOL* sym, FRAME* env);
ENV_ENTRY* lookup_env_entry(char* var, FRAME* env);
ENV_ENTRY* lookup_env_entry_in_entries(char* var, ENV_ENTRY* entries);
FRAME* extend_environment_variable(ENV_ENTRY* entries, FRAME* base_env);
OBJECT* set_variable_value(FRAME* env, SYMBOL* var, OBJECT* value);
void define_variable(FRAME* env, char* var, OBJECT* value);
FRAME* extend_environment(FRAME* base_env, CELL* parameters, CELL* arguments);
FRAME* setup_environment();
ENV_ENTRY* make_param_entries(CELL* parameters, CELL* arguments);
#endif
