# Parellel computing coursework
by Ali Caglayan

See pdf attached. 


## Usage

```
./main [options] fileName
    options:
        -c            Colours output of arrays
        -d dim        dimension of square array (i.e. for 25 entries, 5x5)    
        -p prec       Will run computation with precision given
        -t num        Will run main computation with "num" many threads.
        -T            Activate timing mode
```
 The program will read the first `dim * dim` entries of this file and take the
 rest as zero.
 
 `prec` is a double value giving the precision (> 0) you want to calculation
 to stop at.

 `num` is the number (>0) of threads (i.e. cores) to use
 
 In timing mode the program will show sequential and parallel implementation
 timings for thread numbers upto that specified.
 
 For example
 
 ```
    $ ./main array2 -T -t 16 -p 1E-6
    Naive iterate:                  0.000040 seconds
    Parallel iterate 1 threads:     0.009880 seconds
    Parallel iterate 2 threads:     0.009270 seconds
    Parallel iterate 3 threads:     0.008920 seconds
    Parallel iterate 4 threads:     0.008920 seconds
    Parallel iterate 5 threads:     0.008200 seconds
    Parallel iterate 6 threads:     0.008760 seconds
    Parallel iterate 7 threads:     0.009780 seconds
    Parallel iterate 8 threads:     0.008780 seconds
    Parallel iterate 9 threads:     0.008740 seconds
    Parallel iterate 10 threads:    0.008510 seconds
    Parallel iterate 11 threads:    0.008510 seconds
    Parallel iterate 12 threads:    0.010450 seconds
    Parallel iterate 13 threads:    0.011230 seconds
    Parallel iterate 14 threads:    0.010090 seconds
    Parallel iterate 15 threads:    0.011050 seconds
    Parallel iterate 16 threads:    0.010850 seconds
 ```
 
 By default:
     * output will not be coloured.
    * dim will be 5
    * prec will be 1E-3
    * num will be 4
    * timing mode will be off

## How to make and run

 1. Compile using `make`.
 2. Run `./main fileName
  
 For example the file `array1` has 25 entries, so on two threads 
 it can be ran as:
 
 ```
    ./main -d 5 -p 1E-3 -t 2 array1
 ```
 
## Unit testing
 
 To 

## Testing on Brachiosaurus
```
ali@brachiosaurus:~/parcw$ make
gcc -g -pthread -Wall -Wconversion -Wextra -lm -l pthread -std=gnu99   -c -o main.o main.c
main.c: In function ‘main’:
main.c:153:27: warning: conversion from ‘__time_t’ {aka ‘long int’} to ‘double’ may change value [-Wconversion]
             double time = (finish.tv_sec - start.tv_sec);
                           ^
main.c:154:54: warning: conversion from ‘__syscall_slong_t’ {aka ‘long int’} to ‘double’ may change value [-Wconversion]
             time += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
                                                      ^
main.c:106:34: warning: unused variable ‘end’ [-Wunused-variable]
         clock_t startTime, diff, end;
                                  ^~~
