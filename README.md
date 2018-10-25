# Parellel computing coursework
by Ali Caglayan

See pdf attached. 


## Usage

```
	./main [options] fileName
		options:
			-c			Colours output of arrays
			-d dim		dimension of square array (i.e. for 25 entries, 5x5)	
			-p prec		Will run computation with precision given
			-t num		Will run main computation with "num" many threads.
```
 The program will read the first `dim * dim` entries of this file and take the
 rest as zero.
 
 `prec` is a double value giving the precision (> 0) you want to calculation
 to stop at.

 `num` is the number (>0) of threads (i.e. cores) to use
 
 By default:
 	* output will not be coloured.
	* dim will be 5
	* prec will be 1E-3
	* num will be 4

## How to make and run

 1. Compile using `make`.
 2. Run `./main fileName
  
 For example the file `array1` has 25 entries, so on two threads 
 it can be ran as:
 
 ```
	./main -d 5 -p 1E-3 -t 2 array1
 ```
