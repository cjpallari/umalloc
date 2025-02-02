#include "umem.h"
#include "umem.c"
#include <stdio.h>

void basic_first_fit_test()
{
<<<<<<< HEAD
=======
    /*
     * function: basic_first_fit_test
     * ----------------------------
     * tests the basic functionality of the first-fit allocation algorithm.
     *
     * test cases:
     * 1. allocates three blocks of decreasing size to test basic allocation
     *    - tests if first-fit properly finds the first available space
     *    - verifies proper block splitting and alignment
     *
     * 2. frees all blocks in order to test deallocation
     *    - tests if blocks are properly returned to free list
     *    - verifies coalescing of adjacent free blocks
     *
     * 3. allocates two new blocks after freeing
     *    - tests if freed space can be reused
     *    - verifies that allocator finds first suitable block
     *    - final allocated memory should be 384 bytes (user space only)
     *
     * expected behavior:
     * - should maintain proper alignment (8-byte)
     * - should properly track allocated vs free memory
     * - should show 0% fragmentation when blocks are contiguous
     */
>>>>>>> afbf9b4... umalloc final
    printf("\n=== Testing FIRST_FIT Algorithm ===\n");
    umeminit(4096, FIRST_FIT);

    // test 1: allocate some memory blocks that should play nicely
    void *ptr1 = umalloc(512);
    void *ptr2 = umalloc(1024);
    void *ptr3 = umalloc(256);

    // test 2: free the allocated blocks
    ufree(ptr1);
    ufree(ptr2);
    ufree(ptr3);

    // test 3: allocate again after freeing
    void *ptr4 = umalloc(128);
    void *ptr5 = umalloc(256);

    // resulting total allocated memory should be 384 bytes

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n");
    printf("=========================================");
    printf("\n");
}

void intensive_first_fit_test()
{
<<<<<<< HEAD
=======
    /*
     * function: intensive_first_fit_test
     * ----------------------------
     * stress tests the first-fit allocator with odd sizes and complex patterns.
     *
     * test cases:
     * 1. multiple allocations with odd sizes
     *    - tests handling of non-power-of-2 sizes
     *    - tests alignment with irregular sizes
     *    - verifies proper padding
     *
     * 2. realloc operations with odd sizes
     *    - tests block resizing with unusual sizes
     *    - verifies data preservation during realloc
     *
     * 3. mixed allocation and free pattern
     *    - tests fragmentation handling
     *    - verifies proper block reuse
     *    - tests coalescing under complex conditions
     *
     * expected behavior:
     * - should handle odd sizes with proper alignment
     * - should maintain free list integrity during complex operations
     * - should properly coalesce blocks when possible
     */
>>>>>>> afbf9b4... umalloc final
    printf("\n=== Intensive Testing of FIRST_FIT Algorithm ===\n");
    umeminit(4096, FIRST_FIT);

    // malloc with odd sizes
    void *ptr1 = umalloc(123);
    void *ptr2 = umalloc(507);
    void *ptr3 = umalloc(221);
    void *ptr4 = umalloc(1031);
    void *ptr5 = umalloc(249);
    void *ptr6 = umalloc(61);

    // test realloc with odd sizes
    ptr1 = urealloc(ptr1, 187);
    ptr4 = urealloc(ptr4, 1503);

    // free some blocks
    ufree(ptr2);
    ufree(ptr3);

    // malloc with odd sizes
    void *ptr7 = umalloc(513);
    void *ptr8 = umalloc(127);
    void *ptr9 = umalloc(1025);

    // free some blocks
    ufree(ptr1);
    ufree(ptr5);

    // malloc with odd sizes
    void *ptr10 = umalloc(129);
    void *ptr11 = umalloc(511);
    void *ptr12 = umalloc(255);

    // free everything except ptr13-16
    ufree(ptr4);
    ufree(ptr6);
    ufree(ptr7);
    ufree(ptr8);
    ufree(ptr9);
    ufree(ptr10);
    ufree(ptr11);
    ufree(ptr12);

    // malloc random sizes
    void *ptr13 = umalloc(17);
    void *ptr14 = umalloc(3);
    void *ptr15 = umalloc(25);
    void *ptr16 = umalloc(1023);

    ufree(ptr14);

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n=========================================\n");
}

