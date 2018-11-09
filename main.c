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
        clock_t startTime, diff, end;

        // Naive iterate //     
        
        
        // Initialise matricies
        freeMatrix(old);
        freeMatrix(new);
        
        old = duplicateMatrix(inputArray);
        new = duplicateMatrix(inputArray);
        
        startTime = clock(); // Start
        
        naiveIterate(old, new, inputPrecision);
        
        diff = clock() - startTime; // Finish
        printf("ST: %ld\tD: %ld\tC: %ld\n", startTime, diff, diff + startTime);
        
        //diff = end - startTime;

		double naiveTime = ((double)diff) / CLOCKS_PER_SEC;

		printf("Naive iterate:\t\t\t%f s\n", naiveTime);

        // Parellel iterate //
        
        // We test parallel iterate for threads numbers upto specified
        
        for (int i = 1; i <= thrNum; i++)
        {
        	//clock_t startTime, diff;
        
            // Initialise matricies
            old = duplicateMatrix(inputArray);
            new = duplicateMatrix(inputArray);
            
            //startTime = clock(); // Start
            
            struct timespec start, finish;

            clock_gettime(CLOCK_MONOTONIC, &start);

            parIterate(old, new, inputPrecision, i);

            clock_gettime(CLOCK_MONOTONIC, &finish);

            double time = (finish.tv_sec - start.tv_sec);
            time += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
            
            
            
                        
            //diff = clock() - startTime; // Finish
            
            //diff = end - startTime;

            //printf("ST: %ld\tD: %ld\tC: %ld\n", startTime, diff, clock());
			//double time = ((double)diff) / CLOCKS_PER_SEC;

			printf("Parallel iterate %d threads:\t%f s\tSpeedup: %f\n", 
				i, time, naiveTime / (time));

            freeMatrix(old);
            freeMatrix(new);
        }
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

