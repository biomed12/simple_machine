/*
 * sm.h
 *
 *  Created on: Nov 28, 2019
 *      Author: burak.kirazli
 */

#ifndef SM_H_
#define SM_H_

#define MAKE_STATE(_parent, _handler) {_parent, _handler}
#define ADD_STATE(_s) (&_s)
#define NO_PARENT ((void*)0)
#define SM_ASSERT(_cond, _machine, _message) if((!_cond))sm_assert(_machine, __FILE__, __LINE__, _message)
#define MAKE_NAKED_EVT(_id) {_id, 0, 0}
#define HANDLED() 0
#define UNHANDLED() 1
#define TRANSIT(_target_state) ((int)&_target_state)


/*Forward decs.*/
typedef struct _state_t state_t;
typedef struct _event_t event_t;
typedef struct _machine_t machine_t;

/*
 * */
typedef enum _event_id
{
	EVT_ID_ENTRY = 0,
	EVT_ID_INIT,
	EVT_ID_EXIT,
	EVT_ID_TICK,

	EVT_ID_APP = 4,
}event_id;

/*
 * Event type.
 *
 * */
struct _event_t
{
	int id;
	void* payload;
	int payload_size;
};


/*
 * State handler type.
 *
 * */
typedef int (*state_handler_t)(machine_t* sm, event_t e);
typedef int (*initial_handler_t)(machine_t* sm);


/*
 *
 *
 * */
struct _state_t
{
	const state_t* parent;
	state_handler_t handler;
};

/*
 * Define machine.
 * */
struct _machine_t
{
	void* ctx;
	state_t** table;
	int table_size;
	state_t* current_state;
	initial_handler_t top_initial_handler;
};


/*
 * Assertion callback.
 *
 * */
typedef void (*assertion_callback_t)(machine_t* machine, const char* file_name, int line, const char* message);


/*
 *	Generic framework functions.
 */
void sm_set_assertion_callback(assertion_callback_t cb);

/*
 * Instance handler functions.
 * */
void sm_init(machine_t* machine,
			 void* ctx,
			 state_t** table,
			 initial_handler_t initial_handler,
			 int siz,
			 assertion_callback_t assert);

void sm_start(machine_t* machine);
void sm_process(machine_t* machine, event_t event);
void sm_assert(machine_t* machine, const char* file_name, int line, const char* message);


#endif /* SM_H_ */
