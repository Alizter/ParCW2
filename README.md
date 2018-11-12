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
 
 In order to maintain the correctness of the program, we have implemented unit tests. These can be run by running `make test` which will go ahead and run the tests after compiling them. 
 
## Testing on Balena

Here are the results of a test done on Balena:

```
Naive iterate:                  4.030000 s
Parallel iterate 1 threads:     7.880000 s      Speedup: 0.511421
Parallel iterate 2 threads:     8.060000 s      Speedup: 0.250000
Parallel iterate 3 threads:     8.440000 s      Speedup: 0.159163
Parallel iterate 4 threads:     8.740000 s      Speedup: 0.115275
Parallel iterate 5 threads:     9.080000 s      Speedup: 0.088767
Parallel iterate 6 threads:     9.520000 s      Speedup: 0.070553
Parallel iterate 7 threads:     9.940000 s      Speedup: 0.057919
Parallel iterate 8 threads:     10.230000 s     Speedup: 0.049242
Parallel iterate 9 threads:     10.880000 s     Speedup: 0.041156
Parallel iterate 10 threads:    11.180000 s     Speedup: 0.036047
Parallel iterate 11 threads:    11.670000 s     Speedup: 0.031394
Parallel iterate 12 threads:    12.020000 s     Speedup: 0.027940
Parallel iterate 13 threads:    12.010000 s     Speedup: 0.025812
Parallel iterate 14 threads:    12.830000 s     Speedup: 0.022436
Parallel iterate 15 threads:    13.050000 s     Speedup: 0.020587
Parallel iterate 16 threads:    12.690000 s     Speedup: 0.019848
Parallel iterate 17 threads:    13.260000 s     Speedup: 0.017878
Parallel iterate 18 threads:    13.990000 s     Speedup: 0.016003
Parallel iterate 19 threads:    12.860000 s     Speedup: 0.016493
Parallel iterate 20 threads:    13.730000 s     Speedup: 0.014676
Parallel iterate 21 threads:    13.900000 s     Speedup: 0.013806
Parallel iterate 22 threads:    13.940000 s     Speedup: 0.013141
Parallel iterate 23 threads:    14.140000 s     Speedup: 0.012392
Parallel iterate 24 threads:    14.270000 s     Speedup: 0.011767
Parallel iterate 25 threads:    14.590000 s     Speedup: 0.011049
Parallel iterate 26 threads:    14.280000 s     Speedup: 0.010854
Parallel iterate 27 threads:    14.920000 s     Speedup: 0.010004
Parallel iterate 28 threads:    15.200000 s     Speedup: 0.009469
Parallel iterate 29 threads:    15.130000 s     Speedup: 0.009185
Parallel iterate 30 threads:    15.400000 s     Speedup: 0.008723
Parallel iterate 31 threads:    16.050000 s     Speedup: 0.008100
Parallel iterate 32 threads:    16.040000 s     Speedup: 0.007851
```

