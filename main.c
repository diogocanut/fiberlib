#include <stdio.h>
#include "fiberlib.h"

void *thread1(int i){
	printf("t1\n");
}

void *thread2(int i){
	printf("t2\n");
}

void *thread3(int i){
	printf("t3\n");
}

void *thread4(int i){
	printf("t4\n");
}

void *thread5(int i){
	printf("t5\n");
}

int main ()
{
	fiber t1,t2,t3,t4,t5;

	fiber_init(10);

	fiber_create(&t1,thread1,(void *)1);
	fiber_create(&t2,thread2,(void *)1);
	fiber_create(&t3,thread3,(void *)1);
	fiber_create(&t4,thread4,(void *)1);
	fiber_create(&t5,thread5,(void *)1);

	printf("%d\n",t1.id);
	printf("%d\n",t2.id);


}