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
    
    // set myrank as the rank that MPI is using
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    
    // set nproc as the number of processor that MPI is using
    MPI_Comm_size(MPI_COMM_WORLD, &nproc); 

    int n[nproc];
    
    

    if (myrank == 0) // if the main process
    {
        for (int i = 0; i < nproc; i++)
        {
            n[i] = i*i;
        }
    
        // Send 
        for (int i = 0; i < nproc; i++)
        {
            MPI_Send(n + i, 1, MPI_INT, i, 99, MPI_COMM_WORLD);
        
            printf("main sending message to %d", i);
        }
        
        
        //MPI_Send(n, 5, MPI_INT, 1, 99, MPI_COMM_WORLD);
    
    }
    
    // for the communicator world put a barrier
    // this ensures that main reports is printed first
    MPI_Barrier(MPI_COMM_WORLD);

    //printf("waiting for main to finish...");
    
    MPI_Status* stat;
    MPI_Recv(n + myrank, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, stat);
    
    printf("mpi process %d recieved from main with error %d", myrank, stat->MPI_ERROR);
    
    n[myrank] *= 4;
    n[myrank] += 3;
    
    MPI_Send(n + myrank, 1, MPI_INT, 0, 99, MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (myrank == 0)
    {
        for (int i = 0; i < nproc; i++)
        {
            printf("main recieved %d from mpi process %d", n[i], i);
        }
    }
    
    // name length allocated
    //int namelen = MPI_MAX_PROCESSOR_NAME;
    //MPI_Get_processor_name(name, &namelen);
    
    // print rank and name of process
    //printf("hello world %d from ’%s’\n", myrank, name);
    
    // Finalize
    MPI_Finalize();
    
    return 0;
}
