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
#define SM_ASSERT(_cond, _machine, _message) if(!(_cond))sm_assert(_machine, __FILE__, __LINE__, _message)
#define MAKE_NAKED_EVT(_id) {_id, 0, 0, 0, 0, 0}
#define HANDLED() 0
#define UNHANDLED() 1
#define TRANSIT(_target_state) ((int)&_target_state)

//using these macros is optional.
#define HANDLER(x) x##_shandler
#define STATE(x)   (state_##x)
#define STATEREF(x) (&STATE(x))
#define DECLHANDLER(_handler) extern int HANDLER(_handler)(machine_t* mach, event_t e)
#define DEFHANDLER(_handler) int HANDLER(_handler)(machine_t* mach, event_t e)
#define DECLSTATE(x) extern state_t STATE(x)
#define DEFSTATE(_parent_ref, _handler_name) const state_t STATE(_handler_name) = MAKE_STATE(_parent_ref, HANDLER(_handler_name))


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
	EVT_ID_TICK_MS,

	EVT_ID_APP = 4,
}event_id;

/*
 * Event type.
 *
 * */
struct _event_t
{
	int id;
	int header;
	void* payload;
	int payload_size;
	unsigned char is_dynamic;
	void* additional;
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
 * Assertion callback.
 *
 * */
typedef void (*assertion_callback_t)(machine_t* machine, const char* file_name, int line, const char* message);



/*
 * Define machine.
 * */
struct _machine_t
{
	const state_t** table;
	int table_size;
	const state_t* current_state;
	initial_handler_t top_initial_handler;
	int tick_ms_counter;
	int schedule_timer;

	void* ctx;
	void* runner;
};


/*
 * Instance handler functions.
 * */
void sm_init(machine_t* machine,
			 const state_t** table,
			 int table_size,
			 initial_handler_t initial_handler);


void sm_start(machine_t* machine);
void sm_process(machine_t* machine, event_t event);
void sm_assert(machine_t* machine, const char* file_name, int line, const char* message);

void sm_set_assertion_callback(assertion_callback_t clb);
void sm_set_runner(machine_t* machine, void* runner);
void sm_set_context(machine_t* machine, void* ctx);

//tick related
int sm_is_tick_ms_equal(machine_t* mach, int ms_val);
void sm_reset_tick(machine_t* mach);

#endif /* SM_H_ */
