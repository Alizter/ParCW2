# Parellel computing coursework 2
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

