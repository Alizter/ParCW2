#ifndef PAR2_H
#define PAR2_H

#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include "error.h"

#define SM SquareMatrix

// MPI version of parIterate


void parIterateMPI(char* fileName, int dim, double prec, char* outFileName);
void readMatrix(int dim, char* fileName, double* matrix);

typedef struct 
{
    int firstRow;  // First row 
    int count;     // Number of elements
} IndexData ;

#endif
