### Allocate and Free memory from Windows OS

#### TODO
- [x] Push initial commit to GitHub
- [x] Finish Research on Memory and System Calls
>> - [x] Find appropriate system calls
>> - [x] Learn how memory for programs are handled (Virtual memory, Heap, Stack)
>> - [x] Familiarize with pointers and pointer Arithmetic. 
>> - [x] Struct metadata for quick retreival of information about blocks
>> - [x] Tangent on data alignment with Structs and Unions (did not end up using as much as need to?)
- [ ] Add a squish for blocks of memory next to eachother. (Currently can have 56bytes of data if something like 2 ints next to eachother) 
- [ ] Make the struct better with data alignment.(Research if 24 is acceptable)

# Allocating Memory
- Memory is first allocated from the OS in 2 page sizes to be split up manually (Speed for memory)
- Add that memory to the list of free blocks to be split
- First allocation will cause the head of the free blocks list to be pushed past the header + the data first being inserted
- Sequential allocation will check to see if any blocks sizes are perfect (meaning it doesnt have to be split but can be directly inserted) or if it is big enough to hold another header(for the remaining free space) + the data being inserted

# Freeing Memory
- Adds the memory block back into the free list according to its memory location by linking the metadatas pointer to prev and next's free blocks metadata

![Data Being Allocated and Freed](https://github.com/Dajeki/Rewrite_malloc_and_free_Windows/blob/master/InsightToWorkings.png)
