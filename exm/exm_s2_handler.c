/*
 * exm_s2_handler.c
 *
 *  Created on: Dec 1, 2019
 *      Author: burak.kirazli
 */
#include "exm.h"

int HANDLER(s2)(machine_t* mach, event_t e)
{
	exm_ctx_t* ctx = mach->ctx;

	switch (e.id) {
		case EVT_ID_ENTRY:
		{
			append_log("s2-entry-");
		}
			break;

		case EVT_ID_INIT:
		{
			append_log("s2-init-");
			DECLSTATE(s211);
			return TRANSIT(STATE(s211));
		}
			break;

		case EVT_ID_EXIT:
		{
			append_log("s2-exit-");
		}

			break;

		case EVT_ID_I:
		{
			if(ctx->foo == 0)
			{
				append_log("s2-I-");
				ctx->foo = 1;
				return HANDLED();
			}
			else
			{
				return UNHANDLED();
			}
		}

			break;

		default:

			return UNHANDLED();

			break;
	}

	return 0;
}
