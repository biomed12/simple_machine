/*
 * exm_s_handler.c
 *
 *  Created on: Dec 1, 2019
 *      Author: burak.kirazli
 */
#include "exm.h"

int s_handler(machine_t* mach, event_t e)
{
	switch (e.id) {
		case EVT_ID_ENTRY:
		{
			append_log("s-entry-");
		}
			break;

		case EVT_ID_EXIT:
		{
			append_log("s-exit-");
		}
			break;

		case EVT_ID_INIT:
		{
			append_log("s-init-");
			extern const state_t s11;
			return TRANSIT(s11);
		}
			break;

		case EVT_ID_E:
		{
			append_log("s-E:");
			extern const state_t s11;
			return TRANSIT(s11);
		}

			break;

		case EVT_ID_I:
		{
			append_log("s-I-");
		}

			break;

		default:
			break;
	}

	return 0;
}
