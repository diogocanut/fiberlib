#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fiberlib.h"
#include <sys/time.h>
#include <malloc.h>
#include <signal.h>

fiber *current;

fiber main_t;

ucontext_t maincontext;

int counter;

node *head, *tail;



long period_t;
struct itimerval timer;

void start_time() 
{
	setitimer(ITIMER_VIRTUAL,&timer,0);
}

void stop_time() 
{
	setitimer(ITIMER_VIRTUAL, 0, 0);
}

fiber fiber_self() 
{
  return *current;
}


int add_node(fiber *f, void *(*start_routine)(void *), void *arg)
{

	f->retval = start_routine(arg);
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

	counter++;

	return 1;

}

void fiber_init (long period) 
{


	head = tail = NULL;

	timer.it_value.tv_sec=period/1000000;
	timer.it_value.tv_usec=period;
	timer.it_interval.tv_sec=period/1000000;
	timer.it_interval.tv_usec=period;

	start_time();



	main_t.id = -1;
	
	if (getcontext(&(main_t.uc)) == -1) 
	{
		printf("Erro na fiber_init main context\n");
		exit(1);
	}	
	
	current = &main_t; 

}




int fiber_create (fiber *thread, void *(*start_routine)(void *), void *arg)  
{
	thread->id = 1;

	if (getcontext(&(thread->uc)) == -1 ) 
	{
		printf("Erro getcontext fiber create\n");
		exit(1);
	}

	thread->uc.uc_link = &maincontext;
	thread->uc.uc_stack.ss_sp = malloc(SIGSTKSZ);
	thread->uc.uc_stack.ss_size = SIGSTKSZ;

	add_node(&thread,start_routine,arg);

	makecontext(&thread->uc,(void (*) ()) scheduler, 0);


	return 0;

}


void scheduler()
{
	stop_time();

	if(head==NULL)
	{
		if(swapcontext(&main_t.uc, &current->uc) == -1)
		{
			printf("erro maincontext fiber create\n");
			exit(1);
		}
	}
	else
	{

	fiber *next = head->data;
	head = head->next;

	start_time();
    if(swapcontext(&(next->uc), &(current->uc)) == -1 ) {
		printf("Error while swap context\n"); /*calling the next thread*/
	}

	current = next;
	}
}