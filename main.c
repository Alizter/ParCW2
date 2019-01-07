#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include "error.h"

#define send_row_count  100
#define tag_precision   400
#define tag_dim         500
#define tag_flaginprec  600

#define send_tag        200
#define recieve_tag     300

int process_id;       // The rank of this process

void readMatrix(int dim, char* fileName, double* p_matrix);

typedef struct 
{
    int firstRow;  // First row 
    int count;     // Number of elements
} IndexData ;

// Program takes in dim, fileName and precision
int main(int argc, char** argv)
{
    // Initialise MPI
    MPI_Init(&argc, &argv);
    double startTime = MPI_Wtime();
    
    int num_process;      // The number of processes
       
    // Get and set the process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    
    // Get and set the number of process
    MPI_Comm_size(MPI_COMM_WORLD, &num_process);
    
    MPI_Request request;
    MPI_Status status;
    
    // Default values
    int dim = 5;
    double prec = 1E-3;
    char* outFileName = "results";
    char* fileName = "array1";
    
    if (process_id == 0)
    {
        /* Arg parsing */
        // Command line options
        int c;    
        while ((c = getopt(argc, argv, "d:o:p:")) != -1) {
            switch(c) {            
                case 'd': { // dimension argument
                    dim = atoi(optarg);
                    if (dim <= 0) {
                        throwError(DimensionException, NULL);
                    }                
                    break;
                }
                case 'p': { // precision argument
                    prec = strtod(optarg, NULL);
                    // If our precision is too small, our program may never stop
                    if (prec <= 0.0) {
                        throwError(PrecisionException, NULL);
                    }
                    break;
                }
                case 'o': { // Output file specified
                    outFileName = optarg;
                    break;
                }
            }
        }
        
        // Reading non-optional arguments
        // If the total number of arguments isn't the number of optional arguments
        // plus the number of non-optional arguments, then we have a problem.
        if (argc != 1 + optind) {
            throwError(ArgNumException, NULL);
        }   

        // Get file name and increment pointer for next args if needed
        fileName = argv[optind++];

        /** End of arg parsing **/
        
        //send dim and prec to other processes
        for (int i = 1; i < num_process; i++)
        {
             MPI_Isend(&dim, 1, MPI_INT, i, 
                tag_dim, MPI_COMM_WORLD, &request);
                
             MPI_Isend(&prec, 1, MPI_DOUBLE, i, 
                tag_precision, MPI_COMM_WORLD, &request);
        }
    }
    else
    {
        // Recieve dim and prec if we are not main
        MPI_Recv(&dim, 1, MPI_INT, 0, tag_dim, MPI_COMM_WORLD, &status);
        MPI_Recv(&prec, 1, MPI_DOUBLE, 0, tag_precision, MPI_COMM_WORLD, &status);
    }
     
    //printf("Proc %d: Computation begun...\n", process_id);
    

    int inPrecision = 0;  // Flag for when precision is reached
    
    
    // If we are the main process
    if (process_id == 0)
    {       
        // Reads dim*dim double values from filename
        // into the double* matrix.
        double* p_matrix = calloc(sizeof(double), dim * dim);
        readMatrix(dim, fileName, p_matrix);
                
        // How many rows each process should get
        int numOfRows = 2 + (dim - 2) / num_process;
        
        // The extra rows left over
        // noted for allocation
        int extraRows = (dim - 2) % num_process;
         
        double* oldRows = calloc(sizeof(double), dim * dim);
        double* newRows = calloc(sizeof(double), dim * dim);

        // Data storing all the indicies
        IndexData* pIndexData = calloc(sizeof(IndexData), num_process);
        
        int firstTime = 1;   // flag for first time
        
        while (!inPrecision)
        {
           //printf("Proc %d: Running loop on iteration %d\n", 
           //     process_id, iterations);
                        
            // If we are on the first iteration
            if (firstTime)
            {
                // The number of rows to be allocated
                int rowCount;
                int startRow;
                int endRow = numOfRows - 1;
                
                //printf("Proc %d: Allocating chunks\n", process_id);
                
                // Allocating chunks
                for (int i = 1; i < num_process; i++)
                {
                    rowCount = numOfRows;
                
                    // Append extra rows if needed.
                    if (extraRows > 0)
                    {
                        extraRows--;
                        rowCount++;
                    }
                    
                    // Set start and end rows
                    startRow = endRow - 1;
                    endRow = startRow + numOfRows - 1;
                    
                    // Record data of start and end
                    pIndexData[i].firstRow = startRow;
                    pIndexData[i].count    = rowCount * dim;
                    
                    // MPI send data to process
                    // It can be non blocking
                    
                    // Firstly send number of rows
                    MPI_Isend(&rowCount, 1, MPI_INT, i, 
                        send_row_count, MPI_COMM_WORLD, &request);
                    
                    // Send rows
                    MPI_Isend(p_matrix + startRow * dim, rowCount * dim, 
                        MPI_DOUBLE, i, send_tag, MPI_COMM_WORLD, &request);
                }
                
                // Now there could still be some extra rows however for a large
                // matrix size compared to a small number of processors this
                // will likely never happen. So we won't check for it
                
                // Allocate chunks for main array
                memcpy(oldRows, p_matrix, dim * dim * sizeof(double));
                memcpy(newRows, p_matrix, dim * dim * sizeof(double));
                
                firstTime = 0;         
            }
            else // Not the first time passing through
            {
                //MPI_ISend
               //printf("Proc %d: Sending last row to next\n", process_id);
                //Send last row to next process
                MPI_Isend(oldRows + (numOfRows - 2) * dim, dim, 
                    MPI_DOUBLE, 1, send_tag, MPI_COMM_WORLD, &request);
                
                //printf("Proc %d: Recieving last row from next\n", process_id);
                // Recieve last row from next process
                MPI_Recv(oldRows + (numOfRows - 1) * dim, dim,
                    MPI_DOUBLE, 1, send_tag, MPI_COMM_WORLD, &status);
                    
               //printf("Proc %d: Recieved last row from next\n", process_id);
            }
            
            // We are in precision unless otherwise
            inPrecision = 1;
            
            for (int i = 1; i < numOfRows - 1; i++)
            {
                for (int j = 1; j < dim - 1; j++)
                {
                   //printf("Proc %d: Processing row %d, col %d\n", process_id, i, j);
                    newRows[i * dim + j] = 
                        0.25 
                        * (oldRows[(i - 1) * dim + j]
                         + oldRows[(i + 1) * dim + j]
                         + oldRows[i * dim + j - 1]
                         + oldRows[i * dim + j + 1]);
                         
                    //printf("Proc %d: precision: %f\n", process_id, fabs(
                    //        oldRows[i * dim + j] - newRows[i * dim + j]));
                    
                    // If we are in precision and the difference between
                    // old and new is greater than allowed then we are out
                    // of precision
                    if (inPrecision && prec < fabs(
                            oldRows[i * dim + j] - newRows[i * dim + j]))
                    {
                        inPrecision = 0;
                    }
                }
            }
            
            // Wait for processes to recieve
            MPI_Wait(&request, &status);
            
            int all_inPrecision = 1;
            
            if (inPrecision) // If we are in precision check others
            {
                // For all other processes
                for (int i = 1; i < num_process; i++)
                {
                    // Recieve whether they are in precision or not
                    MPI_Recv(&all_inPrecision, 1, MPI_INT, i, 
                        tag_flaginprec, MPI_COMM_WORLD, &status);
                    
                    //printf("Proc %d: Process %d is in precision\n", process_id, i);
                    
                    if (!all_inPrecision)
                    {
                        // If they are not in precision
                        // then overall we are not in precision
                        inPrecision = 0;
                    }
                }
            }
       
            // Tell other processes about precision
            for (int i = 1; i < num_process; i++)
            {
                MPI_Isend(&inPrecision, 1, MPI_INT, i, 
                    tag_flaginprec, MPI_COMM_WORLD, &request);
            }
            
            // Swap old and new
            double* temp = oldRows;
            oldRows = newRows;
            newRows = temp;
            
            MPI_Wait(&request, &status);
        }
        
        // Now that we have finished we need to 
        // collect the rows from the process
        
        // For each process
        for (int i = 1; i < num_process; i++)
        {
            // Recieve into old rows
            MPI_Recv(oldRows + (pIndexData + i)->firstRow * dim, 
                (pIndexData + i)->count, MPI_DOUBLE, i, 
                recieve_tag, MPI_COMM_WORLD, &status);
        }
        
        // We have finished the computation
        
        // calculate timing data
        // we don't care about the other processes times since the main
        // process has to wait for the others.

        double time = MPI_Wtime() - startTime;
        
        
        
        // Writing output to file
        /*FILE* outFile = fopen(outFileName, "ab+");
        
        if (outFile == NULL)
        {
            // Could not read file so throw error
            char* args[1] = { outFileName };
            throwError(FileException, args);            
        }
        
        for (int i = 0; i < dim; i++)
        {
            for (int j = 0; j < dim; j++)
            {   
            	fprintf(outFile, "%f ", oldRows[i * dim + j]);    
            }
            
            fprintf(outFile, "\n");
        }
        
        fclose(outFile);
        */
        
        FILE* timingFile = fopen("timings", "ab+");
        
        if (timingFile == NULL)
        {
            // Could not open file so throw error
            char* args[1] = { "timings" };
            throwError(FileException, args);            
        }
        
        fprintf(timingFile, "Dim: %d, Prec %f, NoProc: %d Time:\n%lf s\n", dim, 
            prec, num_process, time);
        
        fclose(timingFile);
       
        free(oldRows);
        free(newRows);
        free(pIndexData);
        
    }
    else // If we are not the main process
    {
        inPrecision = 0;
        int numOfRows;

        // Recieve rows from main process
        MPI_Recv(&numOfRows, 1, MPI_INT, 
            0, send_row_count, MPI_COMM_WORLD, &status);
        
        int numOfElements = numOfRows * dim;
        
        double* oldRows = calloc(sizeof(double), numOfElements);
        double* newRows = calloc(sizeof(double), numOfElements);
       
        // Recieve elements
        MPI_Recv(oldRows, numOfElements, MPI_DOUBLE, 
            0, send_tag, MPI_COMM_WORLD, &status);
        
        // Copy old into new for switching later
        memcpy(newRows, oldRows, numOfElements * sizeof(double));
        
        int firstTime = 1;
        
        //Now we can process our values
        while (!inPrecision)
        {
            if (!firstTime)
            {
                // Send first row to previous process
                MPI_Isend(oldRows + dim, dim, MPI_DOUBLE, process_id - 1, 
                    send_tag, MPI_COMM_WORLD, &request);
                
                // If we are not the last process
                if (process_id != num_process - 1)
                {
                    // Send last row to next process_id
                    MPI_Isend(oldRows + (numOfRows - 2) * dim, dim, MPI_DOUBLE, 
                        process_id + 1, send_tag, MPI_COMM_WORLD, &request);
                }
                
                // Recieve last row from previous process and 
                // put it before first
                MPI_Recv(oldRows, dim, MPI_DOUBLE, process_id - 1,  
                    send_tag, MPI_COMM_WORLD, &status); 
                    
                // If we are not the last process
                if (process_id != num_process - 1)
                {
                    // Recieve first row from next process and
                    //put it after last row
                    MPI_Recv(oldRows + (numOfRows - 1) * dim, dim, MPI_DOUBLE, 
                        process_id + 1, send_tag, MPI_COMM_WORLD, &status);
                }              
            }
            else
            {
                // It is no longer the first time
                firstTime = 0;
            }
            
            // Suppose we are within precision
            inPrecision = 1;
            
            //printf("Proc %d: Begining computation\n", process_id);

            for (int i = 1; i < numOfRows - 1; i++)
            {
                for (int j = 1; j < dim - 1; j++)
                {
                    newRows[i * dim + j] = 
                        0.25 
                        * (oldRows[(i - 1) * dim + j]
                         + oldRows[(i + 1) * dim + j]
                         + oldRows[i * dim + j - 1]
                         + oldRows[i * dim + j + 1]);
                    
                    // If we are in precision and the difference between
                    // old and new is greater than allowed then we are out
                    // of precision
                    if (inPrecision && prec < fabs(
                            oldRows[i * dim + j] - newRows[i * dim + j]))
                    {
                        inPrecision = 0;
                    }
                }
            }
            
            // Send (blocking) back results of precision to main process
            MPI_Send(&inPrecision, 1, MPI_INT, 0, tag_flaginprec, MPI_COMM_WORLD);
            
            // Wait for signal from main process
            MPI_Recv(&inPrecision, 1, MPI_INT, 0, 
                tag_flaginprec, MPI_COMM_WORLD, &status);
           
            // Swap old and new for next iteration
            double* temp = oldRows;
            oldRows = newRows;
            newRows = temp;
        }
        
        // This process has finished so it sends back its array to main process
        MPI_Send(oldRows, numOfElements, MPI_DOUBLE, 0, 
            recieve_tag, MPI_COMM_WORLD);
        
        // Finally clean up mallocs
        free(oldRows);
        free(newRows);
    }
    
    //printf("Proc %d: Done.\n", process_id);

    // Finalize MPI
    MPI_Finalize();
    
    // Exit the program Successfully
    //printf("Program finished.\n");
    exit(0);
}

