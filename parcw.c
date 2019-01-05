#include "parcw.h"

/*
---- Implementations -----------------------------------------------------------
*/

// Work for thread
void* worker(void* vargs)
{   
    // Cast void args to ThreadArgs
    ThreadArgs* args = (ThreadArgs*)vargs;

    // get dimension
    int dim = (*args->old)->dim;
    
    // while not flagged to quit
    while (!args->quit)
    {
        // wait for signal from master
        waitFor(args->signalWorker);
        
        // just in case we are flagged, break
        if (args->quit) break;
              
        // record max difference in our batch
        double maxDiff = 0.0;
    
        // going through each cell in our batch
        for (int k = args->start; k < args->end; k++)
        {
            // converting to (i,j) coordinates
            int i = k / dim;
            int j = k % dim;
        
            // First make sure we are within the boundary
            if (i > 0 && j > 0 && i < dim - 1 && j < dim - 1)
            {
                // Compute average of neighbours
                (*args->new)->array[i * dim + j] = 0.25 * (
                    (*args->old)->array[(i - 1) * dim + j] +
                    (*args->old)->array[(i + 1) * dim + j] +
                    (*args->old)->array[i * dim + j + 1] +
                    (*args->old)->array[i * dim + j - 1]);

                // Compute max diff
                maxDiff = fmax(maxDiff, fabs(
                    (*args->new)->array[k] - (*args->old)->array[k]));
            }
        }
        
        // Set max diff
        *(args->diff) = maxDiff;
        
        // tell master that we are done
        signal(args->signalMaster);
    }
    
    return 0;
}

// Gives start and end points of blocks for each thread
// deciding how it will get partitioned
void partitionBlocks(ThreadArgs* args, int thrNum, int dim)
{
	// total number of cells
    int size = dim * dim;
    
    //we allocate chunks like this
    int chunkSize = (size / thrNum) + (size % thrNum) / thrNum;
    
    // equally distribute all but the last thread
    for (int i = 0; i < thrNum - 1; i++)
    {
        args[i].start = i * chunkSize;
        args[i].end = (i + 1) * chunkSize;
    }

	//give the last thread whats left over
    args[thrNum - 1].start = (thrNum - 1) * chunkSize;
    args[thrNum - 1].end = size;
}

// Parellel version of iterate
void parIterate(SquareMatrix* old, SquareMatrix* new, double prec, int thrNum)
{
    // For readability
    int dim = old->dim;

    // Allocate space for thread args
    ThreadArgs args[thrNum];
    
    // Allocate space for threads
    pthread_t threads[thrNum];
    
    // Master signaller
    Signaller* master = newSignaller(thrNum);
    
    // Allocate space for max differences
    double maxDiff[thrNum];
    
    // Populate args
    for (int i = 0; i < thrNum; i++)
    {
        args[i].id = i;             // Assign thread id
        args[i].diff = maxDiff + i; // Point diff to maxDiff
        args[i].quit = 0;           // Don't quit
        args[i].old = &old;         // old array
        args[i].new = &new;         // new array
        
        // Assign new singaller for worker
        args[i].signalWorker = newSignaller(1);
        
        // Assign master signaller
        args[i].signalMaster = master;
    }
    
    // partition data
    partitionBlocks(args, thrNum, dim);
    
    // number of processor
	size_t nop = (size_t)get_nprocs_conf();
	//printf("CPUs: %ld\n", nop);

    // Create and run threads
    for (int i = 0; i < thrNum; i++)
    {
    	// allocat space for pthread attribute
   	    pthread_attr_t attr;
   	    // Allocate a cpu set
   	    cpu_set_t* p_cpuset = CPU_ALLOC((size_t)thrNum % nop);
   	    // get the cpu set size
   	    size_t cpusetSize = CPU_ALLOC_SIZE((size_t)thrNum % nop);
   	    // allocate cpus
   	    CPU_ZERO_S(cpusetSize, p_cpuset);
   	    // set the number of cpus
   	    CPU_SET_S((size_t)i % nop, cpusetSize, p_cpuset);
   	    // initialise attribute
   	    pthread_attr_init(&attr);
        // set cpu set for attribute
        pthread_attr_setaffinity_np(&attr, cpusetSize, p_cpuset);
    	// create thread
        pthread_create(threads + i, &attr, worker, (void*)(args + i));
    }
    
    // flag for within precision
    int withinPrecision = 0;
    
    while (!withinPrecision)
    {
        // Swap pointers to matricies
        // Essentially swapping old and new. Eliminates need to copy
        SquareMatrix* temp = old;
        old = new;
        new = temp;
    
        // Signal all workers to begin
        for (int i = 0; i < thrNum; i++)
        {
            signal(args[i].signalWorker);
        }
        
        // Wait for all workers to signal (like a barrier)
        waitFor(master);
        
        // Each worker has retururned their max difference
        // We simply need to check they are all within precision
        
        // Pretend we are within precision at first
        withinPrecision = 1;
        
        // Check each max_diff
        for (int i = 0; i < thrNum; i++)
        {
            // if any are larger than prec
            if (*(args[i].diff) > prec)
            {
                // then we are not within precision
                withinPrecision = 0;
                break;
            }
        }
    }
    
    // After finishing we join up and clean up
    
    for (int i = 0; i < thrNum; i++)
    {
        // Set thread to quit
        args[i].quit = 1;
        // Signal worker to stop waiting
        signal(args[i].signalWorker);
        // Join thread
        pthread_join(threads[i], NULL);
        // Destroy associated signaller
        destroySignaller(args[i].signalWorker);
    }
        
    // destroy master signaller
    destroySignaller(master);
}

