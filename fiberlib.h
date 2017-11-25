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



void timer_interrupt(int j, siginfo_t *si, void *old_context);
void setup_signals(void);

int add_finished(fiber *f);
void mkcontext(ucontext_t *uc, void *function);
void fiber_init(long period);
int fiber_create(fiber *thread, void *(*start_routine)(void *), void *arg);