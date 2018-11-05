#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>


#include "main.h"

/* File width
--------------------------------------------------------------------------------
*/

/*
	Contents:
		* Main program
		* Implementations (this is the bit with parallel stuff) 
		* Signaller data structure
		* Array data structure	
		* File read and write
		* Error handling
*/

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
	int colour = 0;

	// Command line options
	int c;	
	while ((c = getopt(argc, argv, "cd:p:t:")) != -1)
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
			case 'c': // colour enabled
			{
				colour = 1;
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
	
	// Print input array
	printf("Input array:\n");	
	printMatrix(inputArray, colour);
	printf("\n");
	
	// New and old matricies
	// New would be calculated from old
	SquareMatrix* old = duplicateMatrix(inputArray);
	SquareMatrix* new = duplicateMatrix(inputArray);
		
	// Compute new values
	parIterate(old, new, inputPrecision, thrNum);
	
	// Print new array
	printf("Result:\n");
	printMatrix(new, colour);
	
	// Exit the program Successfully
	printf("Program finished.");
	exit(0);
}





/*
---- Implementations -----------------------------------------------------------
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

void* worker(void* vargs)
{   
    // Cast void args to ThreadArgs
    ThreadArgs* args = (ThreadArgs*)vargs;

    // get dimension
    int dim = (*args->old)->dim;
    
    while (!args->quit)
    {
        waitFor(args->signalWorker);
        
        if (args->quit) break; // just in case break after wait
        
        double maxDiff = 0.0;
    
        for (int k = args->start; k < args->end; k++)
        {
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
            else
            {
                // Throw boundary exception?
            }
        }
        
        // Set max diff
        *(args->diff) = maxDiff;
        
        signal(args->signalMaster);
    }
    
    return 0;
}

// Parellel version of iterate
void parIterate(SquareMatrix* old, SquareMatrix* new, 
    double prec, int thrNum)
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
        
        // Assign start and end points
        args[i].start = i * ((dim * dim) / thrNum + 1);
        args[i].end = (dim * dim) / thrNum + 1;
        
    }
    
    int size = dim * dim;
    
    // Fix trailing job (start and end) due to array size mismatch
    args[thrNum - 1].start = (thrNum - 1) * (size / thrNum + 1);
    args[thrNum - 1].end = size / thrNum + size % thrNum - thrNum + 1;
    
    // Create and run threads
    for (int i = 0; i < thrNum; i++)
    {
        pthread_create(threads + i, NULL, worker, (void*)(args + i));
    }
    
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

/* Signaller */

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

/* *** */


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
	memcpy(
		new->array, 
		old->array, 
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

/*
----- File read and write ------------------------------------------------------
	
	Here we add functionality for reading and writing of files. That way a text
	file with a list of numbers (doubles) seperated by commas  (or any other 
	deliminator) can be read, and a number providing the details of the
	dimension can be given. This will allow the progam to read a matrix.
*/

// Given a file name and a dimension reads (dim * dim) double values from given
// file and returns a pointer to a matrix with said values. If the file does
// not have enough values it simply makes them zero.
// Obviously if there are too many values it doesn't read them
SquareMatrix* readMatrix(int dim, char* fileName)
{
	
	FILE* file = fopen(fileName, "r");
	
	char* buffer;
	long fileLength;
	
	if (file)
	{
		fseek(file, 0, SEEK_END);
		fileLength = ftell(file);
		fseek(file, 0, SEEK_SET);
		
		buffer = malloc((size_t)fileLength);
		
		if (buffer)
		{
			fread(buffer, 1, (size_t)fileLength, file);
		}
		else
		{
			// Could not read array so throw error
			throw(ArrayReadFailure, NULL);
		}
		
		fclose(file);
	}
	else
	{
		// Could not read file so throw error
		char* args[1] = { fileName };
		throw(FileException, args);
	}
	
	
	// Create a new matrix to populate
	SquareMatrix* matrix = newMatrix(dim);
	
	// Starting pointer
	char* pStart = buffer;
	
	// End pointer (for strtod)
	char* pEnd;
	
	// Number of elements read
	int count = 0;
	
	// While the starting pointer is less than the buffer pointer
	// plus the length of the buffer (i.e. when still in the buffer)
	// AND the count is less than expected number of entries
	while (pStart < (buffer + fileLength) && count < (dim * dim))
	{
		// The "count"th entry is the next readable double 
		matrix->array[count] = strtod(pStart, &pEnd);
		// Increment number of elements read
		++count;
		// We wish to skip commas (or any deliminator really)
		pStart = pEnd + 1;
	}
	
	return matrix;
}

// Small sruct for passing around rgb values
typedef struct
{
	int r,g,b;
} RGB;

// Takes a float and gives rgb values for rainbow colouring (basically
// an inferior hsv -> rgb converter)
RGB rgbRainbow(double x)
{
	//Small scalling factor for colouring
	x *= 0.1;

    RGB rgb;

    int section = (int)(x * 6);
    double frac = (x * 6) - (double)((long)(x * 6));

	switch (section % 6)
	{
		case 0:
			rgb.r = 255;
			rgb.g = (int)(255 * frac);
			rgb.b = 0;
			break;
		case 1:
			rgb.r = (int)(255 * (1.0 - frac));
			rgb.g = 255;
			rgb.b = 0;
			break;
		case 2:
			rgb.r = 0;
			rgb.g = 255;
			rgb.b = (int)(255 * frac);
			break;
		case 3:
			rgb.r = 0;
			rgb.g = (int)(255 * (1.0 - frac));
			rgb.b = 255;
			break;
		case 4:
			rgb.r = (int)(255 * frac);

			rgb.g = 0;
			rgb.b = 255;
			break;
		case 5:
			rgb.r = 255;
			rgb.g = 0;
			rgb.b = (int)(255 * (1.0 - frac));
			break;
	}
	
	return rgb;
}

// Prints the given matrix
void printMatrix(SquareMatrix* matrix, int colour)
{
	for (int i = 0; i < matrix->dim; i++)
	{
		for (int j = 0; j < matrix->dim; j++)
		{	
			if (colour)
			{
				// We colour them based on their value
				// Really only works on doubles between 0.0 and 1.0
				RGB rgb = rgbRainbow(matrix->array[i * (matrix->dim) + j]);
				
				printf(
					"\033[38;2;%d;%d;%dm%f\x1b[0m ", 
					rgb.r, rgb.g, rgb.b, 
					matrix->array[i * (matrix->dim) + j]);
			}
			else // We have decided a boring and monochrome output
			{
				printf("%f ", matrix->array[i * (matrix->dim) + j]);
			}
		}
		
		printf("\n");
	}
}


/*
----- Error handling -----------------------------------------------------------
*/


// Prints error message (or Success) with arguments and exits
void throw(Error e, char** args)
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
			printf("Error: Given precision is too small.\n");
			break;
			
		case DimensionException:
			printf("Error: Given dimension is too small.\n");
			break;
			
		case ThreadNumException:
			printf("Error: Given thread number is too small.\n");
			break;
	}
	
	// Exit with error code
	printf("Program exiting.\n");
	exit(e);
}

