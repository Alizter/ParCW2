#include "error.h"

/*
----- Error handling -----------------------------------------------------------
*/

// Prints error message (or Success) with arguments and exits
void throwError(Error e, char** args)
{
    switch (e)
    {        
        case FileException:
            printf("Error: Could not read file: %s\n", args[0]);
            break;
    
        case ArgNumException:
            printf("Error: Incorrect number of arguments given.\n");
            break;
        
        case ArrayReadFailure:
            printf("Error: Could not read array.\n");
            break;
            
        case PrecisionException:
            printf("Error: Given precision is too small. or invalid\n");
            break;
            
        case DimensionException:
            printf("Error: Given dimension is too small or invalid.\n");
            break;
            
        case ThreadNumException:
            printf("Error: Given thread number is too small or invalid.\n");
            break;
            
        case DimStepException:
            printf("Error: Given dimension step in parameter -T invalid.\n");
            break;
    }
    
    // Exit with error code

    // Finalize MPI
    MPI_Finalize();

    printf("Program exiting.\n");
    exit(e);
}

