#!/bin/bash

# set the account to be used for the job
#SBATCH --account=cm30225
#SBATCH --partition=teaching


# set name of job
#SBATCH --job-name=ac2288_test_run_of_parcw
#SBATCH --output=myjob.out
#SBATCH --error=myjob.err

# set the number of nodes
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16

# set max wallclock time
#SBATCH --time=00:30:00

# mail alert at start, end and abortion of execution
#SBATCH --mail-type=END

# send mail to this address
#SBATCH --mail-user=ac2288@bath.ac.uk

# run the application
#for i in `seq 1 16`;
#    do
#        ./main -T -t $i array4 -d 1000 -p 1E-10
#    done    


./main -T -t 8 array4 -d 1000 -p 1E-10
