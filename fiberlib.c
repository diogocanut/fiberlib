#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fiberlib.h"
#include <sys/time.h>
#include <malloc.h>
#include <signal.h>


/* aponta para a fiber em execução no momento */
fiber *current;

/* fiber da main */
fiber main_t;

ucontext_t maincontext;

/* contador de threads na lista */
int counter;

/* cabeça e calda da lista de threads a serem executadas */
node *head, *tail;

/* aponta para a lista de fibers cuja execução foram finalizadas */
node *finished;

int cancel = 0;


/* adiciona nó apontando para fiber no final da lista */
int add_node(fiber *f, void *(*start_routine)(void *), void *arg)
{

	/* return value da fiber recebe a função passada */
	f->retval = start_routine(arg);

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

	counter++;

	return 1;

}

/* função chamada para adicionar uma nova fiber 
no final da lista de terminadas */
int add_finished(fiber *f)
{
	node *novo = (node *) malloc(sizeof(node));
	novo->data = f;
	novo->next = NULL;

	if(novo == NULL)
	{
		return 0;
	}

	if(finished == NULL)
	{
		finished = novo;
		return 0;
	}


	node *buff = finished;
	while(buff->next!=NULL)
	{
		buff = buff->next;

	}

	buff->next = novo;
	return 0;


}


/* inicializa as fibers */
void fiber_init(long period) 
{

	/* atualiza os ponteiros das listas */

	head = tail = finished = NULL;


	/* a fiber da main recebe id -1 */
	main_t.id = -1;
	
	/* salva o contexto da fiber main que está em execução */
	if (getcontext(&(main_t.uc)) == -1) 
	{
		printf("Erro na fiber_init main context\n");
		exit(1);
	}	
	
	/* variável current aponta para main */
	current = &main_t; 

}



int fiber_create (fiber *thread, void *(*start_routine)(void *), void *arg)  
{

	/* o id da fiber recebe seu id de acordo com o contador */
	thread->id = counter;

	/* salva o context da thread em execução */
	if (getcontext(&(thread->uc)) == -1 ) 
	{
		printf("Erro getcontext fiber create\n");
		exit(1);
	}

	thread->uc.uc_link = &maincontext;
	thread->uc.uc_stack.ss_sp = malloc(SIGSTKSZ);
	thread->uc.uc_stack.ss_size = SIGSTKSZ;

	/* adiciona a fiber na lista e linca um node novo a ela */

	add_node(&thread,start_routine,arg);

	/* executa o scheduler */

	makecontext(&thread->uc,(void (*) ()) scheduler, 0);


	return 0;

}


void scheduler()
{

	/* escalonador pega o primeiro elemento da lista de fibers e o coloca
	em execução */
	fiber *prev = NULL;
	fiber *next = NULL;


	prev = current;

	/* TESTE PREV DE UC */
	if(getcontext(&maincontext) == -1) 
	{
		printf("erro no scheduler get maincontext\n");
		exit(1);
	}

	if(head==NULL)
	{
	/* se a lista estiver vazia volta para o contexto da main */	
		if(swapcontext(&current->uc, &main_t.uc) == -1)
		{
			printf("erro maincontext fiber create\n");
			exit(1);
		}

		return;
	}
	
	next = head->data;	
	head = head->next;


	current = next;


	/* troca o contexto para a proxima fiber da lista */

    if(swapcontext(&(prev->uc), &(next->uc)) == -1 ) 
    {
		printf("Error while swap context\n"); 
	}


	/* adiciona a fiber que estava em execução a lista de terminadas */
	add_finished(&prev);

}


int fiber_join(fiber *f, void **retval)
{

	/* retorna 0 e o retval de uma fiber ja finalizada 
	caso se encontre na lista de finalizadas, caso contrario
	retorna -1 */ 
	if(current->id==f->id)
	{
		return 1;
	}
	if(finished==NULL)
	{
		return 1;
	}
	else
	{
		node *buff = finished;
		while(buff!=NULL)
		{
			if(buff->data->id==f->id)
			{
				*retval=f->retval;
				return 0;
			}

			buff = buff->next;

		}

	}

	return 1;
}

void fiber_exit(void *retval) 
{
	/* retorna o retval da fiber em execução e a finaliza */
	retval = current->retval;
	add_finished(current);
	scheduler();
}