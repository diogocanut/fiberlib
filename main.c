#include <stdio.h>
#include "fiberlib.h"

int curcontext = 0;
int vet[10][1];

/* Thread bodies */
void* thread1()
{

while(1){
			printf("Entrou na Funcao Thread 1\n"); 
		    printf("Entrou na Funcao Thread 1 I=%d\n", vet[curcontext][0]++);
		    sleep(1);
		    fiber_exit(NULL);
	}
}


void* thread2()
{

while(1){
		    printf("Entrou na Funcao Thread 2\n"); 
		    printf("Entrou na Funcao Thread 2 I=%d\n", vet[curcontext][1]++);
  	        sleep(1); 
  	        fiber_exit(NULL);
 }
}


void main()
{

	memset(vet,0,10*1*sizeof(int));

	fiber_init(1);	
	fiber t1,t2;

	fiber_create(&t1,thread1,0);
	fiber_create(&t2,thread2,0);
	fiber_join(&t1,NULL);


	printf("chegou aqui\n");
}