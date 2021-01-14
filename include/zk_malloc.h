#ifndef  ZK_CUSTOM_HEAP
#define ZK_CUSTOM_HEAP
	#include "zk_malloc.c"

	void zk_free(void* memory_to_free);
	void* zk_malloc(unsigned int memory_to_allocate);
 
#endif 
