#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include "umem.h"
#define MIN_BLOCK_SIZE 32

node_t *list_head = NULL;
node_t *small_free = NULL;
header_t *header = NULL;
static int allocationAlgo;
int num_allocs = 0;
int num_deallocs = 0;
long unsigned int current_free = 0;
long unsigned int current_allocated = 0;
float fragmentation = 0.0;

void print_free_list()
{
    node_t *current = list_head;
    int count = 0; // Add a safeguard for infinite loops
    while (current != NULL && count < 100)
    { // Limit to 100 iterations
        printf("Block of size %zu -> ", current->size);
        current = current->next;
        count++;
    }
    if (count >= 100)
    {
        printf("Warning: Too many iterations, possibly a circular list.\n");
    }
    printf("NULL\n");
}

int umeminit(size_t sizeOfRegion, int algo)
{

    if (list_head != NULL)
    {
        return 0;
    }

    allocationAlgo = algo;
    // open the /dev/zero device
    int fd = open("/dev/zero", O_RDWR);

    // sizeOfRegion (in bytes) needs to be evenly divisible by the page size
    void *allocated_memory = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (allocated_memory == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }
    list_head = (node_t *)allocated_memory;
    list_head->size = sizeOfRegion;
    current_free = sizeOfRegion;
    list_head->next = NULL;
    // header->magic = MAGIC;

    // close the device (don't worry, mapping should be unaffected)
    close(fd);
    return 0;
}

void *allocate_block(node_t *block, size_t size)
{

    size_t rounded_size = size + sizeof(header_t);          // this will be the size of the block we are allocating
    size_t remaining_size = list_head->size - rounded_size; // this will be the size of the remaining free list

    ((header_t *)block)->magic = MAGIC;

    if (remaining_size >= sizeof(node_t)) // Ensure we have the minimum space remaining in the free list for future allocations
    {
        node_t *new_node = (node_t *)((char *)block + rounded_size);
        new_node->size = remaining_size; // Set the size of the new block
        block->size = rounded_size;
        list_head = new_node;

        // printf("Created new block at %p with size %zu\n", list_head, list_head->size);
    }
    else
    {
        // No split possible, so we remove the block from the free list
        if (list_head == block)
        {
            list_head = block->next;
        }
        else
        {
            node_t *prev = list_head;
            while (prev->next != block)
            {
                prev = prev->next;
            }
            prev->next = block->next;
        }
        printf("Block removed: block->size = %zu\n", block->size);
    }
    current_allocated += rounded_size;
    current_free -= rounded_size;
    num_allocs++;
    // print_free_list();
    return (void *)((char *)block + sizeof(header_t)); // Return pointer to the allocated memory
}

void *best(size_t size)
{
    // Calculate the total size needed including header and alignment
    size_t required_size = ((size + sizeof(header_t) + 7) / 8) * 8;

    node_t *current = list_head;
    node_t *best_fit = NULL;

    printf("\nBEST FIT searching for size: %zu\n", required_size);

    // Traverse the linked list until we hit a null value
    while (current != NULL)
    {
        printf("Checking block of size: %zu\n", current->size);

        if (current->size >= required_size)
        {
            if (best_fit == NULL || current->size < best_fit->size)
            {
                best_fit = current;
                printf("Found new best fit of size: %zu\n", best_fit->size);
            }
        }
        current = current->next;
    }

    if (best_fit != NULL)
    {
        printf("Selected best fit block of size: %zu for request of size: %zu\n",
               best_fit->size, required_size);
        return allocate_block(best_fit, size);
    }
    return NULL;
}

// void *best(size_t size)
// {
//     node_t *current = list_head;
//     node_t *best_fit = NULL;

//     // Traverse the linked list until we hit a null value, updating the best_fit variable as we go
//     while (current != NULL)
//     {
//         if (current->size >= size)
//         {
//             if (best_fit == NULL || current->size < best_fit->size)
//             {
//                 best_fit = current;
//             }
//         }
//         current = current->next;
//     }
//     if (best_fit != NULL)
//     {
//         return allocate_block(best_fit, size);
//     }
//     return NULL;
// }

