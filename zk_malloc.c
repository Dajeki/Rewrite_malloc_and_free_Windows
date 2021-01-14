#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
//Dont need to include header because function order is safe.

//structure used to define free blocks of memory
typedef struct memory_block 
{
	size_t size;
	struct memory_block * next;
	struct memory_block * prev;

} block_t; 

static block_t * _free_list = NULL;

#define MEMORY_BLOCK(prt_to_block_info) ((void*)((unsigned long)prt_to_block_info + sizeof(block_t)))
#define HEADER_FOR_BLOCK(ptr_to_memory) ((void*)((unsigned long)ptr_to_memory - sizeof(block_t)))


#if (defined(_WIN32) || defined(__CYGWIN__))
    #include <windows.h>
    #include <Heapapi.h> 

    unsigned long getDefaultPageSize()
    {  
        SYSTEM_INFO si;
        GetSystemInfo(&si);

        return (unsigned long) si.dwPageSize;
    }

    #ifndef ALLOCATION_BLOCK_SIZE   
    #define ALLOCATION_BLOCK_SIZE 2 * getDefaultPageSize() 
    #endif


    void add_to_free_list(block_t* block)
    {
        block->prev = NULL;
        block->next = NULL;

        //if there is no free list or the address for free list is higher than that of block
        if (!_free_list || (unsigned long)_free_list > (unsigned long)block)
        {   
            //this means that address is higher
            if (_free_list)
            {
                //place current head previous to block
                _free_list->prev = block;
            }

            //set block next to current free_list
            block->next = _free_list;
            //the current start is block
            _free_list = block;
        }
        else
        {
            //set curr to the current free list start
            block_t * curr = _free_list;
            //while there is a current next and the current next address is < block
            //go to the next block in the list
            while (curr->next && (unsigned long)curr->next < (unsigned long)block) 
            {
                curr = curr->next;
            }
            //after you found a higher address, make it the blocks next
            //then set the current next to the block
            // current next -> block next-> orig current next 
            block->next = curr->next;
            curr->next = block;
        }        
    }

    void remove_from_free_list(block_t * block)
    {
        if ((int)block->prev == NULL)
        {
            if (block->next)
            {
                //the new head is block's next
                _free_list = block->next;
            }
            else
            {
                //else there is no block to move to once removed
                _free_list = NULL;
            }
        }
        else
        {
            //if there is a previous , set the previous next to current blocks next
            block->prev->next = block->next;
        }

        //if there is a block next (not the end)
        if (block->next)
        {
            //block's next's previous is the block's current previous
            block->next->prev = block->prev;
        }      
    }


    block_t * split_memory_block(block_t * initial_block, size_t size)
    {
        //get a pointer to the block of memory offset to account for header
        void * memory_block = MEMORY_BLOCK(initial_block);

        //the new free ptr block location is the memory block ptr + the size of the data
        block_t * newptr = (block_t *)((unsigned long)memory_block + size);

        //get the new free data in the block by getting the intial size - the intial data + metadata struct block_t
        newptr->size = initial_block->size - (size + sizeof(block_t));
        printf("Intial block is %d\n",initial_block->size);
        printf("New block size is %d\n", newptr->size);
        //the intial block space is now ONLY the space of the data if it gets freed
        initial_block->size = size;
        printf("Intial block is now %d\n\n",initial_block->size);

        return newptr;
    }



    void * zk_malloc(unsigned int memory_to_allocate)
    {
        void * block_memory;
        block_t *head, *newblockptr, *start_look;
        unsigned int block_header_size = sizeof(block_t);
        //incase we need to allocate more memory because we cant find enough.
        unsigned int allocation_size = memory_to_allocate >= ALLOCATION_BLOCK_SIZE ? memory_to_allocate + block_header_size : ALLOCATION_BLOCK_SIZE;

        //ptr so we know we have looked through everything
        start_look = _free_list;
        head = _free_list;

        while (head != NULL)
        {
            if (head->size >= memory_to_allocate + block_header_size || head->size == memory_to_allocate)
            {
                    block_memory = MEMORY_BLOCK(head);

                    printf("Found a big enough block allocating %d bytes from a total of %d bytes.\n\n", memory_to_allocate, head->size);
                    remove_from_free_list(head);

                    //if we get an exact match to the size
                    if (head->size == memory_to_allocate)
                    {
                        //return a pointer exactly to the space the user can use.
                        return block_memory;
                    }

                    //the size is bigger, but not exact so need to take a chunck off our memory block
                    newblockptr = split_memory_block(head, memory_to_allocate);
                    add_to_free_list(newblockptr);

                    return block_memory;
            }
            else
            {
                printf("Looked at block with %d of space.\nNeed enough for another header for the extra free space and the bytes to allocate\nThe total needed is: %d bytes or %d bytes (the size of the data is a perfect match).\n\n", head->size, memory_to_allocate + block_header_size, memory_to_allocate);
                head = head->next;
            }      
        }
        /*
            We got here because there is no ptr to a head of the free list because it hasnt been created before or there is no free memory
            above should always hit some sort of return if it finds a big enough memory slot
            get 2 pages of memory from the system (to reduce system calls)
            - Set the head size (because its the beggining) to the size of the allocation from the system - the header
        */
        head = HeapAlloc(GetProcessHeap(), 0 , allocation_size); 

        printf("No memory block big enough, getting %d bytes memory from OS\n\n", allocation_size);

        head->next = NULL;
        head->prev = NULL;

        //need to get ONLY AVAILABLE memory because header will be placed before memory block
        head->size = ALLOCATION_BLOCK_SIZE - block_header_size;

        if (ALLOCATION_BLOCK_SIZE > memory_to_allocate + block_header_size)
        {
            newblockptr = split_memory_block(head, memory_to_allocate);
            add_to_free_list(newblockptr);
        } 
        return MEMORY_BLOCK(head);             
    }
    

    void zk_free(void * memory_to_free)
    {
        void * Header = HEADER_FOR_BLOCK(memory_to_free);
        add_to_free_list(Header);
    }


#elif (defined(__linux__))
    #define OS "Linux"

#else
    #define OS "Unknown"

#endif