void basic_best_fit_test()
{
<<<<<<< HEAD
=======
    /*
     * function: basic_best_fit_test
     * ----------------------------
     * tests the fundamental operations of the best-fit allocation strategy.
     *
     * test cases:
     * 1. sequential allocations of varying sizes
     *    - tests if best-fit chooses optimal blocks
     *    - verifies block splitting behavior
     *
     * 2. strategic block freeing
     *    - creates specific hole sizes to test selection
     *    - verifies proper block choice among multiple options
     *
     * 3. allocation after freeing
     *    - tests if allocator chooses most suitable blocks
     *    - verifies minimal space wastage
     *
     * expected behavior:
     * - should select smallest suitable block for each request
     * - should minimize internal fragmentation
     * - should maintain proper block linkage
     */
>>>>>>> afbf9b4... umalloc final
    printf("=== Testing BEST_FIT Algorithm ===\n");
    umeminit(4096, BEST_FIT);

    // allocate some blocks
    int *ptr1 = umalloc(256);
    int *ptr2 = umalloc(128); // best fit should choose a smaller block if available
    int *ptr3 = umalloc(512); // allocate a larger block

    // free some blocks
    ufree(ptr2);
    ufree(ptr1);

    // allocate again after some deallocations
    int *ptr4 = umalloc(150);
    int *ptr5 = umalloc(100);

    // cleanup
    ufree(ptr3);
    ufree(ptr4);
    ufree(ptr5);
    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n");
    printf("=========================================");
    printf("\n");
}

