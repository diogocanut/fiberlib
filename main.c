#include <stdio.h>
#include "fiberlib.h"

void *thread1(int i){
	printf("Fui executada duvido que vai chega ate aqui\n");
}


int main ()
{
	fiber t1;

	fiber_init(10);

	fiber_create(&t1,thread1,(void *)1);

	printf("%d\n",t1.id);


}