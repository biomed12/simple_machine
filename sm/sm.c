/*
 * sm.c
 *
 *  Created on: Nov 28, 2019
 *      Author: burak.kirazli
 */

#include "sm.h"
#include <string.h>

static assertion_callback_t assertion_callback;
void sm_set_assertion_callback(assertion_callback_t cb)
{
	assertion_callback = cb;
}


typedef struct _tick_list
{
	machine_t* list;
	int size;
	int freq;
}tick_list;

void sm_init(machine_t* machine,
			 void* ctx,
			 state_t** table,
			 initial_handler_t initial_handler,
			 int size,
			 assertion_callback_t assert)
{
	SM_ASSERT(machine != 0, machine, "MACHINE OBJECT IS NULL");
	SM_ASSERT(table != 0, machine, "TABLE OBJECT IS NULL");
	SM_ASSERT(initial_handler != 0, machine, "INITIAL HANDLER IS NULL");
	SM_ASSERT(size != 0, machine, "SIZE IS ZERO");
	SM_ASSERT(assert != 0, machine, "ASSERT CALLBACK IS NULL");

	memset(machine, 0, sizeof(machine_t));

	machine->ctx = ctx;
	machine->table = table;
	machine->table_size = size;
	machine->current_state = 0;
	machine->top_initial_handler = initial_handler;
	sm_set_assertion_callback(assert);
}

static int is_available(machine_t* mach, state_t* s)
{
	for(int i = 0; i < mach->table_size; i++)
	{
		if(mach->table[i] == s)
		{
			return 1;
		}
	}

	return 0;
}


static int process_event(machine_t* machine, event_t e)
{
	return machine->current_state->handler(machine, e);
}

static int post_init(machine_t* mach)
{
	//post init signal
	event_t e = MAKE_NAKED_EVT(EVT_ID_INIT);
	return process_event(mach, e);
}

static void post_entry(machine_t* mach)
{
	//post entry event
	event_t e = MAKE_NAKED_EVT(EVT_ID_ENTRY);
	process_event(mach, e);//do not evaluate return value
}

static void post_exit(machine_t* mach)
{
	event_t e = MAKE_NAKED_EVT(EVT_ID_EXIT);
	process_event(mach, e);
}


static int is_target_a_parent(machine_t* machine, state_t* target)
{
	state_t* temp = machine->current_state;
	while(temp != 0)
	{
		if(temp->parent == target)
		{
			return 1;
		}
		temp = (state_t*)temp->parent;
	}

	return 0;
}

static int is_target_a_child(machine_t* machine, state_t* target)
{
	state_t* temp = target;
	while(temp->parent != 0)
	{
		if(temp->parent == machine->current_state)
		{
			return 1;
		}
		temp = (state_t*)temp->parent;
	}

	return 0;
}

static state_t* top_parent(state_t* target)
{
	state_t* temp = target;

	while(temp->parent != 0)
	{
		temp = (state_t*)temp->parent;
	}

	return temp;
}