void basic_next_fit_test()
{
<<<<<<< HEAD
=======
    /*
     * function: basic_next_fit_test
     * ----------------------------
     * tests the basic functionality of the next-fit allocation strategy.
     *
     * test cases:
     * 1. sequential allocations
     *    - tests if allocator properly tracks last position
     *    - verifies correct advancement through free list
     *
     * 2. mixed free operations
     *    - tests coalescing of adjacent blocks
     *    - verifies proper next-position updates after frees
     *
     * 3. wrap-around allocations
     *    - tests if allocator properly wraps to list start
     *    - verifies correct position tracking after wrap
     *
     * expected behavior:
     * - should maintain correct 'last position' pointer
     * - should properly wrap around to list start
     * - should coalesce blocks when possible
     */
>>>>>>> afbf9b4... umalloc final
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

    // test 3: allocate with wrap-around
    void *ptr4 = umalloc(150); // Should use coalesced space
    void *ptr5 = umalloc(50);  // Should use remaining space

    // test 4: free all and verify coalescing
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
<<<<<<< HEAD
=======
    /*
     * function: intensive_best_fit_test
     * ----------------------------
     * stress tests the best-fit allocator with challenging allocation patterns.
     *
     * test cases:
     * 1. multiple allocations with odd sizes
     *    - tests handling of prime numbers and odd sizes
     *    - verifies alignment with irregular requests
     *    - tests memory utilization with varied sizes
     *
     * 2. strategic free operations
     *    - creates specific fragmentation patterns
     *    - tests ability to find best fits in fragmented memory
     *
     * 3. realloc with size changes
     *    - tests block resizing with different size changes
     *    - verifies proper block reuse and splitting
     *    - tests data preservation during reallocation
     *
     * expected behavior:
     * - should handle odd-sized allocations efficiently
     * - should maintain optimal block selection under fragmentation
     * - should properly track memory usage statistics
     */
>>>>>>> afbf9b4... umalloc final
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
<<<<<<< HEAD
=======
    /*
     * function: next_fit_edge_test
     * ----------------------------
     * tests boundary conditions and edge cases of next-fit allocation.
     *
     * test cases:
     * 1. near-capacity allocations
     *    - fills most of memory leaving small gaps
     *    - tests behavior when space is highly constrained
     *
     * 2. strategic middle block free
     *    - tests allocation behavior with fragmented space
     *    - verifies proper handling of larger requests
     *
     * 3. final space allocation
     *    - tests allocator's behavior with minimal remaining space
     *    - verifies proper handling of failed allocations
     *
     * expected behavior:
     * - should handle near-full memory conditions
     * - should properly track available space
     * - should maintain correct next-fit position under stress
     */
>>>>>>> afbf9b4... umalloc final
    printf("\n=== Testing NEXT_FIT Edge Cases ===\n");
    umeminit(4096, NEXT_FIT);

    // test 1: fill most of memory, leaving small gaps
    void *ptr1 = umalloc(2000);
    void *ptr2 = umalloc(1000);
    void *ptr3 = umalloc(500);

    // test 2: free middle block and try to allocate something larger
    ufree(ptr2);
    void *ptr4 = umalloc(1200);

    // test 3: try to allocate remaining space
    printf("\nAllocating remaining space:\n");
    void *ptr5 = umalloc(100);

    // free everything
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
<<<<<<< HEAD
=======
    /*
     * function: intensive_next_fit_test
     * ----------------------------
     * thoroughly tests next-fit allocator with complex allocation patterns.
     *
     * test cases:
     * 1. odd-sized allocations
     *    - tests handling of non-standard sizes
     *    - verifies alignment with irregular requests
     *    - tests memory utilization
     *
     * 2. mixed free operations
     *    - creates specific fragmentation patterns
     *    - tests position tracking after frees
     *    - verifies proper wrap-around behavior
     *
     * 3. repeated allocation after frees
     *    - tests block reuse patterns
     *    - verifies position advancement
     *    - tests coalescing under complex conditions
     *
     * expected behavior:
     * - should maintain correct position tracking
     * - should handle odd sizes with proper alignment
     * - should show efficient space reuse
     */
>>>>>>> afbf9b4... umalloc final
    printf("\n=== Intensive Testing of NEXT_FIT Algorithm ===\n");
    umeminit(4096, NEXT_FIT);

    // odd sizes to test alignment
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
<<<<<<< HEAD
=======
    /*
     * function: edge_case_tests
     * ----------------------------
     * tests various edge cases and boundary conditions across allocation methods.
     *
     * test cases:
     * 1. null and zero-size handling
     *    - tests null pointer handling in free
     *    - verifies zero-size allocation behavior
     *    - tests null pointer handling in realloc
     *
     * 2. alignment verification
     *    - tests 8-byte alignment with various sizes
     *    - verifies proper padding
     *
     * 3. realloc edge cases
     *    - tests growing and shrinking blocks
     *    - verifies behavior with large size changes
     *
     * 4. boundary testing
     *    - tests allocations near memory region limits
     *    - verifies proper handling of failed allocations
     *
     * expected behavior:
     * - should handle all edge cases gracefully
     * - should maintain alignment requirements
     * - should properly handle error conditions
     */
>>>>>>> afbf9b4... umalloc final
    printf("\n=== Testing Edge Cases ===\n");

    // test Case 1: NULL and zero-size tests
    umeminit(4096, BEST_FIT);

    void *null_free_test = NULL;
    ufree(null_free_test); // should handle NULL gracefully

    void *zero_alloc = umalloc(0); // should handle zero size

    void *null_realloc = urealloc(NULL, 100); // should act like malloc

    urealloc(null_realloc, 0); // should act like free

    // test Case 2: alignment tests
    void *ptr1 = umalloc(3);
    void *ptr2 = umalloc(7);
    void *ptr3 = umalloc(8);
    void *ptr4 = umalloc(9);

    // test Case 3: realloc edge cases
    void *realloc_ptr = umalloc(100);

    // grow the block
    realloc_ptr = urealloc(realloc_ptr, 200);

    // shrink the block
    realloc_ptr = urealloc(realloc_ptr, 50);

    // grow to a size that forces a new allocation
    realloc_ptr = urealloc(realloc_ptr, 1000);

    // test Case 4: Boundary conditions
    void *max_alloc = umalloc(4000); // nearly max size

    void *should_fail = umalloc(4096); // should fail - too large

    // print final stats
    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n");
    printf("=========================================");
    printf("\n");
}

