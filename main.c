#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>


#include "parcw2.h"


/*
--------- Main program ---------------------------------------------------------
*/

// Program takes in dim, fileName and precision
int main(int argc, char** argv)
{
    // Initialise MPI
    // Here all workers will run this code
    MPI_Init(&argc, &argv);
    
    //
    //  Parsing arguments
    //

    // Default values
    int dim = 5;
    double inputPrecision = 1E-3;
    
    //char* outFileName = "results";
    
    // Command line options
    int c;    
    while ((c = getopt(argc, argv, "d:o:p:")) != -1)
    {
        switch(c)
        {            
            case 'd': // dimension argument
            {
                dim = atoi(optarg);
                
                if (dim <= 0)
                {
                    throwError(DimensionException, NULL);
                }
                
                break;
            }
            case 'p': // precision argument
            {
                inputPrecision = strtod(optarg, NULL);
    
                // If our precision is too small, our program may never stop
                if (inputPrecision <= 0.0)
                {
                    throwError(PrecisionException, NULL);
                }
                
                break;
            }
            case 'o': // Output file specified
            {
                //outFileName = optarg;
                break;
            }
        }
    }
    
    // Reading non-optional arguments
    
    // If the total number of arguments isn't the number of optional arguments
    // plus the number of non-optional arguments, then we have a problem.
    if (argc != 1 + optind)
    {
        throwError(ArgNumException, NULL);
    }
    
    // Get file name and increment pointer for next args if needed
    char* fileName = argv[optind++];
    

    printf("Computation begun...");
    
    // A struct may need to be passed to parIterateMPI in order to
    // get timing info
    
    // Compute new values
    
    parIterateMPI(fileName, dim, inputPrecision);
    
    printf("Done.\n");

    // Finalize MPI
    MPI_Finalize();

    
    // Exit the program Successfully
    printf("Program finished.\n");
    exit(0);
}

