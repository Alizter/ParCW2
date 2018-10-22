#ifndef MAIN_H_
#define MAIN_H_

void printArray(int dim, double* array);

void naiveIterate(int dim, double* old, double* new);

int isDiff(double precision, int dim, double* old, double* new);

#endif