void fragmentation_test()
{
<<<<<<< HEAD
=======
    /* function: fragmentation_test
     * ----------------------------
     * specifically tests memory fragmentation handling and measurement.
     *
     * test cases:
     * 1. alternating allocation sizes
     *    - creates pattern of small and large blocks
     *    - tests fragmentation calculation
     *
     * 2. strategic free pattern
     *    - creates intentional fragmentation
     *    - tests allocation in fragmented space
     *
     * 3. defragmentation test
     *    - tests coalescing effectiveness
     *    - verifies fragmentation measurement
     *
     * expected behavior:
     * - should accurately measure fragmentation
     * - should handle fragmented allocation attempts
     * - should properly coalesce when possible
     */
>>>>>>> afbf9b4... umalloc final
    printf("\n=== Testing Fragmentation Handling ===\n");
    umeminit(4096, BEST_FIT);

    // create a pattern of alternating small and large allocations
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

    // free every other block to create fragmentation
    for (int i = 0; i < 10; i += 2)
    {
        ufree(ptrs[i]);
    }

    // try to allocate a large block that should fail due to fragmentation
    void *large_fail = umalloc(512);

    // print stats to see fragmentation
    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);

    // free everything and try again
    for (int i = 1; i < 10; i += 2)
    {
        ufree(ptrs[i]);
    }

    large_fail = umalloc(512); // should succeed

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n");
    printf("=========================================");
    printf("\n");
}

void realloc_stress_test()
{
<<<<<<< HEAD
=======
    /*
     * function: realloc_stress_test
     * ----------------------------
     * tests realloc functionality under various conditions.
     *
     * test cases:
     * 1. repeated grow/shrink cycles
     *    - tests repeated resizing of same block
     *    - verifies memory content preservation
     *    - tests fragmentation handling
     *
     * 2. realloc between blocks
     *    - tests realloc with surrounding allocations
     *    - verifies proper space reuse
     *    - tests coalescing during realloc
     *
     * expected behavior:
     * - should preserve data during realloc
     * - should efficiently handle size changes
     * - should maintain proper block organization
     */
>>>>>>> afbf9b4... umalloc final
    printf("\n=== Testing Realloc Stress Cases ===\n");
    umeminit(4096, BEST_FIT);

    // test 1: growing and shrinking same block repeatedly
    void *ptr = umalloc(64);

    for (int i = 0; i < 5; i++)
    {
        ptr = urealloc(ptr, 128 * (i + 1)); // grow
        ptr = urealloc(ptr, 64);            // shrink back
    }

    // test 2: realloc between existing blocks
    void *blocks[3];
    blocks[0] = umalloc(128);
    blocks[1] = umalloc(256);
    blocks[2] = umalloc(128);

    // free middle block and try to realloc first block into that space
    ufree(blocks[1]);
    blocks[0] = urealloc(blocks[0], 384); // Should use middle space

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
    printf("\n");
    printf("=========================================");
    printf("\n");
}

void double_free_test()
{
<<<<<<< HEAD
=======
    /*
     * function: double_free_test
     * ----------------------------
     * tests detection and handling of double free attempts.
     *
     * test cases:
     * 1. basic double free
     *    - allocates and frees same block twice
     *    - tests error detection
     *    - verifies proper error handling
     *
     * expected behavior:
     * - should detect double free attempt
     * - should exit with error message
     * - should prevent memory corruption
     */
>>>>>>> afbf9b4... umalloc final
    umeminit(4096, FIRST_FIT);
    int *ptr = umalloc(20);
    ufree(ptr);
    ufree(ptr); // should result in an error

    printumemstats(num_allocs, num_deallocs, current_allocated, current_free, fragmentation);
}

int main()
{
    basic_first_fit_test();
    reset_values();

    intensive_first_fit_test();
    reset_values();

    basic_best_fit_test();
    reset_values();

    intensive_best_fit_test();
    reset_values();

    basic_next_fit_test();
    reset_values();

    intensive_next_fit_test();
    reset_values();

    next_fit_edge_test();
    reset_values();

    fragmentation_test();
    reset_values();

    realloc_stress_test();
    reset_values();

    edge_case_tests();
    reset_values();

    double_free_test();
    return 0;
}