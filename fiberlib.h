#include <ucontext.h>

typedef struct 
{
	int id;
	ucontext_t uc;
	void *retval;
} fiber;

typedef struct node 
{
	struct node *next;
	fiber *data;
} node;


int add_node(fiber *f, void *(*start_routine)(void *), void *arg);

void fiber_init (long period);

int fiber_create (fiber *thread, void *(*start_routine)(void *), void *arg);

void fiber_run (void* (*start_routine)(void*), void* arg);

void scheduler();

int add_finished(fiber *f);

void fiber_exit(void *retval);