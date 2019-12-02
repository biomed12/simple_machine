/*
 * exm.h
 *
 *  Created on: Nov 30, 2019
 *      Author: burak.kirazli
 */

#ifndef EXAMPLEMACHINE_EXM_H_
#define EXAMPLEMACHINE_EXM_H_

#include "../sm/sm.h"
#include <string.h>
#include <stdlib.h>

void exm_init(void);
machine_t* get_mach(void);

//machine events
typedef enum _exm_events
{
	EVT_ID_G = EVT_ID_APP,
	EVT_ID_I,
	EVT_ID_A,
	EVT_ID_D,
	EVT_ID_C,
	EVT_ID_E,

}exm_events;

//machine context
typedef struct _exm_ctx
{
	int foo;
}exm_ctx_t;

//exm helpers
void reset_log(void);
void append_log(const char* log);
void print_log(void);

#endif /* EXAMPLEMACHINE_EXM_H_ */
