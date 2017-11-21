#include <ucontext.h>

typedef struct {
	int id; /* thread id to be used for equality */
	ucontext_t uc; /* variable to store context */
	void *retval; /*to store return value of thread*/
} fiber;

typedef struct node {
	struct node *next;
	fiber *data;
} node;


void start_time();
void stop_time();

int add_node(fiber *f, void *(*start_routine)(void *), void *arg);

fiber fiber_self();

void fiber_init (long period);

int fiber_create (fiber *thread, void *(*start_routine)(void *), void *arg);

void fiber_run (void* (*start_routine)(void*), void* arg);

void scheduler();