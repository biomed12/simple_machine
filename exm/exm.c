/*
 * exm.c
 *
 *  Created on: Nov 30, 2019
 *      Author: burak.kirazli
 */

#include "exm.h"
#include <stdio.h>

//state handlers.
static int topmost_initial_handler(machine_t* machine);
extern int s_handler(machine_t* mach, event_t e);
extern int s1_handler(machine_t* mach, event_t e);
extern int s11_handler(machine_t* mach, event_t e);
extern int s2_handler(machine_t* mach, event_t e);
extern int s21_handler(machine_t* mach, event_t e);
extern int s211_handler(machine_t* mach, event_t e);

//states
const state_t s  	= MAKE_STATE(NO_PARENT, s_handler);
const state_t s1 	= MAKE_STATE(&s, s1_handler);
const state_t s11 	= MAKE_STATE(&s1, s11_handler);
const state_t s2 	= MAKE_STATE(&s, s2_handler);
const state_t s21 	= MAKE_STATE(&s2, s21_handler);
const state_t s211 	= MAKE_STATE(&s21, s211_handler);

//state table of machine
const state_t* table[] =
{
		ADD_STATE(s),
		ADD_STATE(s1),
		ADD_STATE(s11),
		ADD_STATE(s2),
		ADD_STATE(s21),
		ADD_STATE(s211)
};

//machine instance
static machine_t machine;

//
void assertion_callback(machine_t* mach, const char* file_name, int line, const char* message)
{
	printf("Assertion at line: %d, in file: %s, message:%s\r\n", line, file_name, message);
	exit(1);
}

exm_ctx_t ctx;

//initializer
void exm_init(void)
{
	sm_init(&machine,
			&ctx,
			(state_t**)table,
			topmost_initial_handler,
			sizeof(table) / sizeof(table[0]),
			assertion_callback);

	sm_start(&machine);
	print_log();
}

machine_t* get_mach(void)
{
	return &machine;
}

//initial transtion
static int topmost_initial_handler(machine_t* machine)
{
	exm_ctx_t* ctx = machine->ctx;
	ctx->foo = 0;
	append_log("topmost initial ");
	return TRANSIT(s2);
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