// Computation of differences for 
int isDiff(double precision, SquareMatrix* old, SquareMatrix* new)
{    
    int dim = new->dim;

    // Iterate over inner rows
    for (int i = 1; i < dim - 1; i++)
    {
        // Iterate over inner coloumns
        for (int j = 1; j < dim - 1; j++)
        {
            // If we find an entry that is not within precision
            // then it is different
            double diff = 
                fabs(old->array[i * dim + j] - new->array[i * dim + j]);
            
            if (diff > -precision && diff < precision)
            {
                return 1;
            }
        }
    }
    
    // Else all are within spec
    return 0;
}

// Non-parallel implementation
// Takes in the dimension of the square array,
// the old array, and the new arrays location
// then returns nothing, having modified the new array.
void naiveIterate(SquareMatrix* pOld, SquareMatrix* pNew, double prec)
{
    double maxDiff;
    int swapFlag = 0;

    // We assign pointers like this so we can swap them
    SquareMatrix* old = pOld;
    SquareMatrix* new = pNew;
    
    do
    {    
        if (swapFlag)
        {
            // Swap pointers
            SquareMatrix* temp = old;
            old = new;
            new = temp;
            swapFlag = 0;
        }
    
        maxDiff = 0.0;

        //By iterating over the inner rows,
        //we do not affect the boundary.
        
        int dim = old->dim;
        
        // Iterate over inner rows
        for (int i = 1; i < dim - 1; i++)
        {
            // Iterate over inner coloumns
            for (int j = 1; j < dim - 1; j++)
            {
                // The corresponding entry of
                // the new array is the average
                // of it's neighbours in the old
                new->array[i * dim + j] = 0.25 * (
                    old->array[(i + 1) * dim + j] + 
                    old->array[(i - 1) * dim + j] + 
                    old->array[i * dim + j + 1] + 
                    old->array[i * dim + j - 1]);
                
                // compute max diff
                maxDiff = fmax(maxDiff, fabs(
                    new->array[i * dim + j] - 
                    old->array[i * dim + j]));
            }
        }
        
        swapFlag = 1;
    }
    // loop if old and new are too different
    while (maxDiff > prec);
}

/*
--------- Signaller ------------------------------------------------------------
*/

// Constructor for the signaller
Signaller* newSignaller(int targetValue)
{
    // Allocate space for signaller
    Signaller* s = malloc(sizeof(Signaller));

    // target value is the value given
    s->targetValue = targetValue;
    
    // initialise current counter value
    s->currentValue = 0;
    
    // allocate and initialise conditional variable
    s->pCond = malloc(sizeof(pthread_cond_t));
    pthread_cond_init(s->pCond, NULL);
    
    // allocate and initialise mutex
    s->pMutex= malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(s->pMutex, NULL);
    
    // finally return pointer of constructed Signaller
    return s;
}

// destructor for signaller
void destroySignaller(Signaller* s)
{
    // destroy condional variable and mutex
    pthread_cond_destroy(s->pCond);
    pthread_mutex_destroy(s->pMutex);

    // free all the pointers
    free(s->pCond);
    free(s->pMutex);
    free(s);
}

