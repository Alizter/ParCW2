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
    
    int timingMode = 0;
    char* outFileName = "results";
    char* timingFileName = "timings";

    // Command line options
    int c;    
    while ((c = getopt(argc, argv, "d:o:p:t:T:P")) != -1)
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
            /*
            case 'c': // colour enabled
            {
                colour = 1;
                break;
            }
            */
            case 'T': // Timing mode enabled
            {
                timingMode = 1;
                timingFileName = optarg;
                break;
            }
            case 'o': // Output file specified
            {
                free(outFileName);
                outFileName = optarg;
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
        int dimStep = 5;
        
        // Open files
        FILE* timingFile = fopen(timingFileName, "ab+");
            
    
        // Iterate through dimensions in steps of dimStep
        for (int i = 1; i <= dim / dimStep; i++)
        {
            
            if (timingFile == NULL)
            {
                // Could not read file so throw error
                char* args[1] = { timingFileName };
                throw(FileException, args);
            }
            
        
            // Get the new dimension
            int ndim = i * dimStep;
            
            // write dimension
            fprintf(timingFile, "%d,", ndim);
            
            old = resizeMatrix(inputArray, ndim);
            new = resizeMatrix(inputArray, ndim);
        
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
            
            //printf("Naive iterate:\t\t\t%f s\n", time);
            fprintf(timingFile, "%f,", time);


            //
            //  Parallel iterate
            //
       
            // Go through each thread number
            for (int j = 1; i < thrNum; j++)
            {
                // Initialise matricies
                old = resizeMatrix(inputArray, ndim);
                //new = resizeMatrix(inputArray, ndim);
                
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
    }
    else // Compute old and display
    {
        // Print input array
        //printf("Input array:\n");    
        //printMatrix(inputArray, colour);
        //printf("\n");
    
        printf("Computation begun... ");
    
        // Compute new values
        parIterate(old, new, inputPrecision, thrNum);
        
        printf("Done.\nWriting to file...");
        
        // TODO: Write to results file
        FILE* outFile = fopen(outFileName, "ab+");
        fprintMatrix(outFile, new);
        fclose(outFile);
        
        printf("Done.\n");
        
        // Print new array
        //printf("Result:\n");
        //printMatrix(new, colour);
    }
    
    // Exit the program Successfully
    printf("Program finished.\n");
    exit(0);
}

