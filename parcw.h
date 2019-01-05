#ifndef PARCW_H_
#define PARCW_H_

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/sysinfo.h>

/*
------- Implementations --------------------------------------------------------
*/


void* threadWork(void* args);
void partitionBlocks(ThreadArgs *pargs, int thrNum, int dim);

#endif
