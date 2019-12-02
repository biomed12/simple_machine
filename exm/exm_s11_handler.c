/*
 * exm_s11_handler.c
 *
 *  Created on: Dec 1, 2019
 *      Author: burak.kirazli
 */
#include "exm.h"

int s11_handler(machine_t* mach, event_t e)
{
	exm_ctx_t* ctx = mach->ctx;

	switch (e.id) {
		case EVT_ID_ENTRY:
		{
			append_log("s11-entry- ");
		}
			break;

		case EVT_ID_EXIT:
		{
			append_log("s11-exit- ");
		}
			break;

		case EVT_ID_D:
		{
			if(ctx->foo == 0)
			{
				return UNHANDLED();
			}
			else
			{
				ctx->foo = 0;
				append_log("s11-D:");
				extern const state_t s1;
				return TRANSIT(s1);
			}
		}

			break;

		case EVT_ID_G:
		{
			append_log("s11-G:");
			extern const state_t s211;
			return TRANSIT(s211);
		}

			break;

		default:
			return UNHANDLED();
			break;
	}

	return 0;
}
