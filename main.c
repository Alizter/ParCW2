#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>


#include "parcw.h"


/*
--------- Main program ---------------------------------------------------------
*/

// Program takes in dim, fileName and precision
int main(int argc, char** argv)
{
    // Default values
    int dim = 5;
    double inputPrecision = 1E-3;
    int thrNum = 4;
    int colour = 0;
    
    int timingMode = 0;

    // Command line options
    int c;    
    while ((c = getopt(argc, argv, "cd:p:t:T")) != -1)
    {
        switch(c)
        {            
            case 'd': // dimension argument
            {
                dim = atoi(optarg);
                
                if (dim <= 0)
                {
                    throw(DimensionException, NULL);
                }
                
                break;
            }
            case 'p': // precision argument
            {
                inputPrecision = strtod(optarg, NULL);
    
                // If our precision is too small, our program may never stop
                if (inputPrecision <= 0.0)
                {
                    throw(PrecisionException, NULL);
                }
                
                break;
            }
            case 't': // thread number argument
            {
                thrNum = atoi(optarg);
    
                if (thrNum <= 0)
                {
                    throw(ThreadNumException, NULL);
                }
                
                break;
            }
            case 'c': // colour enabled
            {
                colour = 1;
                break;
            }
            case 'T': // Timing mode enabled
            {
                timingMode = 1;
                break;
            }
        }
    }
    
    // Reading non-optional arguments
    
    // If the total number of arguments isn't the number of optional arguments
    // plus the number of non-optional arguments, then we have a problem.
    if (argc != 1 + optind)
    {
        throw(ArgNumException, NULL);
    }
    
    // Get file name and increment pointer for next args if needed
    char* fileName = argv[optind++];
    
    // Argument reading finished, file reading begins
    
    // Read input array
    SquareMatrix* inputArray = readMatrix(dim, fileName);
    
    // New and old matricies
    // New would be calculated from old
    SquareMatrix* old = duplicateMatrix(inputArray);
    SquareMatrix* new = duplicateMatrix(inputArray);
    
    
    
    // If timing mode is enabled
    if (timingMode)
    {    
//        clock_t startTime, diff, end;
        
        struct timespec start, finish;
		double time;

        // Naive iterate //     
        
        // Start timing
        clock_gettime(CLOCK_MONOTONIC, &start);
        
        naiveIterate(old, new, inputPrecision);
        
        // End timing
        clock_gettime(CLOCK_MONOTONIC, &finish);
        
        // Compute elapsed time
	    time = (double)(finish.tv_sec - start.tv_sec);
        time += (double)(finish.tv_nsec - start.tv_nsec) / 1E9;
        
		printf("Naive iterate:\t\t\t%f s\n", time);

        // Parellel iterate //
        
   
        // Initialise matricies
        old = duplicateMatrix(inputArray);
        new = duplicateMatrix(inputArray);
        
        clock_gettime(CLOCK_MONOTONIC, &start);

        parIterate(old, new, inputPrecision, thrNum);

        clock_gettime(CLOCK_MONOTONIC, &finish);

		double naiveTime = time;

        time = (double)(finish.tv_sec - start.tv_sec);
        time += (double)(finish.tv_nsec - start.tv_nsec) / 1E9;
        
		printf("Parallel iterate %d threads:\t%f s\tSpeedup: %f\n", 
			thrNum, time, naiveTime / (time));

        freeMatrix(old);
        freeMatrix(new);
    
    }
    else // Compute old and display
    {
        // Print input array
        printf("Input array:\n");    
        printMatrix(inputArray, colour);
        printf("\n");
    
        // Compute new values
        parIterate(old, new, inputPrecision, thrNum);
        
        // Print new array
        printf("Result:\n");
        printMatrix(new, colour);
    }
    
    // Exit the program Successfully
    printf("Program finished.\n");
    exit(0);
}

