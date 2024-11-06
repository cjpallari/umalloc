#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include "umem.h"

node_t *list_head = NULL;
header_t *header = NULL;
static int allocationAlgo;
int num_allocs = 0;
int num_deallocs = 0;
long unsigned int bytes_freed = 0;
long unsigned int bytes_allocated = 0;
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
    // header->magic = MAGIC;
    list_head->next = NULL;

    // close the device (don't worry, mapping should be unaffected)
    close(fd);
    return 0;
}

void *allocate_block(node_t *block, size_t size)
{
    // Round the requested size up to the nearest multiple of 16
    // size_t rounded_size = (size + 15) & ~15;

    size_t rounded_size = size + sizeof(header_t);
    // size_t rounded_size = (size + sizeof(header_t) + 15) & ~15;

    size_t remaining_size = block->size - rounded_size; // sizeof(node_t) represents the size of the header

    // printf("allocate_block(): block->size = %zu, rounded_size = %zu, remaining_size = %zu\n", block->size, rounded_size, remaining_size);

    if (remaining_size >= 16) // Ensure we have enough space for a new block of at least 16 bytes
    {
        node_t *new_node = (node_t *)((char *)block + rounded_size);
        new_node->size = remaining_size; // Set the size of the new block
        new_node->next = block->next;

        printf("Created new block at %p with size %zu\n", new_node, new_node->size);

        list_head->size = new_node->size;

        // block->size = rounded_size; // The block is now allocated with the rounded size
        // block->next = new_node;     // The remaining free block becomes the next node

        // printf("Updated block: block->size = %zu\n", block->size);
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

    // Could also be part of a list of allocated blocks
    // bytes_allocated += rounded_size;
    // num_allocs++;
    print_free_list();
    return (void *)((char *)block + sizeof(node_t)); // Return pointer to the allocated memory
}

void *best(size_t size)
{
    node_t *current = list_head;
    node_t *best_fit = NULL;

    // Traverse the linked list until we hit a null value, updating the best_fit variable as we go
    while (current != NULL)
    {
        if (current->size >= size)
        {
            if (best_fit == NULL || current->size < best_fit->size)
            {
                best_fit = current;
            }
        }
        current = current->next;
    }
    if (best_fit != NULL)
    {
        return allocate_block(best_fit, size);
    }
    return NULL;
}

void *worst(size_t size)
{
    node_t *current = list_head;
    node_t *worst_fit = NULL;

    while (current != NULL)
    {
        if (current->size >= size)
        {
            worst_fit = current;
            break;
            // if (worst_fit == NULL || current->size > worst_fit->size)
            // {
            //     worst_fit = current;
            // }
        }
        current = current->next;
    }
    if (worst_fit != NULL)
    {
        allocate_block(worst_fit, size);
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
        }
        current = current->next;
    }
    if (first != NULL)
    {
        allocate_block(first, size);
    }
}

void *next(size_t size)
{
}

void *umalloc(size_t size)
{
    void *allocated_memory = NULL;
    size_t aligned_size = ((size + 15) / 16) * 16;

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
        // Now the header is part of the allocated memory and you can track it
        // For example, add it to an allocated list if needed

        // Add the allocated block to the allocated memory list
        num_allocs++;
        bytes_allocated += aligned_size;
    }

    return allocated_memory;
}

void ufree(void *ptr)
{
    if (ptr == NULL)
    {
        return; // Handle null pointer case
    }

    // Assuming `header_t` is defined as in your `umem.h` and is placed before the allocated memory
    header_t *header = (header_t *)((char *)ptr - sizeof(header_t));

    // Get the size from the header
    size_t size_to_free = header->size;

    // Update the bytes freed
    bytes_freed += size_to_free;

    // Add freed block back to the free list
    node_t *free_block = (node_t *)((char *)ptr - sizeof(node_t)); // Pointer to the free block header
    free_block->size = size_to_free;

    printf("Freeing block of size %zu at address %p\n", size_to_free, ptr);

    node_t *current = list_head;
    node_t *prev = NULL;

    // Merge adjacent blocks (if any)
    while (current != NULL)
    {
        // Check if the current block is adjacent to the freed block (free block follows current block)
        if ((char *)current + current->size == (char *)free_block)
        {
            current->size += free_block->size; // Merge the blocks
            free_block = current;              // Update free_block to the merged block
            printf("Merged with previous block\n");
            break;
        }
        // Check if the freed block follows the current block
        else if ((char *)free_block + free_block->size == (char *)current)
        {
            free_block->size += current->size; // Merge the blocks
            if (prev != NULL)
                prev->next = current->next; // Remove current block from the list
            printf("Merged with next block\n");
            break;
        }

        prev = current;
        current = current->next;
    }

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

    // while (current != NULL && current->next != NULL)
    // {
    //     current = current->next;
    // }

    // // If there's a valid last block, make sure its next pointer is NULL
    // if (current != NULL)
    // {
    //     current->next = NULL; // Ensures the last block has no circular reference
    // }

    // Print free list after operation (for debugging purposes)
    print_free_list();
}

void *urealloc(void *ptr, size_t size)
{
    // ptr points to free list
}

int main()
{
    // Page size is 4096
    umeminit(4096, FIRST_FIT);
    print_free_list();
    int *ptr2 = umalloc(15);
    int *ptr3 = umalloc(30);
    ufree(ptr2);
    // printumemstats(num_allocs, num_deallocs, bytes_allocated, bytes_freed, fragmentation);
    return 0;
}

/*
NOTES FOR TOMORROW
-------------------

-allocate_block should be working correctly now with the exception of the allocated block still showing in the free list. not sure if there's a way around this without
editing the macro which is a no no

-



// void *allocate_block(node_t *block, size_t size)
// {
//     size_t remaining_size = block->size - size;

//     if (remaining_size >= sizeof(node_t) + 16)
//     {
//         node_t *new_node = (node_t *)((char *)block + size + sizeof(node_t));
//         new_node->size = remaining_size - sizeof(node_t);
//         new_node->next = block->next;

//         block->size = size;
//         block->next = new_node;
//     }
//     else
//     {
//         if (list_head == block)
//         {
//             list_head = block->next;
//         }
//         else
//         {
//             node_t *prev = list_head;
//             while (prev->next != block)
//             {
//                 prev = prev->next;
//             }
//             prev->next = block->next;
//         }
//     }
//     bytes_allocated += size;
//     num_allocs++;
//     print_free_list();
//     return (void *)((char *)block + sizeof(node_t));
// }
*/