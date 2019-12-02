#include <stdio.h>
#include <stdlib.h>
#include "zk_malloc.h"

int main(void){

    printf("%s\n", "Starting program");

    int *testInt = zk_malloc(sizeof(int));

    printf("%d", sizeof(*testInt));

    getchar();

}