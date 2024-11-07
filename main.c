#include "umem.h"
#include "umem.c"
#include <stdio.h>

void basic_first_fit_test()
{
    printf("\n=== Testing FIRST_FIT Algorithm ===\n");
    umeminit(4096, FIRST_FIT);

    printf("Initial free list:\n");
    print_free_list();

    // Test 1: Allocate some memory blocks and free them
    void *ptr1 = umalloc(512);  // Allocate 512 bytes
    void *ptr2 = umalloc(1024); // Allocate 1024 bytes
    void *ptr3 = umalloc(256);  // Allocate 256 bytes

    // Print the allocated blocks (for debugging)
    printf("Allocated pointers: %p, %p, %p\n", ptr1, ptr2, ptr3);

    // Test 2: Free the allocated blocks
    ufree(ptr1); // Free the first block
    ufree(ptr2); // Free the second block
    ufree(ptr3); // Free the third block

    // Test 3: Allocate again after freeing
    void *ptr4 = umalloc(128); // Allocate 128 bytes
    void *ptr5 = umalloc(256); // Allocate 256 bytes

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
}

void basic_best_fit_test()
{
    printf("=== Testing BEST_FIT Algorithm ===\n");

    // Initialize memory pool (e.g., 4096 bytes, best fit strategy)
    umeminit(4096, BEST_FIT);

    // Allocate several blocks
    int *ptr1 = umalloc(256); // Should allocate from the first fit
    int *ptr2 = umalloc(128); // Best fit should choose a smaller block if available
    int *ptr3 = umalloc(512); // Allocate a larger block

    printf("Allocated pointers: %p, %p, %p\n", ptr1, ptr2, ptr3);

    // Free some blocks
    ufree(ptr2); // Free the smaller block
    ufree(ptr1); // Free another block

    // Allocate again after some deallocations
    int *ptr4 = umalloc(150); // Should use the best fit block for size 150
    int *ptr5 = umalloc(100); // Allocate another smaller block

    // Print the current state of the free list (to verify best fit allocation)
    printf("Free list after allocations and deallocations:\n");
    print_free_list(); // Assuming you have a function to print free list for debugging

    // Final cleanup
    ufree(ptr3);
    ufree(ptr4);
    ufree(ptr5);
    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
}

int main()
{
    basic_first_fit_test();

    return 0;
}

// int main()
// {
//     // Page size is 4096
//     // float frag = calculate_fragmentation();
//     umeminit(4096, FIRST_FIT);
//     int *ptr = umalloc(16);
//     int *ptr2 = umalloc(25);
//     int *ptr3 = umalloc(100);
//     int *ptr4 = umalloc(10);
//     ufree(ptr2);
//     ufree(ptr4);

//     // int *ptr7 = realloc(ptr6, 25);
//     printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
//     return 0;
// }

// void ufree(void *ptr)
// {
//     if (ptr == NULL)
//     {
//         return; // Handle null pointer case
//     }

//     // Assuming `header_t` is defined as in your `umem.h` and is placed before the allocated memory
//     header_t *start_of_header = (header_t *)((char *)ptr - sizeof(header_t));

//     // Get the size from the header
//     size_t size_to_free = start_of_header->size;

//     // Update the bytes freed
//     current_free += size_to_free;
//     current_allocated -= size_to_free;
//     num_deallocs += 1;

//     // Add freed block back to the free list
//     // node_t *free_block = list_head;
//     node_t *free_block = (node_t *)((char *)ptr - sizeof(node_t)); // Pointer to the free block header
//     printf("Free_block size: %zu\n", free_block->size);
//     node_t *check_addr = ((char *)free_block + free_block->size);
//     free_block->size = size_to_free;

//     // printf("Freeing block of size %zu at address %p\n", size_to_free, start_of_header);

//     node_t *current = list_head;
//     printf("Current_block size: %zu\n", current->size);
//     node_t *prev = NULL;
//     // node_t *new_node = (node_t *)((char *)current + current->size);