void *worst(size_t size)
{
    node_t *current = list_head;
    node_t *worst_fit = NULL;

    while (current != NULL)
    {
        if (current->size >= size)
        {
            if (worst_fit == NULL || current->size > worst_fit->size)
            {
                worst_fit = current;
            }
        }
        current = current->next;
    }
    if (worst_fit != NULL)
    {
        return allocate_block(worst_fit, size);
    }
}

void *first(size_t size)
{
    node_t *current = list_head;
    node_t *first = NULL;

    while (current != NULL)
    {
        if (current->size >= size)
        {
            first = current;
            break;
        }
        current = current->next;
    }
    if (first != NULL)
    {
        return allocate_block(first, size);
    }
    return NULL;
}

// void *next(size_t size)
// {
//     if (list_head == NULL)
//         return NULL;

//     // Start from last allocation point or beginning if not set
//     node_t *start = last_allocation ? last_allocation : list_head;
//     node_t *current = start;

//     // Search from current position to end
//     do
//     {
//         if (current->size >= size)
//         {
//             last_allocation = current->next ? current->next : list_head;
//             return allocate_block(current, size);
//         }
//         current = current->next ? current->next : list_head;
//     } while (current != start);

//     return NULL;
// }

void *next(size_t size)
{
    // do something
}

void *umalloc(size_t size)
{
    void *allocated_memory = NULL;
    size_t aligned_size = ((size + 7) / 8) * 8;

    if (aligned_size == 0)
    {
        return NULL;
    }

    switch (allocationAlgo)
    {
    case BEST_FIT:
        allocated_memory = best(aligned_size);
        break;
    case WORST_FIT:
        allocated_memory = worst(aligned_size);
        break;
    case FIRST_FIT:
        allocated_memory = first(aligned_size);
        break;
    case NEXT_FIT:
        allocated_memory = next(aligned_size);
        break;
    default:
        return NULL;
    }
    // bytes_allocated += size;

    if (allocated_memory != NULL)
    {
        // If necessary, you could store allocated blocks in a separate list or process them here
        header_t *allocated_header = (header_t *)((char *)allocated_memory - sizeof(header_t));

        // num_allocs++;
        // current_allocated += aligned_size;
        // current_free -= aligned_size;
    }
    return allocated_memory;
}

float calculate_fragmentation()
{
    node_t *current = list_head;
    size_t largest_free = 0;
    size_t mem_in_small_blocks = 0;
    while (current != NULL)
    {
        if (largest_free < current->size)
        {
            largest_free = current->size;
        }
        current = current->next;
    }

    size_t small_block_def = largest_free / 2;
    current = list_head;
    while (current != NULL)
    {
        if (current->size < small_block_def)
        {
            mem_in_small_blocks += current->size;
        }
        current = current->next;
    }

    fragmentation = ((float)mem_in_small_blocks / (float)current_free) * 100.0;
    return fragmentation;
}

void ufree(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    // Get the header that precedes the user's pointer
    header_t *header = (header_t *)((char *)ptr - sizeof(header_t));

    if (header->magic != MAGIC)
    {
        fprintf(stderr, "Error: Memory corruption detected at block %p\n", ptr);
        exit(1);
    }

    size_t size_to_free = header->size;

    // Update statistics
    current_free += size_to_free;
    current_allocated -= size_to_free;
    num_deallocs += 1;

    // Convert the freed memory into a free list node
    node_t *free_block = (node_t *)header;
    free_block->size = size_to_free;
    free_block->next = NULL;

    // If the free list is empty, make this the first block
    if (list_head == NULL)
    {
        list_head = free_block;
        calculate_fragmentation();
        print_free_list();
        return;
    }

    // Find where to insert the block and handle merging
    node_t *current = list_head;
    node_t *prev = NULL;

    while (current != NULL)
    {
        // Check if we can merge with the previous block
        if ((char *)current + current->size == (char *)free_block)
        {
            // printf("Previous block -> Current: %p, size: %zu, end: %p\n",
            //        current, current->size, (char *)current + current->size);
            // printf("Free block address: %p\n", free_block);
            // current->size += free_block->size;

            // Now check if we can also merge with the next block
            if (current->next != NULL &&
                (char *)current + current->size == (char *)current->next)
            {
                current->size += current->next->size;
                current->next = current->next->next;
            }

            calculate_fragmentation();
            print_free_list();
            return;
        }

        // Check if we can merge with the next block
        if ((char *)free_block + free_block->size == (char *)current)
        {
            // printf("free_block address: %p, size: %zu, end: %p\n",
            //        free_block, free_block->size, (char *)free_block + free_block->size);
            // printf("current block address: %p\n", current);
            free_block->size += current->size;
            free_block->next = current->next;

            if (prev == NULL)
            {
                list_head = free_block;
            }
            else
            {
                prev->next = free_block;
            }

            calculate_fragmentation();
            print_free_list();
            return;
        }

        // If we can't merge, find the correct position to insert
        // Keep the free list ordered by address
        if ((char *)free_block < (char *)current)
        {
            if (prev == NULL)
            {
                free_block->next = list_head;
                list_head = free_block;
            }
            else
            {
                prev->next = free_block;
                free_block->next = current;
            }

            calculate_fragmentation();
            print_free_list();
            return;
        }

        prev = current;
        current = current->next;
    }

    // If we get here, add to the end of the list
    prev->next = free_block;

    calculate_fragmentation();
    print_free_list();
}

