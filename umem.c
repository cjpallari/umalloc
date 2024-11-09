#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "umem.h"
#define MIN_BLOCK_SIZE 32

node_t *list_head = NULL;
node_t *small_free = NULL;
header_t *header = NULL;
node_t *last_allocation = NULL;
static int allocationAlgo;
int num_allocs = 0;
int num_deallocs = 0;
long unsigned int current_free = 0;
long unsigned int current_allocated = 0;
float fragmentation = 0.0;

int umeminit(size_t sizeOfRegion, int algo)
{
    if (list_head != NULL)
    {
        return 0;
    }

    allocationAlgo = algo;
    int fd = open("/dev/zero", O_RDWR);

    void *allocated_memory = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (allocated_memory == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    // set list head and size
    list_head = (node_t *)allocated_memory;
    list_head->size = sizeOfRegion;
    list_head->next = NULL; // set next to null
    // setting this for stat purposes
    current_free = sizeOfRegion;

    close(fd);
    return 0;
}

void *allocate_block(node_t *block, size_t size)
{

    size_t rounded_size = size + sizeof(header_t);
    size_t remaining_size = block->size - rounded_size;

    // save the next pointer before modifying the block
    node_t *saved_next = block->next;

    // now set the magic number
    ((header_t *)block)->magic = MAGIC;

    // handles block splitting
    if (remaining_size >= sizeof(node_t)) // should i be adding 8 bytes to this as well?
    {
        // create a new block at the end of the allocated block
        node_t *new_node = (node_t *)((char *)block + rounded_size);
        new_node->size = remaining_size;

        // if the block we're splitting is the head of the free list
        if (block == list_head)
        {
            new_node->next = saved_next; // Use saved_next pointer
            list_head = new_node;
        }
        else
        {
            new_node->next = saved_next; // Use saved_next pointer

            // set previous node to list head
            node_t *prev = list_head;
            // keep searching utnil we find block that points to our target
            while (prev && prev->next != block)
            {
                prev = prev->next;
            }
            // if we find it, connect it to our new free block
            if (prev)
            {
                prev->next = new_node;
            }
        }

        // set the size of the allocated block
        block->size = rounded_size;
    }
    // if block is not big enough to split
    else
    {
        // remove the whole block from the list
        if (block == list_head)
        {
            list_head = saved_next; // use saved_next pointer
        }
        else
        {
            node_t *prev = list_head;
            while (prev && prev->next != block)
            {
                prev = prev->next;
            }
            if (prev)
            {
                prev->next = saved_next; // use saved_next pointer
            }
        }
    }

    // update stats
    // it is my understanding that this should NOT include the size of the header
    current_allocated += size; // update with "actual" bytes returned to user
    current_free -= rounded_size;
    num_allocs++;

    return (void *)((char *)block + sizeof(header_t));
}

void *best(size_t size)
{
    // 8 byte alignment
    size_t required_size = ((size + sizeof(header_t) + 7) / 8) * 8;
    node_t *current = list_head;
    node_t *best_fit = NULL;

    // traverse free list
    while (current != NULL)
    {
        // if the block is bigger than the size request aligned for 8 bytes
        if (current->size >= required_size)
        {
            // if we haven't found a block yet or the current block is smaller than the best fit
            if (best_fit == NULL ||
                current->size < best_fit->size ||
                (current->size == best_fit->size && current < best_fit))
            {
                // set the best fit to the current block
                best_fit = current;
            }
        }
        current = current->next;
    }

    // if we found a block return it
    if (best_fit != NULL)
    {

        return allocate_block(best_fit, size);
    }
    return NULL;
}

void *worst(size_t size)
{

    // 8 byte alignment
    size_t required_size = ((size + sizeof(header_t) + 7) / 8) * 8;
    node_t *current = list_head;
    node_t *worst_fit = NULL;

    // traverse free list
    while (current != NULL)
    {
        // if the block is bigger than the size request aligned for 8 bytes
        if (current->size >= required_size)
        {
            // if we haven't found a block yet or the current block is bigger than the worst fit
            if (worst_fit == NULL || current->size > worst_fit->size)
            {
                worst_fit = current;
            }
        }
        current = current->next;
    }
    // if we found a block return it
    if (worst_fit != NULL)
    {
        return allocate_block(worst_fit, size);
    }
    return NULL;
}

void *first(size_t size)
{
    // 8 byte alignment
    size_t required_size = ((size + sizeof(header_t) + 7) / 8) * 8;
    node_t *current = list_head;
    node_t *first = NULL;

    // traverse free list
    while (current != NULL)
    {
        // if the block is bigger than the size request aligned for 8 bytes
        if (current->size >= required_size)
        {
            // set the first block that fits the size
            first = current;
            break;
        }
        current = current->next;
    }

    // if we found a block return it
    if (first != NULL)
    {
        return allocate_block(first, size);
    }
    return NULL;
}

void *next(size_t size)
{

    // 8 byte alignment
    size_t required_size = ((size + sizeof(header_t) + 7) / 8) * 8;

    if (list_head == NULL)
        return NULL;

    // if last_allocation is NULL or no longer in the free list, start from head
    if (last_allocation == NULL)
    {
        last_allocation = list_head;
    }
    else
    {
        // check if last_allocation is still in the free list
        node_t *check = list_head;
        bool found = false;
        while (check != NULL)
        {
            // if list head is the last block we allocated
            if (check == last_allocation)
            {
                found = true;
                break;
            }
            check = check->next;
        }
        // if we don't find the last allocation in the free list, set it to the head
        if (!found)
        {
            last_allocation = list_head;
        }
    }

    node_t *start = last_allocation;
    node_t *current = start;

    do
    {
        // checking to see what the last allocation was
        // if the current block is big enough for our request
        if (current->size >= required_size)
        {
            // save where to start next search before we modify the block
            if (current->next != NULL)
            {
                last_allocation = current->next;
            }
            else
            {
                last_allocation = list_head;
            }

            return allocate_block(current, size);
        }

        // if there's a next block then move to it, otherwise current is the head
        if (current->next != NULL)
        {
            current = current->next;
        }
        else
        {
            current = list_head;
        }

    } while (current != start);

    // if we get here, we've gone through the whole list without finding space
    last_allocation = NULL; // Reset for next time
    return NULL;
}

void *umalloc(size_t size)
{
    void *allocated_memory = NULL;
    // 8 byte alignment
    size_t aligned_size = ((size + 7) / 8) * 8;

    if (aligned_size == 0) // handle 0 size
    {
        return NULL;
    }

    // determine which algorithm we use
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

    // set header if allocation was successful
    if (allocated_memory != NULL)
    {
        header_t *allocated_header = (header_t *)((char *)allocated_memory - sizeof(header_t));
    }
    return allocated_memory;
}

float calculate_fragmentation()
{
    node_t *current = list_head;
    size_t largest_free = 0;
    size_t mem_in_small_blocks = 0;

    //  find the largest free block
    while (current != NULL)
    {
        if (largest_free < current->size)
        {
            largest_free = current->size;
        }
        current = current->next;
    }

    // set threshold for small blocks
    size_t small_block_def = largest_free / 2;
    current = list_head;

    // calculate fragmentation
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

void validate_free_ptr(void *ptr, header_t *header)
{
    if (header->magic != MAGIC)
    {
        fprintf(stderr, "Error: Memory corruption detected at block %p\n", ptr);
        exit(1);
    }
    node_t *check = list_head;
    while (check != NULL)
    {
        if ((void *)check == (void *)header)
        {
            fprintf(stderr, "Error: Double free detected at block %p\n", ptr);
            exit(1);
        }
        check = check->next;
    }
}

void update_free_stats(size_t size_to_free)
{
    current_free += size_to_free;
    // current_allocated -= size_to_free;
    current_allocated -= (size_to_free - sizeof(header_t)); // update with "actual" bytes returned to user
    num_deallocs += 1;
}

void merge_with_next_blocks(node_t *current)
{
    // iterate through the free list and merge with the next block if possible
    while (current->next != NULL &&
           (char *)current + current->size == (char *)current->next)
    {
        current->size += current->next->size; // merge the two blocks
        current->next = current->next->next;  // set the next pointer to the next next block
    }
}

void insert_block(node_t *free_block, node_t *current, node_t *prev)
{
    // insert the block in the free list in address order
    if (prev == NULL)
    {
        free_block->next = list_head;
        list_head = free_block;
    }
    else // insert in the middle
    {
        prev->next = free_block;
        free_block->next = current;
    }
}

node_t *init_free_block(header_t *header, size_t size)
{
    // initialize a free block
    node_t *free_block = (node_t *)header;
    free_block->size = size;
    free_block->next = NULL;
    return free_block;
}

void ufree(void *ptr)
{
    // if ptr is NULL, return
    if (ptr == NULL)
        return;

    // get the header for the current block
    header_t *header = (header_t *)((char *)ptr - sizeof(header_t));
    validate_free_ptr(ptr, header);

    // check if the block is already in the free list
    node_t *check = list_head;
    while (check != NULL)
    {
        if ((void *)check == (void *)header)
        {
            fprintf(stderr, "Error: Double free detected at block %p\n", ptr);
            exit(1);
        }
        check = check->next;
    }

    // get the size of the block to free
    size_t size_to_free = header->size;
    update_free_stats(size_to_free);

    // initialize a free block
    node_t *free_block = init_free_block(header, size_to_free);

    // if the free list is empty, add the block to the head
    if (list_head == NULL)
    {
        list_head = free_block;
        calculate_fragmentation();

        return;
    }

    node_t *current = list_head;
    node_t *prev = NULL;

    // iterate through the free list
    while (current != NULL)
    {
        // Check if we can merge with the previous block
        if ((char *)current + current->size == (char *)free_block)
        {
            current->size += free_block->size;

            // Check if we can also merge with the next block
            merge_with_next_blocks(current);
            calculate_fragmentation();

            return;
        }

        // Check if we can merge with the next block
        if ((char *)free_block + free_block->size == (char *)current)
        {
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

            // After merging with next, check if we can merge with previous
            if (prev != NULL && (char *)prev + prev->size == (char *)free_block)
            {
                prev->size += free_block->size;
                prev->next = free_block->next;
            }

            calculate_fragmentation();

            return;
        }

        // Insert in address order if no merging is possible
        if ((char *)free_block < (char *)current)
        {
            insert_block(free_block, current, prev);
            calculate_fragmentation();

            return;
        }

        prev = current;
        current = current->next;
    }

    // Add to end if we get here
    prev->next = free_block;
    calculate_fragmentation();
}

void validate_realloc_ptr(void *ptr, header_t *header)
{
    // magic number check
    if (header->magic != MAGIC)
    {
        fprintf(stderr, "Error: Memory corruption detected at block %p\n", ptr);
        exit(1);
    }
}

void shrink_block(header_t *current_header, size_t aligned_new_size, size_t old_size)
{
    // get the new free block
    node_t *new_free_block = (node_t *)((char *)current_header + aligned_new_size);
    new_free_block->size = old_size - aligned_new_size;
    new_free_block->next = NULL;
    current_header->size = aligned_new_size;

    // update stats
    current_allocated -= new_free_block->size;
    current_free += new_free_block->size;

    // Add to free list in order
    if (list_head == NULL || (char *)list_head > (char *)new_free_block)
    {
        new_free_block->next = list_head;
        list_head = new_free_block;
    }
    else // insert in the middle
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

void *urealloc(void *ptr, size_t new_size)
{
    // if ptr is NULL, just allocate new block
    if (ptr == NULL)
    {
        return umalloc(new_size);
    }

    // if new size is 0, free the block and return NULL
    if (new_size == 0)
    {
        ufree(ptr);
        return NULL;
    }

    // get the header for the current block
    header_t *current_header = (header_t *)((char *)ptr - sizeof(header_t));

    validate_realloc_ptr(ptr, current_header); // validate the pointer

    size_t old_size = current_header->size;
    size_t aligned_new_size = ((new_size + sizeof(header_t) + 7) / 8) * 8; // 8 byte alignment

    // if the new size is smaller or equal, we can shrink or just return the pointer
    if (aligned_new_size <= old_size)
    {
        // shrinking the block
        if (aligned_new_size + sizeof(node_t) <= old_size)
        {
            shrink_block(current_header, aligned_new_size, old_size); // shrink the block
        }
        return ptr;
    }

    // save the data before freeing
    char saved_data[old_size - sizeof(header_t)];
    char *old_data = (char *)ptr;
    for (size_t i = 0; i < old_size - sizeof(header_t); i++)
    {
        saved_data[i] = old_data[i];
    }

    // free the old block first
    ufree(ptr);

    // allocate new block
    void *new_ptr = umalloc(new_size);
    if (new_ptr == NULL)
    {
        // allocation failed - need to restore old state
        // need to reallocate the old block
        void *restored_ptr = umalloc(old_size - sizeof(header_t));
        if (restored_ptr != NULL)
        {
            char *restore_data = (char *)restored_ptr;
            for (size_t i = 0; i < old_size - sizeof(header_t); i++)
            {
                restore_data[i] = saved_data[i]; // restore the data
            }
        }
        return restored_ptr;
    }

    // copy saved data to new block
    char *new_data = (char *)new_ptr;
    for (size_t i = 0; i < old_size - sizeof(header_t); i++)
    {
        new_data[i] = saved_data[i]; // copy the data
    }

    return new_ptr;
}

// reset memory allocation stats
void reset_values()
{
    num_allocs = 0;
    num_deallocs = 0;
    current_allocated = 0;
    current_free = 0;
    fragmentation = 0.0;
    list_head = NULL;
    last_allocation = NULL;
}