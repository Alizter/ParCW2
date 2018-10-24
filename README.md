# Parellel computing coursework
by Ali Caglayan

See pdf attached. 


## How to use

 1. Compile using `make`.
 2. Run `./main dim file prec thrNum`
 
 Where `dim` is the dimension (>0) of the array (i.e. it is a square array with 
 `dim * dim` entries). `file` is the filename of the file with the entries in.
 
 The program will read the first `dim * dim` entries of this file and take the
 rest as zero.
 
 `prec` is a double value giving the precision (> 0) you want to calculation
 to stop at.
 
 `thrNum` is the number (>0) of threads (i.e. cores) to use
 
 For example the file `array1` has 25 entries, so it can be ran as
 
 ```
	./main 5 array1 1E-3 2
 ```
