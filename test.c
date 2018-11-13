/* 
    Unit testing
    
    We unit test using the minimalist unit testing MinUnit which can be 
    found here:

    http://www.jera.com/techinfo/jtns/jtn002.html
    
    
    The main function is at the end and simply runs all the unit tests 
    (functions that begin with test_...).
*/

/* MinUnit */
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                            if (message) return message; } while (0)
extern int tests_run;
/* ******* */

#include "parcw.h"

// A count for the number of tests run
int tests_run = 0;

/*
------- Array data structure tests --------------------------------------------- 
*/

// Test matrix equality when equal
static char* test_eqSquareMatrix_equal()
{
    SquareMatrix* a = newMatrix(5);
    SquareMatrix* b = newMatrix(5);
    
    double a_arr[25] = 
        {3, 3, 2, 12, -34, 
         5, 3, 235, 2, 12, 
         1, 6, 8, 5, 32, 
         6, 3, 6, 32, 57, 
         234, 3, 4, 6, 5};
    
    double b_arr[25] = 
        {3, 3, 2, 12, -34, 
         5, 3, 235, 2, 12, 
         1, 6, 8, 5, 32, 
         6, 3, 6, 32, 57, 
         234, 3, 4, 6, 5};
    
    a->array = a_arr;
    b->array = b_arr;
    
    mu_assert("error equal matricies are unequal", 
        eqSquareMatrix(a, b));
    
    return 0;
}


// Test matrix equality when unequal
static char* test_eqSquareMatrix_unequal()
{
    SquareMatrix* a = newMatrix(5);
    SquareMatrix* b = newMatrix(5);
    
    double a_arr[25] = 
        {3, 3, 2, 12, -34, 
         5, 3, 235, 2, 12, 
         1, 6, 8, 5, 32, 
         6, 3, 6, 32, 57, 
         234, 3, 4, 6, 5};
         
    double b_arr[25] = 
        {3, 3, 2, 24, -34, 
         5, 3, 235, 2, 12, 
         1, 6, 8, 5, 32, 
         6, 3, 6, 234, 57, 
         234, 3, 4, 6, 5};
    
    a->array = a_arr;
    b->array = b_arr;
    
    mu_assert("error unequal matricies are equal", 
        !eqSquareMatrix(a, b));
        
    return 0;
}

// Test matrix equality when dim unequal
static char* test_eqSquareMatrix_dim_unequal()
{
    SquareMatrix* a = newMatrix(5);
    SquareMatrix* b = newMatrix(4);
    
    double a_arr[25] = 
        {3, 3, 2, 12, -34, 
         5, 3, 235, 2, 12, 
         1, 6, 8, 5, 32, 
         6, 3, 6, 32, 57, 
         234, 3, 4, 6, 5};
         
    double b_arr[25] = 
        {3, 3, 2, 12, -34, 
         5, 3, 235, 2, 12, 
         1, 6, 8, 5, 32, 
         6, 3, 6, 32, 57, 
         234, 3, 4, 6, 5};
         
    a->array = a_arr;
    b->array = b_arr;
    
    mu_assert("error unequal dim matricies are equal", 
        !eqSquareMatrix(a, b));

    return 0;
}

// Test duplicateMatrix()
static char* test_duplicateMatrix()
{
    SquareMatrix* matrix = newMatrix(5);
    
    double arr[25] = 
        {3, 234, 2, 12, -34, 
         5, 3, 235, 2, 356, 
         1, 6, 8, 5, 32, 
         6, 3, 6, 32, 57, 
         234, 3, 4, 6, 5}; 
         
    matrix->array = arr;
    

    mu_assert("error, duplicateMatrix != original", 
        eqSquareMatrix(matrix, duplicateMatrix(matrix)));
    
    return 0;

}

