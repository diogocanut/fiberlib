#include <ucontext.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include "fiberlib.h"


#define STACKSIZE 4096

/* timers */
sigset_t set; 
ucontext_t signal_context; 
void *signal_stack;
struct itimerval it;

/* contexts */
ucontext_t *cur_context; 


/* fiber em execução */
fiber *current;

/* main fiber */
fiber main_t;


/* nodes list */
node *head,*tail;

int counter;

int cancel=0;

int add_node(fiber *f)
{



	/* cria novo nó e atualiza a lista */
	node *novo = (node *) malloc(sizeof(node));
	novo->data = f;
	novo->next = NULL;

	if(novo == NULL)
	{
		return 0;
	}

	if(head == NULL)
	{
		head = tail = novo;

	}
	else
	{
		tail->next = novo;
		tail = novo;
	}


	return 1;

}

int rm_node(fiber *f)
{

	if(head==NULL){
		return 0;
	}

	node *prev = NULL;
	node *buff = head;
	printf("oiaaa\n");
	while(buff!=NULL&&buff->data->id!=f->id){
		prev = buff;
		buff = buff->next;
	}

	if(buff->data->id==f->id){
		if(buff->data->id==tail->data->id){
			buff->next = NULL;
			tail = prev;
		}else{
			prev->next = buff->next;
			}
			
		free(buff);
		return 1;
	}

	return 0;
}


int exists_node(fiber *f)
{
	int x = 0;
	node *buff = head;

	if(head==NULL){
		return 0;
	}

	while(buff->next!=NULL){

		if(buff->data->id==f->id){
			x = 1;
		}
	
		buff = buff->next;

	}

	if(x==0){
		return 0;
	}
	else return 1;

}


void scheduler()
{

	fiber *next = NULL;


	if(current->cancel==1){
		/*rm_node(current); */

	}
	else{
		
		if(current->id!=-1){
			add_node(current);

		}
	}

	if(head==NULL)
	{
	/* se a lista estiver vazia volta para o contexto da main */	
		return;
	}



	next = head->data;
	

	/*if(head->data->id == -1 && head->next!=NULL){
		head = head->next;
	}*/

	head = head->next;	
	current = next;



	cur_context = &next->uc;


	setcontext(&next->uc);

}


void timer_interrupt(int j, siginfo_t *si, void *old_context)
{

	getcontext(&signal_context);
	signal_context.uc_stack.ss_sp = signal_stack;
	signal_context.uc_stack.ss_size = STACKSIZE;
	signal_context.uc_stack.ss_flags = 0;
	sigemptyset(&signal_context.uc_sigmask);
	makecontext(&signal_context, scheduler, 1);



	swapcontext(cur_context,&signal_context);
}


void setup_signals(void)
{
	struct sigaction act;
	act.sa_sigaction = timer_interrupt;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART | SA_SIGINFO;
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	if(sigaction(SIGALRM, &act, NULL) != 0) 
	{
		perror("Signal handler");
	}
}


void mkcontext(ucontext_t *uc, void *function)
{
	void * stack;
	getcontext(uc);
	stack = malloc(STACKSIZE);
	if (stack == NULL) {
		perror("malloc");
	exit(1);
	}


	uc->uc_stack.ss_sp = stack;
	uc->uc_stack.ss_size = STACKSIZE;
	uc->uc_stack.ss_flags = 0;
	if (sigemptyset(&uc->uc_sigmask) < 0){
		perror("sigemptyset");
	exit(1);
	}


	makecontext(uc, function, 1);
	printf("context is %p\n", uc);
}


int fiber_create(fiber *thread, void *(*start_routine)(void *), void *arg)  
{


	/* o id da fiber recebe seu id de acordo com o contador */
	thread->id = counter;


	/* adiciona a fiber na lista e linca um node novo a ela */

	add_node(thread);

	/* executa o scheduler */

	mkcontext(&thread->uc,start_routine);


	return 0;

}


void fiber_init(long period) 
{

	signal_stack = malloc(STACKSIZE);
	if (signal_stack == NULL) {
		perror("malloc");
	exit(1);
	}

	

	setup_signals();

	/* timer */
	it.it_interval.tv_sec = period;
	it.it_interval.tv_usec = 0;
	it.it_value = it.it_interval;
	if (setitimer(ITIMER_REAL, &it, NULL) ) perror("setitiimer");

	/*salvando contexto da main*/
	main_t.id = -1;
	if ( getcontext(&(main_t.uc)) == -1) {
		printf("erro no contexto da main\n");
		exit(1);
	}	
	current = &main_t; 

}


int fiber_join(fiber *f, void **status)
{

	main_t.id = -1; 

	while(exists_node(f)){
		scheduler();
	}

	 main_t.id = 666; 

}

void fiber_exit(void *retval)
{
	current->retval = retval;
	current->cancel = 1;
	scheduler();

}
