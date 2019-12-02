/*
 * exm_s211_handler.c
 *
 *  Created on: Dec 1, 2019
 *      Author: burak.kirazli
 */

#include "exm.h"

int s211_handler(machine_t* mach, event_t e)
{
	switch (e.id) {
		case EVT_ID_ENTRY:
		{
			append_log("s211-entry-");
		}

			break;

		case EVT_ID_EXIT:
		{
			append_log("s211-exit-");
		}

			break;

		default:
		{
			return UNHANDLED();
		}
			break;
	}

	return HANDLED();
}