static void transit(machine_t* machine, state_t* target)
{
	state_t* entry_begin = 0;
	state_t* entry_end = 0;

	state_t* exit_begin = 0;
	state_t* exit_end = 0;

label_again:

	if(machine->current_state == 0)
	{
		//this is initial transtion
		entry_end = target;
		entry_begin = top_parent(target);

		//go to enter_sequence...
		goto label_entry;
	}
	else
		if(is_target_a_child(machine, target))
		{
			//there is no exit...
			entry_end = target;

			state_t* temp = entry_end;
			while(temp->parent != machine->current_state)
			{
				temp = (state_t*) temp->parent;
			}

			entry_begin = temp;

			goto label_entry;
		}
		else
			if(is_target_a_parent(machine, target))
			{
				//there will be exit actions.
				exit_begin = machine->current_state;
				exit_end = target;

				goto label_to_parent_exit;
			}
			else
				if(target == machine->current_state)
				{
					//an exit, an entry and an init
					post_exit(machine);
					post_entry(machine);

					int result = post_init(machine);

					if((result != HANDLED()) && (result != UNHANDLED()))
					{
						target = (state_t*)result;
						goto label_again;
					}

					return;
				}
				else
					{
						//find common parent
						state_t* begin = machine->current_state;
						state_t* end = target;
						state_t* common_parent = 0;

						while((begin != 0) && (end != 0))
						{
							state_t* temp = end;

							while(temp != 0)
							{
								if(temp->parent == begin->parent)
								{
									common_parent = (state_t*)temp->parent;

									//exit borders...
									exit_begin = machine->current_state;

									//find lower than common parent
									state_t* temp = exit_begin;
									while(temp->parent != common_parent)
									{
										temp = (state_t*)temp->parent;
									}
									exit_end = temp;


									//enter borders...
									entry_end = target;
									temp = entry_end;
									while(temp->parent != common_parent)
									{
										temp = (state_t*)temp->parent;
									}
									entry_begin = temp;

									goto label_exit;
								}
								//else
								temp = (state_t*)temp->parent;
							}

							begin = (state_t*)begin->parent;
						}
					}

label_exit:

	while(exit_begin != exit_end)
	{
		machine->current_state = exit_begin;
		post_exit(machine);
		exit_begin = (state_t*)exit_begin->parent;
	}

	machine->current_state = exit_begin;
	post_exit(machine);

	if(machine->current_state == target)
	{
		goto label_init;
	}

label_to_parent_exit:
	while(exit_begin != exit_end)
	{
		machine->current_state = exit_begin;
		post_exit(machine);
		exit_begin = (state_t*)exit_begin->parent;
	}

	machine->current_state = exit_begin;

	if(machine->current_state == target)
	{
		goto label_init;
	}

label_entry:

	while(entry_begin != entry_end)
	{
		machine->current_state = entry_begin;
		post_entry(machine);

		//find next
		state_t* temp = entry_end;
		while(temp->parent != entry_begin)
		{
			temp = (state_t*)temp->parent;
		}
		entry_begin = temp;
	}

	//post to the lowest level.
	machine->current_state = entry_begin;
	post_entry(machine);

label_init:

	{
		int result = post_init(machine);

		if((result != HANDLED()) && (result != UNHANDLED()))
		{
			target = (state_t*)result;
			goto label_again;
		}
	}
}

void sm_start(machine_t* machine)
{
	//run initial handler...
	int result = machine->top_initial_handler(machine);

	if((result == HANDLED()) || (result == UNHANDLED()))
	{
		SM_ASSERT(0, machine, "TOP INITIAL HANDLER MUST TRANSIT TO ANY STATE.");
	}

	//else
	//get the target state
	state_t* target = (state_t*)result;

	//find it from the list
	if(is_available(machine, target))
	{
		transit(machine, target);
	}
	else
	{
		SM_ASSERT(0, machine, "TOP INITIAL HANDLER DOES NOT TRANSIT TO A VALID STATE.");
	}
}

void sm_process(machine_t* machine, event_t event)
{
	//make sure machine was started...b
	SM_ASSERT(machine->current_state != 0, machine, "START MACHINE FIRST!");

	//make sure main handler set
	SM_ASSERT(machine->current_state->handler != 0, machine, "STATE HANDLER IS NULL!");

	int result = machine->current_state->handler(machine, event);

	if(result == UNHANDLED())
	{
		//unhandled event...
		//try to handle event in parents...
		state_t* parent = (state_t*)machine->current_state->parent;

		while(parent != 0)
		{
			result = parent->handler(machine, event);

			if(result == HANDLED())
			{
				break;
			}
			else
				if(result == UNHANDLED())
				{
					parent = (state_t*)parent->parent;
				}
				else
				{
					//transition occured...
					if(((state_t*)result) == parent)
					{
						//self transtiton by parent...
						post_exit(machine);
						machine->current_state = (state_t*)parent;
					}

					else

					//self transition over parent
					if(((state_t*)result) == machine->current_state)
					{
						while(machine->current_state->parent != parent)
						{
							post_exit(machine);
							if(machine->current_state->parent != 0)
							machine->current_state = (state_t*)machine->current_state->parent;
							else
							break;
						}

						post_exit(machine);
						machine->current_state = (state_t*)machine->current_state->parent;
					}

					transit(machine, (state_t*)result);
					break;
				}
		}
	}
	else
		if(result != HANDLED())
		{
			transit(machine, (state_t*)result);
		}

	return;
}

void sm_assert(machine_t* machine, const char* file_name, int line, const char* message)
{
	if(assertion_callback)
	{
		assertion_callback(machine, file_name, line, message);
	}
}


