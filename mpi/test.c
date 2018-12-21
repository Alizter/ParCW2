#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    // Allocating buffer for processor name
    char name[MPI_MAX_PROCESSOR_NAME];
    
    // Initialising MPI with return code
    int returnCode = MPI_Init(&argc, &argv);
    
    // Check for initialising error and abort
    if (returnCode != MPI_SUCCESS) 
    {
        printf ("Error starting MPI program\n");
        MPI_Abort(MPI_COMM_WORLD, returnCode);
    }

    int myrank; // rank of mpi process
    int nproc; // number of processors
    
    
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank); // allocate space for rank
    MPI_Comm_size(MPI_COMM_WORLD, &nproc); // allocate space for number of processor

    if (myrank == 0) // if the main process
    {
        printf("main reports %d procs\n", nproc);
    }
    
    // for the communicator world put a barrier
    // this ensures that main reports is printed first
    MPI_Barrier(MPI_COMM_WORLD);

    // name length allocated
    int namelen = MPI_MAX_PROCESSOR_NAME;
    MPI_Get_processor_name(name, &namelen);
    
    // print rank and name of process
    printf("hello world %d from ’%s’\n", myrank, name);
    
    // Finalize
    MPI_Finalize();
    
    return 0;
}