void *urealloc(void *ptr, size_t new_size)
{
    if (ptr == NULL)
    {
        return umalloc(new_size);
    }

    if (new_size == 0)
    {
        ufree(ptr);
        return NULL;
    }

    // Get the header for the current block
    header_t *current_header = (header_t *)((char *)ptr - sizeof(header_t));

    if (current_header->magic != MAGIC)
    {
        fprintf(stderr, "Error: Memory corruption detected at block %p\n", ptr);
        exit(1);
    }

    size_t old_size = current_header->size;
    size_t aligned_new_size = ((new_size + sizeof(header_t) + 7) / 8) * 8;

    // If the new size is smaller or equal, we can shrink or just return the pointer
    if (aligned_new_size <= old_size)
    {
        // Shrinking logic remains the same
        if (aligned_new_size + sizeof(node_t) <= old_size)
        {
            node_t *new_free_block = (node_t *)((char *)current_header + aligned_new_size);
            new_free_block->size = old_size - aligned_new_size;
            new_free_block->next = NULL;
            current_header->size = aligned_new_size;

            current_allocated -= new_free_block->size;
            current_free += new_free_block->size;

            // Add to free list in order
            if (list_head == NULL || (char *)list_head > (char *)new_free_block)
            {
                new_free_block->next = list_head;
                list_head = new_free_block;
            }
            else
            {
                node_t *current = list_head;
                while (current->next != NULL &&
                       (char *)current->next < (char *)new_free_block)
                {
                    current = current->next;
                }
                new_free_block->next = current->next;
                current->next = new_free_block;
            }
        }
        return ptr;
    }

    // If the new size is larger, we need to:
    // 1. Free the old block first (so it can potentially merge with adjacent blocks)
    // 2. Then try to allocate the new size (which might use the newly merged space)

    // Save the data before freeing
    char saved_data[old_size - sizeof(header_t)];
    char *old_data = (char *)ptr;
    for (size_t i = 0; i < old_size - sizeof(header_t); i++)
    {
        saved_data[i] = old_data[i];
    }

    // Free the old block first
    ufree(ptr);

    // Now allocate new block (might use merged space)
    void *new_ptr = umalloc(new_size);
    if (new_ptr == NULL)
    {
        // Allocation failed - need to restore old state
        // We'll need to reallocate the old block
        void *restored_ptr = umalloc(old_size - sizeof(header_t));
        if (restored_ptr != NULL)
        {
            char *restore_data = (char *)restored_ptr;
            for (size_t i = 0; i < old_size - sizeof(header_t); i++)
            {
                restore_data[i] = saved_data[i];
            }
        }
        return restored_ptr;
    }

    // Copy saved data to new block
    char *new_data = (char *)new_ptr;
    for (size_t i = 0; i < old_size - sizeof(header_t); i++)
    {
        new_data[i] = saved_data[i];
    }

    return new_ptr;
}