#ifndef MAIN_H_
#define MAIN_H_



int isDiff(double precision, int dim, double* old, double* new);

void printArray(int dim, double* array);
void readArray(int dim, char fileName[255], double* array);

void naiveIterate(int dim, double* old, double* new);
void parIterate(int dim, double* old, double* new, int threadNum);

void* paverage(void* args);

// Error codes
typedef enum errors
{
	Success = 0, 			// Program finished sucessfully
	FileException = -1,	 	// Program couldn't read file
	DimParse = -2, 			// Program couldn't parse dimension
	ArgNumExeption = -3,	// Program has wrong number of arguents
	ArrayReadFailure = -4,	// Program couldn't read array
	PrecisionException = -5,// Program precision too small
	DimensionException = -6 // Program dimension too small
} Error;

void throw(Error e, char** args);


// In order to pass this function to a pthread we need it to have signature 
// void* foo(void*); This will be done by taking the arguments of average
// and squishing them into a struct.

typedef struct AvgArgs
{
    int* dim;
    int* i;
    int* j;
    double* old;
    double* new;
} AvgArgs;

#endif
