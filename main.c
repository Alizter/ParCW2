#include <stdio.h>
#include <string.h>
#include "main.h"

/*

	Main program

*/

// Dimension of the square array
 int inputDim = 5;

// The initial array
double inputArray[5][5] = 
{
	{1, 2, 3, 4, 5},
	{1, 2, 3, 4, 5},
	{1, 2, 3, 4, 5},
	{1, 2, 3, 4, 5},
	{1, 2, 3, 4, 5}
};


// Number of threads to use
int threadNum = 4;

// Precision to achieve
double precision = 1E-3;


int main()
{	
	// Print input array
	printf("Input array:\n");	
	printArray(inputDim, inputArray);
	
	// Allocate space for new array
	// by copying old one
	double newArray[inputDim][inputDim];
	
	memcpy(newArray, inputArray, sizeof(double) * inputDim * inputDim);
	
	// Iterate and store values in new array.
	naiveIterate(inputDim, inputArray, newArray);
	
	// Print new array
	printf("New array:\n");
	printArray(inputDim, newArray);
	
	return 0;
}

/*

	Implementations

*/


// Non-parallel implementation
// Takes in the dimension of the square array,
// the old array, and the new arrays location
// then returns nothing, having modified the new array.
void naiveIterate(int dim, double** old, double** new)
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
			new[i][j] =	0.25 * 
				(old[i+1][j] + old[i-1][j] + 
				old[i][j+1] + old[i][j-1]);
		}
	}
}

void printArray(int dim, double** array)
{
	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			printf("%f ", array[i][j]);
		}
		
		printf("\n");
	}
}