// Test resizeMatrix()
static char* test_resizeMatrix()
{
    SquareMatrix* matrix = newMatrix(5);
    
    double arr[25] = 
        {3, 234, 2, 12, -34, 
         5, 3, 235, 2, 356, 
         1, 6, 8, 5, 32, 
         6, 3, 6, 32, 57, 
         234, 3, 4, 6, 5}; 
         
    matrix->array = arr;
    
    SquareMatrix* real = newMatrix(4);
    real->array = arr;
    
    SquareMatrix* new = resizeMatrix(matrix, 4);
    
    mu_assert("error, resized matrix dimension wrong", new->dim == real->dim);
    mu_assert("error, resized matrix unexpected", eqSquareMatrix(new, real));
    
    return 0;
}

// test partitionBlocks on 4by4 for 4 threads
static char* test_partitionBlocks_4By4_4thr()
{
    const int thrNum = 4;
    int dim = 4;
    ThreadArgs args[thrNum];
    
    partitionBlocks(args, thrNum, dim);
       
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on start 0",  
        args[0].start == 0);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on end 0",    
        args[0].end == 4);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on start 1",  
        args[1].start == 4);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on end 1",    
        args[1].end == 8);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on start 2",  
        args[2].start == 8);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on end 2",    
        args[2].end == 12);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on start 3",  
        args[3].start == 12);
    mu_assert("test_partitionBlocks_4By4_to_4thr failed on end 3",    
        args[3].end == 16);
    
    return 0;    
}

// test partitionBlocks on 4by4 for 3 threads
static char* test_partitionBlocks_4By4_3thr()
{
    const int thrNum = 3;
    int dim = 4;
    ThreadArgs args[thrNum];
    
    partitionBlocks(args, thrNum, dim);
       
    mu_assert("test_partitionBlocks_4By4_to_3thr failed on start 0",  
        args[0].start == 0);
    mu_assert("test_partitionBlocks_4By4_to_3thr failed on end 0",    
        args[0].end == 5);
    mu_assert("test_partitionBlocks_4By4_to_3thr failed on start 1",  
        args[1].start == 5);
    mu_assert("test_partitionBlocks_4By4_to_3thr failed on end 1",    
        args[1].end == 10);
    mu_assert("test_partitionBlocks_4By4_to_3thr failed on start 2",  
        args[2].start == 10);
    mu_assert("test_partitionBlocks_4By4_to_3thr failed on end 2",    
        args[2].end == 16);
    
    return 0;
}

/*
------ Implementations tests ---------------------------------------------------
*/

// tests naive iterate
static char* test_naiveIterate()
{
    double prec = 1E-5;
    
    SquareMatrix* matrix = newMatrix(5);
      
    double arr[25] = 
        {3, 234, 2, 12, -34, 
         5, 3, 235, 2, 356, 
         1, 6, 8, 5, 32, 
         6, 3, 6, 32, 57, 
         234, 3, 4, 6, 5}; 
    matrix->array = arr;

    SquareMatrix* old = duplicateMatrix(matrix);
    SquareMatrix* new = duplicateMatrix(matrix);
    
    SquareMatrix* expected = newMatrix(5);
    
    double arrEx[25] =
        {3.000000, 234.000000, 2.000000, 12.000000, -34.000000, 
        5.000000, 85.370535, 64.446429, 123.102678, 356.000000, 
        1.000000, 38.035714, 47.312499, 59.964286, 32.000000, 
        6.000000, 18.459821, 26.803572, 37.441964, 57.000000, 
        234.000000, 3.000000, 4.000000, 6.000000, 5.000000 };
    
    expected->array = arrEx;
    
    naiveIterate(old, new, prec);
    
    mu_assert("naiveIterate failed", eqPrecSquareMatrix(new, expected, prec));
        
    return 0;
}


/*
------ main testing program ----------------------------------------------------
*/

// Run all tests
static char* all_tests() 
{ 
    mu_run_test(test_eqSquareMatrix_dim_unequal);
    mu_run_test(test_eqSquareMatrix_unequal);
    mu_run_test(test_eqSquareMatrix_equal);

    mu_run_test(test_duplicateMatrix);
    mu_run_test(test_resizeMatrix);
    
    mu_run_test(test_partitionBlocks_4By4_4thr);
    mu_run_test(test_partitionBlocks_4By4_3thr);
    
    mu_run_test(test_naiveIterate);
    
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
