#include "umem.h"
#include "umem.c"
#include <stdio.h>

void basic_first_fit_test()
{
    printf("\n=== Testing FIRST_FIT Algorithm ===\n");
    umeminit(4096, FIRST_FIT);

    // test 1: Allocate some memory blocks and free them
    void *ptr1 = umalloc(512);
    void *ptr2 = umalloc(1024);
    void *ptr3 = umalloc(256);

    // test 2: Free the allocated blocks
    ufree(ptr1);
    ufree(ptr2);
    ufree(ptr3);

    // test 3: Allocate again after freeing
    void *ptr4 = umalloc(128); // should allocate block size of 144
    void *ptr5 = umalloc(256); // should allocate block size of 272

    // resulting total allocated memory should be 416 bytes

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n");
    printf("=========================================");
    printf("\n");
}

void intensive_first_fit_test()
{
    printf("\n=== Intensive Testing of FIRST_FIT Algorithm ===\n");
    umeminit(4096, FIRST_FIT);

    void *ptr1 = umalloc(123);
    void *ptr2 = umalloc(507);
    void *ptr3 = umalloc(221);
    void *ptr4 = umalloc(1031); // Prime number
    void *ptr5 = umalloc(249);
    void *ptr6 = umalloc(61); // Prime number

    // Test realloc with odd sizes
    ptr1 = urealloc(ptr1, 187);
    ptr4 = urealloc(ptr4, 1503);

    ufree(ptr2);
    ufree(ptr3);

    void *ptr7 = umalloc(513);
    void *ptr8 = umalloc(127);  // One less than power of 2
    void *ptr9 = umalloc(1025); // One more than power of 2

    ufree(ptr1);
    ufree(ptr5);

    void *ptr10 = umalloc(129); // One more than power of 2
    void *ptr11 = umalloc(511); // One less than power of 2
    void *ptr12 = umalloc(255); // One less than power of 2

    // Free everything except ptr13-16
    ufree(ptr4);
    ufree(ptr6);
    ufree(ptr7);
    ufree(ptr8);
    ufree(ptr9);
    ufree(ptr10);
    ufree(ptr11);
    ufree(ptr12);

    void *ptr13 = umalloc(17); // Prime number
    void *ptr14 = umalloc(3);  // Prime number
    void *ptr15 = umalloc(25);
    void *ptr16 = umalloc(1023); // One less than power of 2

    ufree(ptr14);

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n=========================================\n");
}

// void basic_best_fit_test()
// {
//     printf("=== Testing BEST_FIT Algorithm ===\n");
//     umeminit(4096, BEST_FIT);

//     // Allocate several blocks
//     int *ptr1 = umalloc(256);
//     int *ptr2 = umalloc(128); // Best fit should choose a smaller block if available
//     int *ptr3 = umalloc(512); // Allocate a larger block

//     // Free some blocks
//     ufree(ptr2);
//     ufree(ptr1);

//     // Allocate again after some deallocations
//     int *ptr4 = umalloc(150);
//     int *ptr5 = umalloc(100);

//     // Final cleanup
//     ufree(ptr3);
//     ufree(ptr4);
//     ufree(ptr5);
//     printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
//     printf("\n");
//     printf("=========================================");
//     printf("\n");
// }

void basic_best_fit_test()
{
    printf("\n=== Basic BEST_FIT Test ===\n");
    umeminit(1000, BEST_FIT); // Small size for easier tracking

    // Test 1: Basic allocations
    printf("\nTest 1 - Basic allocations:\n");
    void *ptr1 = umalloc(50);  // Not aligned
    void *ptr2 = umalloc(64);  // Already aligned
    void *ptr3 = umalloc(100); // Not aligned

    printf("ptr1 (50): %p\n", ptr1);
    printf("ptr2 (64): %p\n", ptr2);
    printf("ptr3 (100): %p\n", ptr3);
    print_free_list();

    // Test 2: Free middle block and try new allocation
    printf("\nTest 2 - Free middle block:\n");
    ufree(ptr2);
    print_free_list();

    // Should use ptr2's space if alignment is working correctly
    void *ptr4 = umalloc(60); // Not aligned
    printf("ptr4 (60): %p\n", ptr4);
    print_free_list();

    // Clean up
    ufree(ptr1);
    ufree(ptr3);
    ufree(ptr4);
    print_free_list();

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
}

