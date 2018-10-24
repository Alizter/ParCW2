#ifndef MAIN_H_
#define MAIN_H_



// Main program


// Array data structure

typedef struct
{
	double* array;
	int dim;
} SquareMatrix;

SquareMatrix* duplicateMatrix(SquareMatrix* old);
void freeMatrix(SquareMatrix* matrix);
SquareMatrix* newMatrix(int dim);


// File read and write

SquareMatrix* readMatrix(int dim, char* fileName);
void printMatrix(SquareMatrix* matrix);




// Implementations

int isDiff(double precision, SquareMatrix* old, SquareMatrix* new);
void naiveIterate(SquareMatrix* old, SquareMatrix* new);

//void parIterate(int dim, double* old, double* new, int threadNum);
//void* paverage(void* args);




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
	FileException = -1,	 	// Program couldn't read file
	DimParse = -2, 			// Program couldn't parse dimension
	ArgNumExeption = -3,	// Program has wrong number of arguents
	ArrayReadFailure = -4,	// Program couldn't read array
	PrecisionException = -5,// Program precision too small
	DimensionException = -6,// Program dimension too small
	ThreadNumException = -7 // Program threadNum too small
} Error;

void throw(Error e, char** args);



#endif
