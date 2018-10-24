#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#include "main.h"

/* File width
--------------------------------------------------------------------------------
*/

/*
	Contents:
		* Main program
		* File read and write
		* Array data structure
		* Implementations (this is the bit with parallel stuff) 
		* Error handling
*/

/*
--------- Main program ---------------------------------------------------------
*/


// Number of threads to use
int threadNum = 4;


// Program takes in dim, fileName and precision
int main(int argc, char** argv)
{
	if (argc != 4)
	{
		// We do not have the right number of arguments
		throw(ArgNumExeption, NULL);
	}
	
	// Inputted dimension
	const int dim = atoi(argv[1]);
	
	if (dim <= 0)
	{
		throw(DimensionException, NULL);
	}
	
	// File name for array
	char* fileName = argv[2];
	
	// Read precision
	const double inputPrecision = strtod(argv[3], NULL);
	
	// If our precision is too small, our program may never stop
	if (inputPrecision <= 0.0)
	{
		throw(PrecisionException, NULL);
	}
	
	// Read input array
	SquareMatrix* inputArray = readMatrix(dim, fileName);
	
	// Print input array
	printf("Input array:\n");	
	printMatrix(inputArray);
	printf("\n");
	
	// New and old matricies
	// New would be calculated from old
	SquareMatrix* old;
	SquareMatrix* new = duplicateMatrix(inputArray);
		
	do
	{
		// Copy value of new to old
		old = duplicateMatrix(new);
		
		//iterate once on old saving to new
		naiveIterate(old, new);
	//	parIterate(dim, old, new, threadNum);
		
	}
	// loop if old and new are too different
	while (isDiff(inputPrecision, new, old));
	
	
	// Print new array
	printf("Result:\n");
	printMatrix(new);
	
	// Exit the program Successfully
	printf("Program finished.");
	return 0;
}

int isDiff(double precision, SquareMatrix* old, SquareMatrix* new)
{	
	// TODO: Add parallel checking

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

// Prints the given matrix
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
---- Implementations -----------------------------------------------------------
*/


// Non-parallel implementation
// Takes in the dimension of the square array,
// the old array, and the new arrays location
// then returns nothing, having modified the new array.
void naiveIterate(SquareMatrix* old, SquareMatrix* new)
{
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
		}
	}
}
/*
// Parallel implementation
// Complete nonsense doesn't work
void parIterate(int dim, double* old, double* new, int threadNum)
{
	// Create a job queue and then run "threadNum" many threads 
	// to process them
	
	// Allocate space for pointers to structs with arguments in them,
	// this will essentially be our jobs queue
	int queueSize =  (dim - 1) * (dim - 1);
		
	AvgArgs args[queueSize];
	
	// Iterate over rows
	for (int i = 1; i < dim - 1; i++)
	{
		// Iterate over inner coloumns
		for (int j = 1; j < dim - 1; j++)
		{			
			// Squash arguments into struct
			(args + i * dim + j)->dim = &dim;
			(args + i * dim + j)->i = &i;
			(args + i * dim + j)->j = &j;
			(args + i * dim + j)->old = old;
			(args + i * dim + j)->new = new;
		}
	}
	
	//Now that we have our jobs we can go and run them
	
	// Create thread ids
	pthread_t threads[queueSize];
	
	// Create pthreads
	for (int i = 0; i < threadNum; i++)
	{
		pthread_create(threads + i, NULL, paverage, args + i);
	}
	
	// Wait for threads
	for (int i = 0; i < threadNum; i++)
	{
		pthread_join(threads[i], NULL);
	}
}

pthread_mutex_t lock;

void* paverage(void* packedArgs)
{
	//Casting void* to (AvgArgs*) and taking value
	AvgArgs args = *(AvgArgs*)packedArgs;
	
	printf("%f\n", args.old[(*args.i + 1) * *args.dim + *args.j]);
	
	 // Lock mutex
	pthread_mutex_lock(&lock);
	
	// Calculate value
	double result = 0.25 * 
		(args.old[(*args.i + 1) * *args.dim + *args.j] + 
		args.old[(*args.i - 1) * *args.dim + *args.j] + 
		args.old[*args.i * *args.dim + *args.j + 1] + 
		args.old[*args.i * *args.dim + *args.j - 1]);
	
   
	
	// Write to memory new value
	//args.new[args.i * args.dim + args.j] = result;
	
	// Unlock mutex
	pthread_mutex_unlock(&lock);
	
	// Exiting safely
	pthread_exit(0);
}*/

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
		
		case DimParse:
			printf("Error: Could not parse dimension given.\n");
			break;
		
		case ArgNumExeption:
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
	}
	
	// Exit with error code
	printf("Program exiting.\n");
	exit(e);
}

