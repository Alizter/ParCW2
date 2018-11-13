#ifndef PARCW_H_
#define PARCW_H_

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/sysinfo.h>

/*
---- Array data structure ------------------------------------------------------
*/

typedef struct
{
    double* array;
    int dim;
} SquareMatrix;

SquareMatrix* duplicateMatrix(SquareMatrix* old);
void freeMatrix(SquareMatrix* matrix);
SquareMatrix* newMatrix(int dim);
SquareMatrix* resizeMatrix(SquareMatrix* old, int dim);
int eqSquareMatrix(SquareMatrix* a, SquareMatrix* b);
int eqPrecSquareMatrix(SquareMatrix* a, SquareMatrix* b, double prec);


/*
-------- Signaller -------------------------------------------------------------
*/

// "Signaller" data structure like a reverse semaphore
// instead of checking if the counter >0 we check if
// the counter is != targetValue
typedef struct 
{
    // The value that the counter wants to be (capacity of the room)
    int targetValue; 
    
    // The value the counter is (number of people in the room)
    int currentValue;
    
    // Conditional variable
    pthread_cond_t *pCond;
    
    // Mutex for the counter
    pthread_mutex_t *pMutex;
    
} Signaller;

Signaller* newSignaller(int targetValue);
void destroySignaller(Signaller* s);
void waitFor(Signaller* s);
void signal(Signaller* s);

SquareMatrix* readMatrix(int dim, char* fileName);
void printMatrix(SquareMatrix* matrix);
void fprintMatrix(FILE* file, SquareMatrix* matrix);

/*
------- Implementations --------------------------------------------------------
*/

// Data structure that gets passed to thread
typedef struct
{
    int start;                  //  Start point of partition
    int end;                    //  end point of parition
    
    double* diff;               // The difference between the iteration
    
    int id;                     // Thread id
    Signaller* signalWorker;    // Signaller of the worker
    Signaller* signalMaster;    // Signaller of the master
    int quit;                   // Quit/Abort flag
    SquareMatrix** old;         // A pointer to a pointer to the old matrix
    SquareMatrix** new;         // A pointer to a pointer to the new matrix
} ThreadArgs;

int isDiff(double precision, SquareMatrix* old, SquareMatrix* new);
void naiveIterate(SquareMatrix* old, SquareMatrix* new, double prec);
void parIterate(SquareMatrix* old, SquareMatrix* new, double prec, int thrNum);
void* threadWork(void* args);
void partitionBlocks(ThreadArgs *pargs, int thrNum, int dim);

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
    BoundaryException = -8      // Thread allocation outside of boundary
} Error;

void throw(Error e, char** args);

#endif
