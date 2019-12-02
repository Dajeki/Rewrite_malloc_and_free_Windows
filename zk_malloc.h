#include <stdio.h>
#include <stdlib.h>

#if (defined(_WIN32) || defined(__CYGWIN__))
    #define OS "Windows"
    //double the default on windows
    #define support_heap_size 2048
    #include <Heapapi.h>

    static HANDLE heap_Memory_Support = NULL; 

    /**S
    * 
    * This beggining part is to allow for DECLSPEC is for logging purposes
    * @return void* - a pointer to the memory location.
    * @param memory_To_Allocate - how many bytes of data you would like to allocate to the heap.
    * 
    */

    DECLSPEC_ALLOCATOR void* zk_malloc(int memory_To_Allocate)
    {   
        //start the static HANDLE to the extra as NULL for comparison purposes
        void *memory_Location;

        //Try to allocate memory on the calling processes default heap
        memory_Location = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, memory_To_Allocate);        
        
        if (memory_Location != NULL)
        {

            return memory_Location;

        }
        else if (heap_Memory_Support == NULL)
        {

            printf("%s", "Ran out of memory on default heap, creating new heap");
            heap_Memory_Support = HeapCreate(0, 0, support_heap_size);
            
        }

        memory_Location = HeapAlloc(heap_Memory_Support, HEAP_ZERO_MEMORY, memory_To_Allocate);
        return memory_Location;    

    }

#elif (defined(__linux__))
    #define OS "Linux"

#else
    #define OS "Unknown"

#endif