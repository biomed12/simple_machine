/*
 * main.c
 *
 *  Created on: Dec 3, 2019
 *      Author: burak.kirazli
 */


#include "exm/exm.h"

//some heplers
static event_t make_evt(int id)
{
	event_t e = MAKE_NAKED_EVT(id);
	return e;
}

static void post_event(int evt_id)
{
	reset_log();
	sm_process(get_mach(), make_evt(evt_id));
	print_log();
}

//entry point
int main(void)
{
	//initialize example machine
	exm_init();

	//post example machine events...
	post_event(EVT_ID_G);
	post_event(EVT_ID_I);
	post_event(EVT_ID_A);
	post_event(EVT_ID_D);
	post_event(EVT_ID_D);
	post_event(EVT_ID_C);
	post_event(EVT_ID_E);
	post_event(EVT_ID_E);
	post_event(EVT_ID_G);
	post_event(EVT_ID_I);
	post_event(EVT_ID_I);

	return 0;

}

