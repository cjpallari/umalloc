#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include "umem.h"

node_t *free_list = NULL;

int umeminit(size_t sizeOfRegion, int allocationAlgo)
{
    // open the /dev/zero device
    int fd = open("/dev/zero", O_RDWR);

    // sizeOfRegion (in bytes) needs to be evenly divisible by the page size
    void *allocated_memory = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (allocated_memory == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }
    free_list = (node_t *)allocated_memory;
    free_list->size = sizeOfRegion;
    free_list->next = NULL;

    // close the device (don't worry, mapping should be unaffected)
    close(fd);
    return 0;
}

int main()
{
    size_t page_size = getpagesize();
    printf("Page size: %zu\n", page_size);
    return 0;
}