// Waits until current value becomes the target value
void waitFor(Signaller* s)
{
    // lock with mutex to avoid race condition
    pthread_mutex_lock(s->pMutex);
    
    while (s->currentValue != s->targetValue)
    {
        // wait for conditional varaible
        pthread_cond_wait(s->pCond, s->pMutex);
    }
    
    // Reset counter for reuse next time
    // note that we don't decrement as in the semaphore case
    s->currentValue = 0;
    
    // unlock mutex
    pthread_mutex_unlock(s->pMutex);
}

// signal a signaller
void signal(Signaller* s)
{
    // lock mutex
    pthread_mutex_lock(s->pMutex);
    
    // increment counter counting uses
    s->currentValue++;
    
    // unlock the mutex
    pthread_mutex_unlock(s->pMutex);
    
    // signal the conditional varaible
    pthread_cond_signal(s->pCond);
}


/*
--------- Array data structure -------------------------------------------------
*/

// Duplicate a square matrix
// Takes a pointer to a square matrix and returns another to a copy
// of the matrix
SquareMatrix* duplicateMatrix(SquareMatrix* old)
{
    // Create new square matrix
    SquareMatrix* new = malloc(sizeof(SquareMatrix));
    
    // Allocate space for array
    new->array = calloc(sizeof(double), (size_t)(old->dim * old->dim));
    
    // Copy array to new array
    memcpy(new->array, old->array, 
        sizeof(double) * (size_t)(old->dim * old->dim));
    
    // Copy dims
    new->dim = old->dim;
    
    return new;
}

// Free SquareMatrix from memory
void freeMatrix(SquareMatrix* matrix)
{
    free(matrix->array);
    free(matrix);
}

// Create a new square matrix of certain dimension
SquareMatrix* newMatrix(int dim)
{
    if (dim <= 0)
    {
        throw(DimensionException, NULL);
    }
    
    SquareMatrix* matrix = malloc(sizeof(SquareMatrix));
    matrix->array = calloc(sizeof(double), (size_t)(dim * dim));
    matrix->dim = dim;        
    
    return matrix;
}

// Resizes matrix usually meant for making matrix smaller than original
// Does not preserve any order simply shifts entries to fit new one
// if resized bigger adds zeros
SquareMatrix* resizeMatrix(SquareMatrix* old, int dim)
{
    if (dim < old->dim)
    {
        SquareMatrix* new = duplicateMatrix(old);
        new->dim = dim;
        return new;
    }
    
    // dim >= old->dim
    // Create new square matrix
    SquareMatrix* new = newMatrix(dim);

    // Allocate space for array
    new->array = calloc(sizeof(double), (size_t)(dim * dim));
    
    // Copy old entries onto new matrix
    memcpy(new->array, old->array, 
        sizeof(double) * (size_t)(old->dim * old->dim));
        
    return new;
}

// Equality function for two square matricies
int eqSquareMatrix(SquareMatrix* a, SquareMatrix* b)
{
    // check dimension
    if (a->dim != b->dim)
    {
        return 0;
    }

    // check each element
    for (int i = 0; i < a->dim * a->dim; i++)
    {
        // compare doubles in each element
        if (a->array[i] != b->array[i])
        {
            return 0;
        }
    }
    
    return 1;
}

// Equality for two matrices within toleance
int eqPrecSquareMatrix(SquareMatrix* a, SquareMatrix* b, double prec)
{
    // check dimensions
    if (a->dim != b->dim)
    {
        return 0;
    }

    // check each element
    for (int i = 0; i < a->dim * a->dim; i++)
    {
        // if there amounts are larger a given precision
        if (fabs(a->array[i] - b->array[i]) > prec)
        {
            // return 0 failing the equality
            return 0;
        }
    }
    
    // otherwise we have suceeded
    return 1;
}

/*
----- File read and write ------------------------------------------------------
    
    Here we add functionality for reading and writing of files. That way a text
    file with a list of numbers (doubles) seperated by commas  (or any other 
    deliminator) can be read, and a number providing the details of the
    dimension can be given. This will allow the progam to read a matrix.
*/


// Prints the matrix
void printMatrix(SquareMatrix* matrix)
{
    for (int i = 0; i < matrix->dim; i++)
    {
        for (int j = 0; j < matrix->dim; j++)
        {   
        	printf("%f ", matrix->array[i * (matrix->dim) + j]);    
        }
        
        printf("\n");
    }
}

// print matrix to file
void fprintMatrix(FILE* file, SquareMatrix* matrix)
{
    for (int i = 0; i < matrix->dim; i++)
    {
        for (int j = 0; j < matrix->dim; j++)
        {   
        	fprintf(file, "%f ", matrix->array[i * (matrix->dim) + j]);    
        }
        
        fprintf(file, "\n");
    }
}



