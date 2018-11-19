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

The program will read the first `dim * dim` entries of the file. See files named
array* for more examples.

## How to make and run

 1. Compile using `make`.
 2. Run `./main fileName
  
 For example the file `array1` has 25 entries, so on two threads 
 it can be ran as:
 
 ```
    ./main -d 5 -p 1E-3 -t 2 array1
 ```

The result will be outputted to a file called results.

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
 

 
## Unit testing
In order to maintain the correctness of the program, we have implemented unit 
tests. These can be run by running `make test` which will go ahead and run the 
tests after compiling them.

```
~/parcw$ make test
gcc -o test test.o parcw.o -g -pthread -Wall -Wconversion -Wextra -lm -l pthread -lrt -std=gnu99
./test
ALL TESTS PASSED
Tests run: 8
```

## Performance

Using the timing function we analysed the speedup given by our parallel 
implementation. Due to the long job queue and large nature of our timings we
didn't conduct this test on Balena but on another machine called Brachiosaurus.

Here is a specification of Brachiosaurus:

```
Processor         |  Intel(R) Xeon(R) CPU E5620 @ 2.40GHz
Memory            |  24670MB
Machine Type      |  Desktop
Operating System  |  Ubuntu 18.10

Kernel	          |  Linux 4.18.0-10-generic (x86_64)
Version	          |  #11-Ubuntu SMP Thu Oct 11 15:13:55 UTC 2018
Distribution	  |  Ubuntu 18.10
Computer Name	  |  brachiosaurus
```
Brachiosaurus has 16 cores. 

Now running `./main -T 5 -d 300 -p 1E-10 -t 64 array4` on Brachiosaurus gave me
the following results which can be accessed via the csv file:

    'Brachiosaurus Raw timings.csv'

To view a more pleasent presentation, there is an associated pdf with the same 
name before the extension.

**Note: All pdfs and csvs are in parcw/appendix/**

### Speedup

Firstly we investigate the speedup associated to our data. Speedup is 
calculated as the sequential time divided by the parallel time. Our 'naive 
iterate' algorithm is our reference for sequential and is the first coloumn, 
in our raw timing data.

Calculating all the speedups gives us the following data which can be found 
under the name 'Brachiosaurus Speedup.csv' (or .pdf).

Infact looking at the pdf, some obvious artifacts emerge. Take for example at
dim 300 (bottom row). When our program is running with 8 threads it has a peak 
speedup. This makes sense because many of the cores are busy (i.e. they have a 
high efficiency which we will discuss later). The main thread which is 
essentially doing all the overhead has no trouble switching cores. But as soon 
as we go to 9 threads there is a big workload being swapped around so the 
waiting quickly slows down to program. Note the drop in speedup from 3.64 to 
2.53.

Fortunately, as the number of threads increases, the extra division of the 
workload is quickly made up for and we get an even higher peak at 15 threads.
This is what one would expect due to the number of cores being 16, the last 
core running the main thread. I would argue this is the perfect balance 
between efficiency and speedup.

Now that's not to say it doesn't get any better. Infact there are more peaks 
in speedup. Notably at 24 threads and 31 threads. Initially this was confusing,
however my current hypothesis is that this is due to hyperthreading on 
Brachiosaurus.

Here are the peaks at dim 300

```
Threads     Speedup

8           3.64
15          3.88
24          3.81
31          3.87
```

So it would seem the optimum configuration with hyperthreading would be the 
number of cores times 2 minus 1. Without hyperthreading it seems to be number 
of cores minus 1.

### Efficiency

We calculated the efficiency from the results. Simply as the speedup divided 
by the number of threads. We get the following data which can be found in 
'Brachiosaurus Efficiency.csv' (or .pdf).

Efficiency seems to come in homogenous chunks of size 8. The most efficient is 
1-8 threads, followed by 9-16, and so on. This makes sense due to lower thread 
counts spending less time switching. We note that it is usually expected 50% 
efficiency for a parallel program and this seems to be the case.

## Balena vs Brachiosaurus

For reference here is a comparision of Balena and Brachiosaurus. Note due to 
constraints on the Balena queue, spending time testing on Brachiosaurus was 
the more sensible option.

Note that these are old results, and the syntax of the program may have changed 
slightly. But that is not important, what is important is the comparison of the 
timings.

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

