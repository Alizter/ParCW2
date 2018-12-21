#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>


#include "parcw.h"
#include "parcw2.c"

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
    int dimStep = 5;
    
    int timingMode = 0;
    char* outFileName = "results";
    char* timingFileName = "timings";

    // Command line options
    int c;    
    while ((c = getopt(argc, argv, "d:o:p:t:T:")) != -1)
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
            case 'T': // Timing mode enabled
            {
                timingMode = 1;
                
                dimStep = atoi(optarg);
                
                if (dimStep <= 0)
                {
                    throw(DimStepException, NULL);
                }
                
                break;
            }
            case 'o': // Output file specified
            {
                outFileName = optarg;
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
    
    // If timing mode is enabled we want to time and record the program on
    // different values of dim upto and including one specified. We will write
    // the results to a csv file 
    if (timingMode)
    {   
        // Open files
        FILE* timingFile = fopen(timingFileName, "ab+");
        
        if (timingFile == NULL)
        {
            // Could not read file so throw error
            char* args[1] = { timingFileName };
            throw(FileException, args);
        } 
    
        printf("Begining computations:\n");
    
        // Iterate through dimensions in steps of dimStep
        for (int i = dimStep; i <= dim; i += dimStep)
        {            
            // write dimension
            fprintf(timingFile, "%d,", i);
            
            old = resizeMatrix(inputArray, i);
            new = resizeMatrix(inputArray, i);
        
            struct timespec start, finish;
            double time;

            //
            //  Naive iterate
            //
            
            // Start timing
            clock_gettime(CLOCK_MONOTONIC, &start);
            
            naiveIterate(old, new, inputPrecision);
            
            // End timing
            clock_gettime(CLOCK_MONOTONIC, &finish);
            
            // Compute elapsed time
            time = (double)(finish.tv_sec - start.tv_sec);
            time += (double)(finish.tv_nsec - start.tv_nsec) / 1E9;
            
            fprintf(timingFile, "%f,", time);

            //
            //  Parallel iterate
            //
            
            // Loading bar stuff
            int lbarSize = 24;
            char full[lbarSize];
            char empty[lbarSize];
            
            for (int n = 0; n < lbarSize; n++)
            {
                full[n] = '#';
                empty[n] = ' ';
            }
            // Go through each thread number
            for (int j = 1; j <= thrNum; j++)
            {
                double progress = fmin(1.0, ((double)((i-1) * thrNum + j) / 
                    ((thrNum) * (dim - dim % dimStep))));
                int prog1 = (int)(progress * lbarSize);
                int prog2 = lbarSize - prog1;
                double prog3 = 100 * progress;
            
                printf("\rDim: %d Threads: %d [%.*s%.*s] %3.3f%%", 
                    i, j, prog1, full, prog2, empty, prog3);
                fflush(stdout);
                
                // Initialise matricies
                old = resizeMatrix(inputArray, i);
                new = resizeMatrix(inputArray, i);
                
                // Start timing
                clock_gettime(CLOCK_MONOTONIC, &start);

                parIterate(old, new, inputPrecision, j);

                // End timing
                clock_gettime(CLOCK_MONOTONIC, &finish);

                // Save naive time for computing speedup
                //double naiveTime = time;

                // compute parallel timing
                time = (double)(finish.tv_sec - start.tv_sec);
                time += (double)(finish.tv_nsec - start.tv_nsec) / 1E9;
                
                //printf("Parallel iterate %d threads:\t%f s\tSpeedup: %f\n", 
                //   thrNum, time, naiveTime / (time));
                fprintf(timingFile, "%f,", time);

                freeMatrix(old);
                freeMatrix(new);
            }
            
            fprintf(timingFile, "\n");
        }
        
        fclose(timingFile);
        printf("\n");
    }
    else // Compute old and display
    {
        printf("Computation begun...");
    
        // Compute new values
        parIterate(old, new, inputPrecision, thrNum);
        
        printf("Done.\nWriting to file...");
        
        FILE* outFile = fopen(outFileName, "ab+");
        
        if (outFile == NULL)
        {
            // Could not read file so throw error
            char* args[1] = { outFileName };
            throw(FileException, args);
        }
        
        fprintMatrix(outFile, new);
        fclose(outFile);
        
        printf("Done.\n");
    }
    
    // Exit the program Successfully
    printf("Program finished.\n");
    exit(0);
}

