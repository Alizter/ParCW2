#include <stdio.h>
#include <string.h>
#include <math.h>
#include "main.h"

/*

	Main program

*/

// Dimension of the square array
 int inputDim = 5;

// The initial array
double inputArray[25] = 
    {-30, -38, -96, -39, 39, 13, 46, 1, -57, 74, -4, -18,
     -25, -36, 52, -21, -18, 64, -40, 35, -62, 2, -65, -97, 62};




// Number of threads to use
int threadNum = 4;

// Precision to achieve
double inputPrecision = 1E-3;


int main()
{	

	// Print input array
	printf("Input array:\n");	
	printArray(inputDim, inputArray);
	
	// Allocate space for new array
	double* new = (double*)malloc(inputDim * inputDim * sizeof(double));
	
	// Allocate space for old array (this may be unnecessary)
	double* old = (double*)malloc(inputDim * inputDim * sizeof(double));
	
	// copy input to new
	memcpy(new, inputArray, sizeof(double) * inputDim * inputDim);
	
	//copy input to old
	memcpy(old, inputArray, sizeof(double) * inputDim * inputDim);
	
	printf("\n");
	
	do
	{
	    //iterate once on old saving to new
		naiveIterate(inputDim, old, new);
		
		//copy value of new to old
		memcpy(old, new, sizeof(double) * inputDim * inputDim);
	}
	// loop if old and new are too different
	while (isDiff(inputPrecision, inputDim, new, old));
	

	
	// Print new array
	printf("Result:\n");
	printArray(inputDim, new);
	
	return 0;
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
			if (fabs(old[i * dim + j] - new[i * dim + j]) > precision)
			{
			    return 1;
            }
		}
	}
	
	// Else all are within spec
	return 0;
}


/*

	Implementations

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
			// The corresponding entry of
			// the new array is the average
			// of it's neighbours in the old
			new[i * dim + j] =	0.25 * (
			    old[(i + 1) * dim + j] + 
				old[(i - 1) * dim + j] + 
				old[i * dim + j + 1] + 
				old[i * dim + j - 1]);
		}
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
