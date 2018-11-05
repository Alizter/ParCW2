#ifndef MAIN_H_
#define MAIN_H_


// Array data structure

typedef struct
{
    double* array;
    int dim;
} SquareMatrix;

SquareMatrix* duplicateMatrix(SquareMatrix* old);
void freeMatrix(SquareMatrix* matrix);
SquareMatrix* newMatrix(int dim);
int eqSquareMatrix(SquareMatrix* a, SquareMatrix* b);

// Signaller

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


// File read and write

SquareMatrix* readMatrix(int dim, char* fileName);
void printMatrix(SquareMatrix* matrix, int colour);




// Implementations

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

void parIterate(SquareMatrix* old, SquareMatrix* new, 
    double prec, int threadNum);

void* threadWork(void* args);
void partitionBlocks(ThreadArgs *pargs, int thrNum, int dim);



// In order to pass this function to a pthread we need it to have signature 
// void* foo(void*); This will be done by taking the arguments of average
// and squishing them into a struct.

typedef struct
{
    int* dim;
    int* i;
    int* j;
    double* old;
    double* new;
} AvgArgs;

// Error handling

typedef enum
{
    FileException = -1,         // Program couldn't read file
    ArgNumException = -3,       // Program has wrong number of arguents
    ArrayReadFailure = -4,      // Program couldn't read array
    PrecisionException = -5,    // Program precision too small
    DimensionException = -6,    // Program dimension too small
    ThreadNumException = -7     // Program threadNum too small
} Error;

void throw(Error e, char** args);



#endif
