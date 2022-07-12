/*
 * $Author: m-wachi $
 * $Rev: 204 $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trscheme_config.h"
#include "objects.h"
#include "env.h"
#include "primitive_procedures.h"

FRAME* create_empty_environment() {
	return create_frame();
}

FRAME* enclosing_environment(FRAME* env) {
	return env->base_env;
}

void add_bindings_to_frame(FRAME* frame, char* var, OBJECT* value) {
	ENV_ENTRY* new_entry;
	
	new_entry = create_env_entry(var, value);
	new_entry->next = frame->first_entry;
	frame->first_entry = new_entry;
}

OBJECT* lookup_variable_value(SYMBOL* var , FRAME* env) {
	ENV_ENTRY* env_entry;
	char msg[128];
	
	if((env_entry = lookup_env_entry(var->symbol, env))) {
		return env_entry->value;
	}
	
	sprintf(msg, "Unbound variable %s", var->symbol);
	return (OBJECT*)create_error(msg);
	
}

ENV_ENTRY* lookup_env_entry(char* var, FRAME* env) {
	FRAME* frame;
	ENV_ENTRY* env_entry;
	
	frame = env;
	
	for (frame=env; frame; frame=frame->base_env) {
		if((env_entry = lookup_env_entry_in_entries(var, frame->first_entry)))
			return env_entry;
	}
	return NULL;
}



ENV_ENTRY* lookup_env_entry_in_entries(char* var, ENV_ENTRY* entries) {
	ENV_ENTRY* env_entry;
	
	env_entry = entries;
	
	while(env_entry) {
		if (!strcmp(var, env_entry->symbol))
			return env_entry;
		env_entry = env_entry->next;
	}
	return NULL;
	
}

FRAME* extend_environment_variable(ENV_ENTRY* entries, FRAME* base_env) {
	FRAME* new_env;
	
	new_env = create_frame();
	new_env->base_env = base_env;
	new_env->first_entry = entries;
	return new_env;
}

OBJECT* set_variable_value(FRAME* env, SYMBOL* var, OBJECT* value) {
	ENV_ENTRY* env_entry;
	char msg[128];
	
	if(!(env_entry = lookup_env_entry(var->symbol, env))) {
		sprintf(msg, "Unbound variable -- SET! %s", var->symbol);
		return (OBJECT*)create_error(msg);
	}
	//TODO implemnt making free old object. 
	env_entry->value = value;
	return value;
}

void define_variable(FRAME* env, char* var, OBJECT* value) {
	ENV_ENTRY* env_entry;
	
	env_entry = lookup_env_entry_in_entries(var, env->first_entry);
	
	if (env_entry) {
		//TODO implemnt making free old object. 
		env_entry->value = value;
	}
	else {
		add_bindings_to_frame(env, var, value);
	}
}

FRAME* extend_environment(FRAME* base_env, CELL* parameters, CELL* arguments) {
	FRAME* env;
	
	env = create_frame();
	env->first_entry = make_param_entries(parameters, arguments);
	env->base_env = base_env;
	return env;
}

FRAME* setup_environment() {
	FRAME* initial_env;
	initial_env = create_empty_environment();
	setup_prim_proc_env(initial_env);
	return initial_env;
}

ENV_ENTRY* make_param_entries(CELL* parameters, CELL* arguments) {
	CELL *param, *arg;
	ENV_ENTRY entry_hd;
	ENV_ENTRY* prev_entry;
	ENV_ENTRY* cur_entry;
	
	param = parameters;
	arg = arguments;
	prev_entry = &entry_hd;
	while(!is_null(param)) {
		cur_entry = create_env_entry(((SYMBOL*)param->p_car)->symbol, arg->p_car);
		prev_entry->next = cur_entry;
		prev_entry = prev_entry->next;
		param = (CELL*)param->p_cdr;
		arg = (CELL*)arg->p_cdr;
	}
	return entry_hd.next;
}