gcc -o main main.o parcw.o -g -pthread -Wall -Wconversion -Wextra -lm -l pthread -std=gnu99 
ali@brachiosaurus:~/parcw$ ./main -T -t 16 -p 1E-10 -d 100 array3
ST: 5699	D: 6701539	C: 6707238
Naive iterate:			6.701539 s
Parallel iterate 1 threads:	15.712820 s	Speedup: 0.426501
Parallel iterate 2 threads:	9.095290 s	Speedup: 0.736814
Parallel iterate 3 threads:	6.867489 s	Speedup: 0.975835
Parallel iterate 4 threads:	5.092073 s	Speedup: 1.316073
Parallel iterate 5 threads:	3.940216 s	Speedup: 1.700805
Parallel iterate 6 threads:	3.008588 s	Speedup: 2.227470
Parallel iterate 7 threads:	2.748026 s	Speedup: 2.438674
Parallel iterate 8 threads:	2.668869 s	Speedup: 2.511004
Parallel iterate 9 threads:	3.182685 s	Speedup: 2.105624
Parallel iterate 10 threads:	3.097379 s	Speedup: 2.163616
Parallel iterate 11 threads:	2.932531 s	Speedup: 2.285241
Parallel iterate 12 threads:	2.878939 s	Speedup: 2.327781
Parallel iterate 13 threads:	2.995573 s	Speedup: 2.237148
Parallel iterate 14 threads:	3.216697 s	Speedup: 2.083360
Parallel iterate 15 threads:	3.425592 s	Speedup: 1.956316
Parallel iterate 16 threads:	3.901238 s	Speedup: 1.717798
Program finished.
ali@brachiosaurus:~/parcw$ ./main -T -t 16 -p 1E-10 -d 120 array3
ST: 5393	D: 14386488	C: 14391881
Naive iterate:			14.386488 s
Parallel iterate 1 threads:	30.182220 s	Speedup: 0.476654
Parallel iterate 2 threads:	16.710139 s	Speedup: 0.860944
Parallel iterate 3 threads:	10.706214 s	Speedup: 1.343751
Parallel iterate 4 threads:	7.421653 s	Speedup: 1.938448
Parallel iterate 5 threads:	6.957637 s	Speedup: 2.067726
Parallel iterate 6 threads:	6.034310 s	Speedup: 2.384115
Parallel iterate 7 threads:	5.782051 s	Speedup: 2.488129
Parallel iterate 8 threads:	5.215702 s	Speedup: 2.758304
Parallel iterate 9 threads:	6.263882 s	Speedup: 2.296737
Parallel iterate 10 threads:	6.131760 s	Speedup: 2.346225
Parallel iterate 11 threads:	5.621838 s	Speedup: 2.559037
Parallel iterate 12 threads:	5.587811 s	Speedup: 2.574620
Parallel iterate 13 threads:	5.648233 s	Speedup: 2.547078
Parallel iterate 14 threads:	5.977018 s	Speedup: 2.406968
Parallel iterate 15 threads:	6.520544 s	Speedup: 2.206332
Parallel iterate 16 threads:	7.341499 s	Speedup: 1.959612
Program finished.
```
## Testing on Balena
`./main -T -t 32 array3 -d 100 -p 1E-9`
```
[ac2288@balena-02 parcw]$ cat myjob.out
ST: 0   D: 4040000      C: 4040000
Naive iterate:                  4.040000 s
Parallel iterate 1 threads:     7.903556 s      Speedup: 0.511162
Parallel iterate 2 threads:     4.072275 s      Speedup: 0.992074
Parallel iterate 3 threads:     2.915246 s      Speedup: 1.385818
Parallel iterate 4 threads:     2.375720 s      Speedup: 1.700537
Parallel iterate 5 threads:     2.028088 s      Speedup: 1.992024
Parallel iterate 6 threads:     1.621585 s      Speedup: 2.491389
Parallel iterate 7 threads:     1.771690 s      Speedup: 2.280308
Parallel iterate 8 threads:     1.544127 s      Speedup: 2.616365
Parallel iterate 9 threads:     1.693876 s      Speedup: 2.385062
Parallel iterate 10 threads:    1.593370 s      Speedup: 2.535506
Parallel iterate 11 threads:    1.581546 s      Speedup: 2.554463
Parallel iterate 12 threads:    1.485761 s      Speedup: 2.719145
Parallel iterate 13 threads:    1.515069 s      Speedup: 2.666545
Parallel iterate 14 threads:    2.153764 s      Speedup: 1.875785
Parallel iterate 15 threads:    1.925700 s      Speedup: 2.097939
Parallel iterate 16 threads:    1.793719 s      Speedup: 2.252304
Parallel iterate 17 threads:    2.167608 s      Speedup: 1.863806
Parallel iterate 18 threads:    2.266392 s      Speedup: 1.782569
Parallel iterate 19 threads:    2.232860 s      Speedup: 1.809339
Parallel iterate 20 threads:    2.238318 s      Speedup: 1.804927
Parallel iterate 21 threads:    2.295505 s      Speedup: 1.759961
Parallel iterate 22 threads:    2.306734 s      Speedup: 1.751394
Parallel iterate 23 threads:    2.356231 s      Speedup: 1.714603
Parallel iterate 24 threads:    2.506357 s      Speedup: 1.611901
Parallel iterate 25 threads:    2.537136 s      Speedup: 1.592347
Parallel iterate 26 threads:    2.598869 s      Speedup: 1.554522
Parallel iterate 27 threads:    2.605649 s      Speedup: 1.550477
Parallel iterate 28 threads:    2.687727 s      Speedup: 1.503129
Parallel iterate 29 threads:    2.907241 s      Speedup: 1.389634
Parallel iterate 30 threads:    2.485910 s      Speedup: 1.625160
Parallel iterate 31 threads:    2.788240 s      Speedup: 1.448943
Parallel iterate 32 threads:    2.605861 s      Speedup: 1.550351
Program finished.

########################################################################
----------------------! Balena Job Details !----------------------------

JobID          : 1715598
No. of nodes   : 1
No. of CPUs    : 16
Timelimit      : 00:10:00
Elapsed time   : 00:01:25
Nodelist       : node-sw-160
Energy Used    : 11028.75 Joules

########################################################################

```
