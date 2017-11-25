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
int curcontext = 0;
ucontext_t *cur_context; 
int vet[10][1];

/* fiber em execução */
fiber *current;

/* main fiber */
fiber main_t;


/* nodes list */
node *head,*tail;

int counter;

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

void scheduler()
{

	fiber *next = NULL;

	if(head==NULL)
	{
	/* se a lista estiver vazia volta para o contexto da main */	
		return;
	}


	next = head->data;
	head = head->next;
	current = next;
	add_node(current);
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

/* Thread bodies */
void* thread1()
{
	while(1) {
			printf("Entrou na Funcao Thread 1\n"); 
		    printf("Entrou na Funcao Thread 1 I=%d\n", vet[curcontext][0]++);
		    sleep(1);
		 } 
}


void* thread2()
{
	int i;
	while(1) {
		    printf("Entrou na Funcao Thread 2\n"); 
		    printf("Entrou na Funcao Thread 2 I=%d\n", vet[curcontext][1]++);
  	        sleep(1); 
		 }; 
}



void
mkcontext(ucontext_t *uc, void *function)
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

	memset(vet,0,10*1*sizeof(int));

	setup_signals();

	/* timer */
	it.it_interval.tv_sec = period;
	it.it_interval.tv_usec = 0;
	it.it_value = it.it_interval;
	if (setitimer(ITIMER_REAL, &it, NULL) ) perror("setitiimer");

	/*salvando contexto da main*/
	main_t.id = -1;
	if ( getcontext(&(main_t.uc)) == -1) {
		printf("Error while getting context...exiting\n");
		exit(1);
	}	
	current = &main_t; 

}

void
main()
{

	fiber_init(4);	
	fiber t1,t2;
	fiber_create(&t1,thread1,0);
	fiber_create(&t2,thread2,0);
	scheduler();

}
