#include <stdio.h>
#include "fiberlib.h"

int curcontext = 0;
int vet[10][7];

/* Thread bodies */
void* thread1()
{

while(1){
			printf("Entrou na Funcao Thread 1\n"); 
		    printf("Entrou na Funcao Thread 1 I=%d\n", vet[curcontext][0]++);
		    sleep(1);

	}
}


void* thread2()
{

while(1){
		    printf("Entrou na Funcao Thread 2\n"); 
		    printf("Entrou na Funcao Thread 2 I=%d\n", vet[curcontext][1]++);
  	        sleep(1); 

 }
}

void* thread3()
{

while(1){
		    printf("Entrou na Funcao Thread 3\n"); 
		    printf("Entrou na Funcao Thread 3 I=%d\n", vet[curcontext][2]++);
  	        sleep(1); 

 }
}

void* thread4()
{

while(1){
		    printf("Entrou na Funcao Thread 4\n"); 
		    printf("Entrou na Funcao Thread 4 I=%d\n", vet[curcontext][3]++);
  	        sleep(1); 

 }
}

void* thread5()
{

while(1){
		    printf("Entrou na Funcao Thread 5\n"); 
		    printf("Entrou na Funcao Thread 5 I=%d\n", vet[curcontext][4]++);
  	        sleep(1); 

 }
}

void* thread1_io()
{

int i = 0;

		    printf("Entrou na Funcao Thread IO 1\n"); 
		    printf("Entrou na Funcao Thread IO 1 I=%d\n", vet[curcontext][5]++);
		    scanf("%d" ,&i);
  	        sleep(1); 

}

void* thread2_io()
{
int i = 0;

		    printf("Entrou na Funcao Thread IO 2\n"); 
		    printf("Entrou na Funcao Thread IO 2 I=%d\n", vet[curcontext][6]++);
		    scanf("%d" ,&i);
  	        sleep(1); 

}

void main()
{

	memset(vet,0,10*7*sizeof(int));

	fiber_init(1);	
	fiber t1,t2,t3,t4,t5,t1io,t2io;

	fiber_create(&t1,thread1,0);
	fiber_create(&t2,thread2,0);
	fiber_create(&t3,thread3,0);
	fiber_create(&t4,thread4,0);
	fiber_create(&t5,thread5,0);

	fiber_create(&t1io,thread1_io,0);
	fiber_create(&t2io,thread2_io,0);

	fiber_join(&t1,NULL);


	printf("chegou aqui\n");
}
