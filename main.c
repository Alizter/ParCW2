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
		* Error handling
		* File read and write
		* Implementations (this is the bit with parallel stuff) 
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
	
	// File name for array
	char* fileName = argv[2];
	
	// Read precision
	const double inputPrecision = strtod(argv[3], NULL);
	
	// If our precision is too small, our program may never stop
	if (inputPrecision <= 0.0)
	{
		throw(PrecisionException, NULL);
	}
	
	// Read array
	double* inputArray = (double*)calloc(dim * dim, sizeof(double));
	readArray(dim, fileName, inputArray);

	
	// Print input array
	printf("Input array:\n");	
	printArray(dim, inputArray);
	
	// Allocate space for new array
	double* new = (double*)malloc(dim * dim * sizeof(double));
	
	// Allocate space for old array (this may be unnecessary)
	double* old = (double*)malloc(dim * dim * sizeof(double));
	
	// copy input to new
	memcpy(new, inputArray, sizeof(double) * dim * dim);
	
	//copy input to old
	memcpy(old, inputArray, sizeof(double) * dim * dim);
	
	printf("\n");
		
	do
	{
		//copy value of new to old
		memcpy(old, new, sizeof(double) * dim * dim);
		
	    //iterate once on old saving to new
		naiveIterate(dim, old, new);
		
		
	}
	// loop if old and new are too different
	while (isDiff(inputPrecision, dim, new, old));
	

	
	// Print new array
	printf("Result:\n");
	printArray(dim, new);
	
	// Exit the program Successfully
	throw(Success, NULL);
}

int isDiff(double precision, int dim, double* old, double* new)
{    
    // Iterate over inner rows
	for (int i = 1; i < dim - 1; i++)
	{
		// Iterate over inner coloumns
		for (int j = 1; j < dim - 1; j++)
		{
			// If we find an entry that is not within precision
			// then it is different
			//double diff = fabs(old[i * dim + j] - new[i * dim + j]);
			
			if (fabs(old[i * dim + j] - new[i * dim + j]) > precision)
			{
				//printf("DIFF (%d,%d): %f\n", i, j, diff);
			    return 1;
            }
		}
	}
	
	// Else all are within spec
	return 0;
}

/*
----- Error handling -----------------------------------------------------------
*/


// Prints error message (or Success) with arguments and exits
void throw(Error e, char** args)
{
	switch (e)
	{
		case Success:
			printf("Program finished successfully.\n");
			break;
		
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
	}
	
	// Exit with error code
	printf("Program exiting.\n");
	exit(e);
}

/*
----- File read and write ------------------------------------------------------
	
	Here we add functionality for reading and writing
	of files. That way a text file with a list of numbers (doubles) 
	seperated by commas can be read, and a number providing the details 
	of the dimension can be given. This will allow the progam to read an
	"array".
*/

// Given a file name and a dimension reads (dim * dim) double values 
// from given file and returns a pointer to them in memory. If the
// file does not have enough values it simply makes them zero.
// Obviously if there are too many values it doesn't read them
void readArray(int dim, char fileName[255], double* array)
{
	FILE* file = fopen(fileName, "r");
	
	char* buffer;
	
	if (file)
	{
		fseek(file, 0, SEEK_END);
		long fileLength = ftell(file);
		fseek(file, 0, SEEK_SET);
		
		buffer = malloc(fileLength);
		
		if (buffer)
		{
			fread(buffer, 1, fileLength, file);
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
	
	// Now that we have a successfull read we can process our buffer
	
	// Number of entries read
	int count = 0;
	
	// pointer to current read location
	char* ptr = buffer;
	
	// while our current read location isn't null and
	// our number of entries is less than expected
	while (ptr && count++ < dim * dim)
	{
		// the "count"th double entry is the next double
		array[count] = strtod(buffer, &ptr);
		// we set the start of the buffer to where the comma should be
		// WARNING if there are spaces in the string this bit will mess up
		buffer = ptr + 1;
	}
}

void printArray(int dim, double* array)
{
	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			printf("%f ", array[i * dim + j]);
		}
		
		printf("\n");
	}
}

/*
---- Implementations -----------------------------------------------------------
*/


// Non-parallel implementation
// Takes in the dimension of the square array,
// the old array, and the new arrays location
// then returns nothing, having modified the new array.
void naiveIterate(int dim, double* old, double* new)
{
	//By iterating over the inner rows,
	//we do not affect the boundary.
	
	// Iterate over inner rows
	for (int i = 1; i < dim - 1; i++)
	{
		// Iterate over inner coloumns
		for (int j = 1; j < dim - 1; j++)
		{
			average(dim, i, j, old, new);
		}
	}
}

// Parallel implementation
void parIterate(int dim, double* old, double* new, int threadNum)
{
	
}

// The corresponding entry of
// the new array is the average
// of it's neighbours in the old
void average(int dim, int i, int j, double* old, double* new)
{
	new[i * dim + j] = 0.25 * (old[(i + 1) * dim + j] + old[(i - 1) * dim + j] + 
		old[i * dim + j + 1] + old[i * dim + j - 1]);
}


