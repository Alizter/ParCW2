#ifndef ERR_H
#define ERR_H

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
-------- Error handling --------------------------------------------------------
*/

typedef enum
{
    FileException = -1,         // Program couldn't read file
    ArgNumException = -3,       // Program has wrong number of arguents
    ArrayReadFailure = -4,      // Program couldn't read array
    PrecisionException = -5,    // Program precision too small
    DimensionException = -6,    // Program dimension too small
    ThreadNumException = -7,    // Program threadNum too small
    DimStepException = -8       // Program dim step given too small
} Error;

/*
----- Error handling -----------------------------------------------------------
*/

// Prints error message (or Success) with arguments and exits
void throwError(Error e, char** args);

#endif
