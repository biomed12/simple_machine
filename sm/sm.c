/*
 * sm.c
 *
 *  Created on: Nov 28, 2019
 *      Author: burak.kirazli
 */

#include "sm.h"

#include <string.h>

static int is_available(machine_t* mach, const state_t* s)
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


static int is_target_a_parent(machine_t* machine, const state_t* target)
{
	const state_t* temp = machine->current_state;
	while(temp != 0)
	{
		if(temp->parent == target)
		{
			return 1;
		}
		temp = temp->parent;
	}

	return 0;
}

static int is_target_a_child(machine_t* machine, const state_t* target)
{
	const state_t* temp = target;
	while(temp->parent != 0)
	{
		if(temp->parent == machine->current_state)
		{
			return 1;
		}
		temp = temp->parent;
	}

	return 0;
}

static const state_t* top_parent(const state_t* target)
{
	const state_t* temp = target;

	while(temp->parent != 0)
	{
		temp = temp->parent;
	}

	return temp;
}

static void transit(machine_t* machine, const state_t* target)
{
	const state_t* entry_begin = 0;
	const state_t* entry_end = 0;

	const state_t* exit_begin = 0;
	const state_t* exit_end = 0;

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

			const state_t* temp = entry_end;
			while(temp->parent != machine->current_state)
			{
				temp =  temp->parent;
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
						target = (const state_t*)result;
						goto label_again;
					}

					return;
				}
				else
					{
						//find common parent
						const state_t* begin = machine->current_state;
						const state_t* end = target;
						const state_t* common_parent = 0;

						while((begin != 0) && (end != 0))
						{
							const state_t* temp = end;

							while(temp != 0)
							{
								if(temp->parent == begin->parent)
								{
									common_parent = temp->parent;

									//exit borders...
									exit_begin = machine->current_state;

									//find lower than common parent
									const state_t* temp = exit_begin;
									while(temp->parent != common_parent)
									{
										temp = temp->parent;
									}
									exit_end = temp;


									//enter borders...
									entry_end = target;
									temp = entry_end;
									while(temp->parent != common_parent)
									{
										temp = temp->parent;
									}
									entry_begin = temp;

									goto label_exit;
								}
								//else
								temp = temp->parent;
							}

							begin = begin->parent;
						}
					}

label_exit:

	while(exit_begin != exit_end)
	{
		machine->current_state = exit_begin;
		post_exit(machine);
		exit_begin = exit_begin->parent;
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
		exit_begin = exit_begin->parent;
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
		const state_t* temp = entry_end;
		while(temp->parent != entry_begin)
		{
			temp = temp->parent;
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
			target = (const state_t*)result;
			goto label_again;
		}
	}

	machine->tick_ms_counter = 0;
}


void sm_init(machine_t* machine,
			 const state_t** table,
			 int table_size,
			 initial_handler_t initial_handler)
{

    SM_ASSERT(machine != 0, machine, "MACHINE OBJECT IS NULL");
    SM_ASSERT(table != 0, machine, "TABLE OBJECT IS NULL");
    SM_ASSERT(initial_handler != 0, machine, "INITIAL HANDLER IS NULL");
    SM_ASSERT(table_size != 0, machine, "SIZE IS ZERO");

    memset(machine, 0, sizeof(machine_t));

    machine->table = table;
    machine->table_size = table_size;
    machine->current_state = 0;
    machine->top_initial_handler = initial_handler;
    machine->tick_ms_counter = 0;
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
    const state_t* target = (const state_t*)result;

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

	//
	if(event.id == EVT_ID_TICK_MS)
	{
		SM_ASSERT(event.header > 0, machine, "TICK MS HEADER IS ZERO\r\n");
		machine->tick_ms_counter += event.header;
	}

	int result = machine->current_state->handler(machine, event);

	if(result == UNHANDLED())
	{
		//unhandled event...
		//try to handle event in parents...
		const state_t* parent = machine->current_state->parent;

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
					parent = parent->parent;
				}
				else
				{
					const state_t* casted_result = (const state_t*)result;

					//transition occured...
					if(casted_result == parent)
					{
						//self transtiton by parent...
						post_exit(machine);
						machine->current_state = parent;
					}

					else

					//self transition over parent
					if(casted_result == machine->current_state)
					{
						while(machine->current_state->parent != parent)
						{
							post_exit(machine);
							if(machine->current_state->parent != 0)
							machine->current_state = machine->current_state->parent;
							else
							break;
						}

						post_exit(machine);
						machine->current_state = machine->current_state->parent;
					}

					transit(machine, casted_result);
					break;
				}
		}
	}
	else
		if(result != HANDLED())
		{
			transit(machine, (const state_t*)result);
		}

	return;
}



static assertion_callback_t assertion_callback;

void sm_assert(machine_t* machine, const char* file_name, int line, const char* message)
{
	if(assertion_callback)
	{
		assertion_callback(machine, file_name, line, message);
	}
}

void sm_set_assertion_callback(assertion_callback_t cb)
{
    assertion_callback = cb;
}

void sm_set_runner(machine_t* machine, void* runner)
{
	machine->runner = runner;
}

void sm_set_context(machine_t* machine, void* ctx)
{
	machine->ctx = ctx;
}


int sm_is_tick_ms_equal(machine_t* mach, int ms_val)
{
	if(mach->tick_ms_counter == ms_val)
	{
		return 1;
	}

	return 0;
}

void sm_reset_tick(machine_t* mach)
{
	mach->tick_ms_counter = 0;
}


