/* 
    Unit testing
    
We unit test using the minimalist unit testing MinUnit which can be found here:

    http://www.jera.com/techinfo/jtns/jtn002.html
    
*/

/* MinUnit */
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                            if (message) return message; } while (0)
extern int tests_run;
/* ******* */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "parcw.h"



int tests_run = 0;


/*
int foo = 7;
int bar = 4;

static char* test_foo() 
{
    mu_assert("error, foo != 7", foo == 7);
    return 0;
}

static char* test_bar() 
{
    mu_assert("error, bar != 5", bar == 5);
    return 0;
}

*/

/*
------- Array data structure tests --------------------------------------------- 
*/

// Test duplicateMatrix()
static char* test_duplicateMatrix()
{
    SquareMatrix* matrix = malloc(sizeof(matrix));
    
    
    double arr[25] = 
        {3, 234, 2, 12, -34, 
         5, 3, 235, 2, 356, 
         1, 6, 8, 5, 32, 
         6, 3, 6, 32, 57, 
         234, 3,4, 6, 5}; 
         
    matrix->array = arr;
    
    matrix->dim = 5;

    mu_assert("error, duplicateMatrix != original", 
        eqSquareMatrix(matrix, duplicateMatrix(matrix)));
    
    return 0;

}

// test partitionBlocks on 4by4 for 4 threads
static char* test_partitionBlocks_4By4_4thr()
{
    const int thrNum = 4;
    int dim = 4;
    ThreadArgs args[thrNum];
    
    partitionBlocks(args, thrNum, dim);
       
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on start 0",  args[0].start == 0);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on end 0",    args[0].end == 4);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on start 1",  args[1].start == 4);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on end 1",    args[1].end == 8);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on start 2",  args[2].start == 8);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on end 2",    args[2].end == 12);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on start 3",  args[3].start == 12);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on end 3",    args[3].end == 16);
    
    return 0;    
}

// test partitionBlocks on 4by4 for 3 threads
static char* test_partitionBlocks_4By4_3thr()
{
    const int thrNum = 3;
    int dim = 4;
    ThreadArgs args[thrNum];
    
    partitionBlocks(args, thrNum, dim);
       
    mu_assert("test_partitionBlocks_4By4_to_3thr failed on start 0",  args[0].start == 0);
    mu_assert("test_partitionBlocks_4By4_to_3thr failed on end 0",    args[0].end == 5);
    mu_assert("test_partitionBlocks_4By4_to_3thr failed on start 1",  args[1].start == 5);
    mu_assert("test_partitionBlocks_4By4_to_3thr failed on end 1",    args[1].end == 10);
    mu_assert("test_partitionBlocks_4By4_to_3thr failed on start 2",  args[2].start == 10);
    mu_assert("test_partitionBlocks_4By4_to_3thr failed on end 2",    args[2].end == 16);
    
    return 0;
}



// Run all tests
static char* all_tests() 
{
    mu_run_test(test_duplicateMatrix);
    mu_run_test(test_partitionBlocks_4By4_4thr);
    mu_run_test(test_partitionBlocks_4By4_3thr);
    
    return 0;
}

// Main program
int main() 
{
    char* result = all_tests();

    if (result != 0) 
    {
        printf("%s\n", result);
    }
    else 
    {
        printf("ALL TESTS PASSED\n");
    }

    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
