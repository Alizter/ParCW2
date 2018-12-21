#include <mpi.h>

#include "parcw.h"

#define SM SquareMatrix

// MPI version of parIterate


void parIterateMPI(SM* old, SM* new, double prec, int thrNum);
