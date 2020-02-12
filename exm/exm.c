/*
 * exm.c
 *
 *  Created on: Nov 30, 2019
 *      Author: burak.kirazli
 */

#include "exm.h"
#include <stdio.h>

//state handlers.
DECLHANDLER(s);
DECLHANDLER(s1);
DECLHANDLER(s11);
DECLHANDLER(s2);
DECLHANDLER(s21);
DECLHANDLER(s211);

//states
DEFSTATE(NO_PARENT, 	s);
DEFSTATE(STATEREF(s), 	s1);
DEFSTATE(STATEREF(s1), 	s11);
DEFSTATE(STATEREF(s), 	s2);
DEFSTATE(STATEREF(s2), 	s21);
DEFSTATE(STATEREF(s21), s211);

//state table of machine
const state_t* table[] =
{
		STATEREF(s),
		STATEREF(s1),
		STATEREF(s11),
		STATEREF(s2),
		STATEREF(s21),
		STATEREF(s211)
};

//machine instance
static machine_t machine;

//
void assertion_callback(machine_t* mach, const char* file_name, int line, const char* message)
{
	printf("Assertion at line: %d, in file: %s, message:%s\r\n", line, file_name, message);
	exit(1);
}

//initial transtion
static int topmost_initial_handler(machine_t* machine)
{
	exm_ctx_t* ctx = machine->ctx;
	ctx->foo = 0;
	append_log("topmost initial ");
	return TRANSIT(STATE(s2));
}


exm_ctx_t ctx;

//initializer
void exm_init(void)
{
	sm_init(&machine,
			table,
			sizeof(table) / sizeof(table[0]),
			topmost_initial_handler
			);

	sm_set_assertion_callback(assertion_callback);

	sm_set_context(&machine, &ctx);

	sm_start(&machine);

	print_log();
}

machine_t* get_mach(void)
{
	return &machine;
}

static char buffer[4096];
void reset_log(void)
{
	memset(buffer, 0, 4096);
}

void append_log(const char* log)
{
	size_t idx = 0;
	for(int i = 0; i < 4096; i++)
	{
		idx = i;
		if(buffer[i] == 0)
		{
			break;
		}
	}

	if(idx + strlen(log) < 4096)
	{
		buffer[idx] = '-';
		strcpy(&buffer[idx + 1], log);
	}
	else
	{
		reset_log();
	}
}

void print_log(void)
{
	printf("%s\r\n", buffer);
}