void readMatrix(int dim, char* fileName, double* p_matrix)
{
    // Open file
    FILE* file = fopen(fileName, "r");
    
    // buffer and file length
    char* buffer;
    long fileLength;
    
    // if file != null
    if (file)
    {
        // get the file length
        fseek(file, 0, SEEK_END);
        fileLength = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        // allocate memory for buffer
        buffer = malloc((size_t)fileLength);
        
        // if memory allocated
        if (buffer)
        {
            fread(buffer, 1, (size_t)fileLength, file);
        }
        else
        {
            // Could not read array so throw error
            throwError(ArrayReadFailure, NULL);
        }
        
        // close file
        fclose(file);
    }
    else
    {
        // Could not read file so throw error
        char* args[1] = { fileName };
        throwError(FileException, args);
    }
    
    
    // Create a new matrix to populate
    //p_matrix = malloc(sizeof(double)*(dim * dim));
    
    // Starting pointer
    char* pStart = buffer;
    
    // End pointer (for strtod)
    char* pEnd;
    
    // Number of elements read
    int count = 0;
    
    // While the starting pointer is less than the buffer pointer
    // plus the length of the buffer (i.e. when still in the buffer)
    // AND the count is less than expected number of entries
    while (pStart < (buffer + fileLength) && count < (dim * dim))
    {
        // The "count"th entry is the next readable double 
        p_matrix[count] = strtod(pStart, &pEnd);
        // Increment number of elements read
        ++count;
        // We wish to skip commas (or any deliminator really)
        pStart = pEnd + 1;
    }
}


