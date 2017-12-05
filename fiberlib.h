#include <ucontext.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>


typedef struct 
{
	int id;
	ucontext_t uc;
	void *retval;
	int cancel;
} fiber;

typedef struct node 
{
	struct node *next;
	fiber *data;
} node;


int add_node(fiber *f);
int rm_node(fiber *f);
int exists_node(fiber *f);

void scheduler();

void timer_interrupt(int j, siginfo_t *si, void *old_context);
void setup_signals(void);


void mkcontext(ucontext_t *uc, void *function);


void fiber_init(long period);
int fiber_create(fiber *thread, void *(*start_routine)(void *), void *arg);
int fiber_join(fiber *f, void **status);
void fiber_exit(void *retval);