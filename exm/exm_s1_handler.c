/*
 * exm_s1_handler.c
 *
 *  Created on: Dec 1, 2019
 *      Author: burak.kirazli
 */
#include "exm.h"

int HANDLER(s1)(machine_t* mach, event_t e)
{
	exm_ctx_t* ctx = mach->ctx;

	switch (e.id) {
		case EVT_ID_ENTRY:
		{
			append_log("s1-entry- ");
		}
			break;

		case EVT_ID_INIT:
		{
			append_log("s1-init- ");
			DECLSTATE(s11);
			return TRANSIT(STATE(s11));
		}
			break;

		case EVT_ID_EXIT:
		{
			append_log("s1-exit- ");
		}

			break;

		case EVT_ID_I:
		{
			append_log("s1-I: ");
		}

			break;

		case EVT_ID_A:
		{
			append_log("s1-A: ");
			DECLSTATE(s1);
			return TRANSIT(STATE(s1));
		}

			break;

		case EVT_ID_D:
		{
			if(ctx->foo == 0)
			{
				ctx->foo = 1;
				append_log("s1-D:");
				DECLSTATE(s);
				return TRANSIT(STATE(s));
			}
			else
			{
				return UNHANDLED();
			}
		}

			break;

		case EVT_ID_C:
		{
			append_log("s1-C:");
			DECLSTATE(s2);
			return TRANSIT(STATE(s2));
		}

		default:

			return UNHANDLED();

			break;
	}

	return 0;
}