void basic_next_fit_test()
{
    printf("\n=== Testing NEXT_FIT Algorithm ===\n");
    umeminit(4096, NEXT_FIT);

    printf("Initial free list:\n");

    // test 1: Allocate memory blocks in sequence
    void *ptr1 = umalloc(100);
    void *ptr2 = umalloc(200);
    void *ptr3 = umalloc(300);

    // test 2: free in different order
    ufree(ptr2);

    ufree(ptr1); // free first block - should coalesce

    // test 3: Allocate with wrap-around
    void *ptr4 = umalloc(150); // Should use coalesced space
    void *ptr5 = umalloc(50);  // Should use remaining space

    // test 4: Free all and verify coalescing
    ufree(ptr3);
    ufree(ptr4);
    ufree(ptr5);

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n");
    printf("=========================================");
    printf("\n");
}

void intensive_best_fit_test()
{
    printf("\n=== Intensive Testing of BEST_FIT Algorithm ===\n");
    umeminit(4096, BEST_FIT);

    // malloc with odd sizes
    void *ptr1 = umalloc(509);
    void *ptr2 = umalloc(1027);
    void *ptr3 = umalloc(253);
    void *ptr4 = umalloc(129);
    void *ptr5 = umalloc(65);
    void *ptr6 = umalloc(519);
    void *ptr7 = umalloc(251);
    void *ptr8 = umalloc(127);
    void *ptr9 = umalloc(61);
    void *ptr10 = umalloc(131);

    // free some blocks
    ufree(ptr1);
    ufree(ptr3);
    ufree(ptr5);
    ufree(ptr7);
    ufree(ptr9);

    // try to realloc some blocks with odd sizes
    ptr2 = urealloc(ptr2, 1503);
    ptr4 = urealloc(ptr4, 95);
    ptr6 = urealloc(ptr6, 777);

    // malloc with odd sizes
    void *ptr11 = umalloc(63);
    void *ptr12 = umalloc(135);
    void *ptr13 = umalloc(507);

    ufree(ptr11);
    ufree(ptr13);

    // malloc with odd sizes
    void *ptr14 = umalloc(123);
    void *ptr15 = umalloc(249);

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n=========================================\n");
}

void next_fit_edge_test()
{
    printf("\n=== Testing NEXT_FIT Edge Cases ===\n");
    umeminit(4096, NEXT_FIT);

    // test 1: Fill most of memory, leaving small gaps
    void *ptr1 = umalloc(2000);
    void *ptr2 = umalloc(1000);
    void *ptr3 = umalloc(500);

    // test 2: Free middle block and try to allocate something larger
    ufree(ptr2);
    void *ptr4 = umalloc(1200);

    // test 3: Try to allocate remaining space
    printf("\nAllocating remaining space:\n");
    void *ptr5 = umalloc(100);

    // Clean up
    ufree(ptr1);
    ufree(ptr3);
    ufree(ptr4);
    ufree(ptr5);

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n");
    printf("=========================================");
    printf("\n");
}

void intensive_next_fit_test()
{
    printf("\n=== Intensive Testing of NEXT_FIT Algorithm ===\n");
    umeminit(4096, NEXT_FIT);

    // dd sizes to test alignment
    void *ptr1 = umalloc(511);
    void *ptr2 = umalloc(1023);
    void *ptr3 = umalloc(255);
    void *ptr4 = umalloc(517);
    void *ptr5 = umalloc(127);
    void *ptr6 = umalloc(67);
    void *ptr7 = umalloc(259);
    void *ptr8 = umalloc(127);
    void *ptr9 = umalloc(63);
    void *ptr10 = umalloc(129);

    // free some blocks
    ufree(ptr1);
    ufree(ptr3);
    ufree(ptr5);
    ufree(ptr7);
    ufree(ptr9);

    // malloc again with random sizes
    void *ptr11 = umalloc(61);
    void *ptr12 = umalloc(133);
    void *ptr13 = umalloc(511);

    // free some more blocks
    ufree(ptr11);
    ufree(ptr13);

    // malloc again with random sizes
    void *ptr14 = umalloc(125);
    void *ptr15 = umalloc(257);

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n=========================================\n");
}