//     // Merge adjacent blocks (if any)
//     while (current != NULL)
//     {
//         // Check if the current block is adjacent to the freed block (free block follows current block)
//         if ((char *)current + current->size == (char *)free_block)
//         {
//             current->size += free_block->size; // Merge the blocks
//             free_block = current;              // Update free_block to the merged block
//             printf("Merged with previous block\n");
//             break;
//         }
//         //Check if the freed block follows the current block
//         else if ((char *)free_block + free_block->size == (char *)current)
//         {
//             free_block->size += current->size; // Merge the blocks
//             if (prev != NULL)
//                 prev->next = current->next; // Remove current block from the list
//             printf("Merged with next block\n");
//             continue;
//         }

//         prev = current;
//         current = current->next;
//     }

//     if (prev == NULL)
//     {
//         free_block->next = list_head;
//         list_head = free_block;
//     }
//     else
//     {
//         prev->next = free_block;
//         free_block->next = current;
//     }

//     // Print free list after operation (for debugging purposes)
//     calculate_fragmentation();
//     print_free_list();
// }

// void *urealloc(void *ptr, size_t new_size)
// {
//     if (ptr == NULL)
//     {
//         // If ptr is NULL, behave like malloc (allocate a new block)
//         return umalloc(new_size);
//     }

//     // Get the header for the current block
//     header_t *current_header = (header_t *)((char *)ptr - sizeof(header_t));

//     if (current_header->magic != MAGIC)
//     {
//         fprintf(stderr, "Error: Memory corruption detected at block %p\n", ptr);
//         exit(1);
//     }

//     size_t old_size = current_header->size;

//     // If the new size is smaller or equal, we can shrink or just return the pointer
//     if (new_size <= old_size)
//     {
//         // If shrinking, make sure the excess space is returned to the free list
//         if (new_size < old_size)
//         {
//             // Optionally split the block if there's enough free space left
//             size_t remaining_size = old_size - new_size;
//             if (remaining_size >= MIN_BLOCK_SIZE)
//             {
//                 node_t *new_free_block = (node_t *)((char *)ptr + new_size);
//                 new_free_block->size = remaining_size;
//                 // Insert the new block into the free list (you can use best/worst/first fit)
//                 allocate_block(new_free_block, new_size);
//             }
//         }
//         return ptr;
//     }

//     // If the new size is larger, we need to allocate a new block and copy the data
//     void *new_ptr = umalloc(new_size);
//     if (new_ptr == NULL)
//     {
//         return NULL; // If allocation failed, return NULL
//     }

//     // Copy the data from the old block to the new block manually
//     size_t copy_size = old_size < new_size ? old_size : new_size; // Copy the smaller of the old and new sizes
//     char *old_block = (char *)ptr;
//     char *new_block = (char *)new_ptr;

//     for (size_t i = 0; i < copy_size; ++i)
//     {
//         new_block[i] = old_block[i]; // Manual byte-by-byte copy
//     }

//     // Free the old block
//     ufree(ptr);

//     return new_ptr;
// }

// void *allocate_block(node_t *block, size_t size)
// {
//     size_t rounded_size = size + sizeof(header_t);
//     size_t remaining_size = block->size - rounded_size;

//     ((header_t *)block)->magic = MAGIC;

//     // Save the next block before any modifications
//     node_t *saved_next = block->next;

//     if (remaining_size >= sizeof(node_t))
//     {
//         // Create new free block
//         node_t *new_node = (node_t *)((char *)block + rounded_size);
//         new_node->size = remaining_size;
//         block->size = rounded_size;

//         // Put new_node in the same position in the list that block was in
//         if (list_head == block)
//         {
//             new_node->next = saved_next;
//             list_head = new_node;
//         }
//         else
//         {
//             // Find the previous node
//             node_t *prev = list_head;
//             while (prev && prev->next != block)
//             {
//                 prev = prev->next;
//             }
//             if (prev)
//             {
//                 new_node->next = saved_next;
//                 prev->next = new_node;
//             }
//         }
//     }
//     else
//     {
//         // No split possible, just remove the block
//         if (list_head == block)
//         {
//             list_head = saved_next;
//         }
//         else
//         {
//             node_t *prev = list_head;
//             while (prev && prev->next != block)
//             {
//                 prev = prev->next;
//             }
//             if (prev)
//             {
//                 prev->next = saved_next;
//             }
//         }
//     }

//     current_allocated += rounded_size;
//     current_free -= rounded_size;
//     num_allocs++;

//     return (void *)((char *)block + sizeof(header_t));
// }