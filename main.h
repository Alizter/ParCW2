#ifndef MAIN_H_
#define MAIN_H_



int isDiff(double precision, int dim, double* old, double* new);

void printArray(int dim, double* array);
void readArray(int dim, char fileName[255], double* array);

void naiveIterate(int dim, double* old, double* new);


// Error codes
typedef enum errors
{
	Success = 0, 			// Program finished sucessfully
	FileException = -1,	 	// Program couldn't read file
	DimParse = -2, 			// Program couldn't parse dimension
	ArgNumExeption = -3,	// Program has wrong number of arguents
	ArrayReadFailure = -4,	// Program couldn't read array
	PrecisionException = -5	// Program precision too small
} Error;

void throw(Error e, char** args);


#endif
