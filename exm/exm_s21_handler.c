/*
 * exm_s21_handler.c
 *
 *  Created on: Dec 1, 2019
 *      Author: burak.kirazli
 */

#include "exm.h"

int s21_handler(machine_t* mach, event_t e)
{
	switch (e.id) {
		case EVT_ID_ENTRY:
		{
			append_log("s21-entry-");
		}

			break;

		case EVT_ID_EXIT:
		{
			append_log("s21-exit-");
		}

		break;


		case EVT_ID_G:
		{
			append_log("s21-G-");
			extern const state_t s1;
			return TRANSIT(s1);
		}

			break;

		default:

			return UNHANDLED();

			break;
	}

	return 0;
}

