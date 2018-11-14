# Parellel computing coursework
by Ali Caglayan

See pdf attached. 


## Usage

```
./main [options] fileName
    options:
        -d dim        dimension of square array (default: 5)    
        -p prec       precision of computation (default: 1E-3)
        -t num        run main computation with "num" many threads (default: 4)
        -o fileName   output file name (default: "results")
        -T num        activate timing mode with dim stepsize of num (default: 5)
```

### Timing mode

In "timing mode" the program will run several versions of the computation with
varying parameters and record the elapsed time for each computation in a file
called "timings".

The program will iterate upto and including the threads specified, for each
size of array. For example running `./main -T 5 array3 -d 20 -t 8` has the
following output:

```
$ ./main -T 5 array3 -d 70 -t 8
Begining computations:
Dim: 70 Threads: 8 [########################] 100.000%
Program finished.
```

And the "timings" file contains:

```
5,0.000004,0.000344,0.001467,0.001364,0.002263,0.005386,0.005458,0.002701,0.003082,
10,0.000071,0.000656,0.006590,0.003552,0.004665,0.005084,0.004887,0.007800,0.006083,
15,0.000523,0.003696,0.011480,0.030216,0.013309,0.016665,0.015112,0.017343,0.033272,
20,0.001139,0.003245,0.010499,0.011186,0.013485,0.022314,0.021256,0.023817,0.022369,
25,0.007489,0.044592,0.032982,0.032572,0.066525,0.041665,0.043887,0.048666,0.071515,
30,0.009290,0.057037,0.057547,0.042976,0.065891,0.091048,0.083380,0.068871,0.066387,
35,0.019096,0.050385,0.070463,0.065512,0.103745,0.080462,0.117100,0.130738,0.166892,
40,0.027114,0.119410,0.098411,0.091749,0.103043,0.145344,0.106505,0.150458,0.140986,
45,0.061985,0.230154,0.162608,0.237613,0.203169,0.260442,0.217536,0.232194,0.305000,
50,0.076757,0.107489,0.133702,0.171697,0.241647,0.247685,0.281243,0.304543,0.224425,
55,0.143738,0.283375,0.504043,0.480490,0.763469,0.516239,0.516945,0.571810,0.691818,
60,0.209259,0.288329,0.264705,0.351721,0.392368,0.365421,0.387223,0.397689,0.426030,
65,0.293432,0.451855,0.435839,0.534228,0.572226,0.568549,0.553841,0.621455,0.793791,
70,0.206570,0.320209,0.295400,0.324676,0.404540,0.371016,0.510100,0.478557,0.483595,
```

Now as you can see the first coloumn is the dimension that was computed on.
The second coloumn is the time for the naiveIterate function to compute.
The rest of the coloumns are the results for parallelIterate for thread numbers
1 to 8 (which was specified).

---


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

## Testing on Brachiosaurus
```
ali@brachiosaurus:~/parcw$ ./main -T -t 16 -p 1E-10 -d 100 array3
ST: 5699    D: 6701539    C: 6707238
Naive iterate:            6.701539 s
Parallel iterate 1 threads:    15.712820 s    Speedup: 0.426501
Parallel iterate 2 threads:    9.095290 s    Speedup: 0.736814
Parallel iterate 3 threads:    6.867489 s    Speedup: 0.975835
Parallel iterate 4 threads:    5.092073 s    Speedup: 1.316073
Parallel iterate 5 threads:    3.940216 s    Speedup: 1.700805
Parallel iterate 6 threads:    3.008588 s    Speedup: 2.227470
Parallel iterate 7 threads:    2.748026 s    Speedup: 2.438674
Parallel iterate 8 threads:    2.668869 s    Speedup: 2.511004
Parallel iterate 9 threads:    3.182685 s    Speedup: 2.105624
Parallel iterate 10 threads:    3.097379 s    Speedup: 2.163616
Parallel iterate 11 threads:    2.932531 s    Speedup: 2.285241
Parallel iterate 12 threads:    2.878939 s    Speedup: 2.327781
Parallel iterate 13 threads:    2.995573 s    Speedup: 2.237148
Parallel iterate 14 threads:    3.216697 s    Speedup: 2.083360
Parallel iterate 15 threads:    3.425592 s    Speedup: 1.956316
Parallel iterate 16 threads:    3.901238 s    Speedup: 1.717798
Program finished.
ali@brachiosaurus:~/parcw$ ./main -T -t 16 -p 1E-10 -d 120 array3
ST: 5393    D: 14386488    C: 14391881
Naive iterate:            14.386488 s
Parallel iterate 1 threads:    30.182220 s    Speedup: 0.476654
Parallel iterate 2 threads:    16.710139 s    Speedup: 0.860944
Parallel iterate 3 threads:    10.706214 s    Speedup: 1.343751
Parallel iterate 4 threads:    7.421653 s    Speedup: 1.938448
Parallel iterate 5 threads:    6.957637 s    Speedup: 2.067726
Parallel iterate 6 threads:    6.034310 s    Speedup: 2.384115
Parallel iterate 7 threads:    5.782051 s    Speedup: 2.488129
Parallel iterate 8 threads:    5.215702 s    Speedup: 2.758304
Parallel iterate 9 threads:    6.263882 s    Speedup: 2.296737
Parallel iterate 10 threads:    6.131760 s    Speedup: 2.346225
Parallel iterate 11 threads:    5.621838 s    Speedup: 2.559037
Parallel iterate 12 threads:    5.587811 s    Speedup: 2.574620
Parallel iterate 13 threads:    5.648233 s    Speedup: 2.547078
Parallel iterate 14 threads:    5.977018 s    Speedup: 2.406968
Parallel iterate 15 threads:    6.520544 s    Speedup: 2.206332
Parallel iterate 16 threads:    7.341499 s    Speedup: 1.959612
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

