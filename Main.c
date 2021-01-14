#include <stdio.h>
#include <stdlib.h>
#include "zk_malloc.h"

int main(void){

    printf("%s\n", "Starting program"); 

    int * test1 = zk_malloc(sizeof(int));

    zk_free(test1);

    int * test2 = zk_malloc(sizeof(int));
    int * test3 = zk_malloc(sizeof(int));

    *test2 = 7;
    *test3 = 9;
	
    printf("Use of test2 %d, Use of test3 %d\n\n", *test2, *test3);
    
    zk_free(test2);
    zk_free(test3);

    char * test4 = zk_malloc(sizeof(char));

    getchar();

}