void edge_case_tests()
{
    printf("\n=== Testing Edge Cases ===\n");

    // test Case 1: NULL and zero-size tests
    umeminit(4096, BEST_FIT);

    void *null_free_test = NULL;
    ufree(null_free_test); // Should handle NULL gracefully

    void *zero_alloc = umalloc(0); // Should handle zero size

    void *null_realloc = urealloc(NULL, 100); // Should act like malloc

    urealloc(null_realloc, 0); // Should act like free

    // test Case 2: Alignment tests
    void *ptr1 = umalloc(3);
    void *ptr2 = umalloc(7);
    void *ptr3 = umalloc(8);
    void *ptr4 = umalloc(9);

    // test Case 3: Realloc edge cases
    void *realloc_ptr = umalloc(100);

    // Grow the block
    realloc_ptr = urealloc(realloc_ptr, 200);

    // Shrink the block
    realloc_ptr = urealloc(realloc_ptr, 50);

    // Grow to a size that forces a new allocation
    realloc_ptr = urealloc(realloc_ptr, 1000);

    // test Case 4: Boundary conditions
    void *max_alloc = umalloc(4000); // Nearly max size

    void *should_fail = umalloc(4096); // Should fail - too large

    // Print final stats
    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n");
    printf("=========================================");
    printf("\n");
}

void fragmentation_test()
{
    printf("\n=== Testing Fragmentation Handling ===\n");
    umeminit(4096, BEST_FIT);

    // Create a pattern of alternating small and large allocations
    void *ptrs[10];
    for (int i = 0; i < 10; i++)
    {
        if (i % 2 == 1)
        {
            ptrs[i] = umalloc(64);
        }
        else
        {
            ptrs[i] = umalloc(256);
        }
    }

    // Free every other block to create fragmentation
    for (int i = 0; i < 10; i += 2)
    {
        ufree(ptrs[i]);
    }

    // try to allocate a large block that should fail due to fragmentation
    void *large_fail = umalloc(512);

    // Print stats to see fragmentation
    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);

    // Now free everything and try again
    for (int i = 1; i < 10; i += 2)
    {
        ufree(ptrs[i]);
    }

    large_fail = umalloc(512); // Should succeed now

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n");
    printf("=========================================");
    printf("\n");
}

void realloc_stress_test()
{
    printf("\n=== Testing Realloc Stress Cases ===\n");
    umeminit(4096, BEST_FIT);

    // test 1: Growing and shrinking same block repeatedly
    void *ptr = umalloc(64);

    for (int i = 0; i < 5; i++)
    {
        ptr = urealloc(ptr, 128 * (i + 1)); // Grow
        ptr = urealloc(ptr, 64);            // Shrink back
    }

    // test 2: Realloc between existing blocks
    void *blocks[3];
    blocks[0] = umalloc(128);
    blocks[1] = umalloc(256);
    blocks[2] = umalloc(128);

    // Free middle block and try to realloc first block into that space
    ufree(blocks[1]);
    blocks[0] = urealloc(blocks[0], 384); // Should use middle space

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n");
    printf("=========================================");
    printf("\n");
}

void double_free_test()
{
    umeminit(4096, FIRST_FIT);
    int *ptr = umalloc(20);
    ufree(ptr);
    ufree(ptr); // should result in an error

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
}

int main()
{
    // basic_first_fit_test();
    // reset_values();

    // intensive_first_fit_test();
    // reset_values();

    // basic_best_fit_test();
    // reset_values();

    // intensive_best_fit_test();
    // reset_values();

    // basic_next_fit_test();
    // reset_values();

    // intensive_next_fit_test();
    // reset_values();

    // next_fit_edge_test();
    // reset_values();

    // fragmentation_test();
    // reset_values();

    // realloc_stress_test();
    // reset_values();

    // edge_case_tests();
    // reset_values();

    // double_free_test();
    basic_best_fit_test();
    return 0;
}