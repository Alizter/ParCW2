#include "parcw2.h"

// MPI version of parIterate

#define send_row_count  0xBEEFBABE
#define send_tag        0xCAFEBABE
#define recieve_tag     0xDEADBEEF


void parIterateMPI(char* fileName, int dim, double prec)
{
    int process_id;       // The rank of this process
    int num_process;      // The number of processes
    
    int iterations = 0;   // number of iterations
    int inPrecision = 0;  // Flag for when precision is reached
    
    double* matrix = malloc(sizeof(double) * dim * dim);

    // Initialise MPI
    // Here all workers will run this code
    //MPI_Init(NULL, NULL);
    
    // Get and set the process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    
    // Get and set the number of process
    MPI_Comm_size(MPI_COMM_WORLD, &num_process);
    
    MPI_Request request;
    
    // TODO: If more processes than cores, what to do?
    if (num_process > dim - 2) 
    {
        num_process = dim - 2;

        // TODO: is this really necessery?
        if (process_id > num_process - 1) 
        {
            // TODO: throw something
        }
    }
    
    // If we are the main process
    if (process_id == 0)
    {
        // Reads dim*dim double values from filename
        // into the double* matrix.
        readMatrix(dim, fileName, matrix);
        
        // The main process needs to allocate chunks for each process
        // sent the whole row to each process if it is the first iteration
        
        // How many rows each process should get
        // Simply the number of interior points divided by processor number
        // We add two for the rows that come from other arrays or are on
        // the boundary
        int numOfRows = 2 + (dim - 2) / num_process;
        // The extra rows left over
        // noted for allocation
        int extraRows = (dim - 2) % num_process;
        
        // Index of end row for main thread
        //int endRow = avg_rows;
        
        double* oldRows = malloc(numOfRows * dim * sizeof(double));
        double* newRows = malloc(numOfRows * dim * sizeof(double));
        
        // Data storing all the indicies
        IndexData indexData[num_process];
        
        while (!inPrecision)
        {
            iterations++;
            
            // If we are on the first iteration
            if (iterations <= 0)
            {
                // The number of rows to be allocated
                int rowCount;
                int startRow;
                int endRow = numOfRows - 1;
                
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
                    indexData[i].firstRow = startRow;
                    indexData[i].lastRow  = endRow;
                    indexData[i].count    = rowCount * dim;
                    
                    
                    // MPI send data to process
                    // It can be non blocking
                    
                    // Firstly send number of rows
                    MPI_Isend(&rowCount, 1, MPI_INT, i, 
                        send_row_count, MPI_COMM_WORLD, &request);
                    
                    // Send rows
                    MPI_Isend(matrix + startRow * dim, rowCount * dim, 
                        MPI_DOUBLE, i, send_tag, MPI_COMM_WORLD, &request);
                }
                
                // Now there could still be some extra rows however for a large
                // matrix size compared to a small number of processors this
                // will likely never happen. So we won't check for it
                
                // Allocate chunks for main array
                // by pointing there
                oldRows = matrix;
                newRows = matrix;
                
            }
            else // Not the first time passing through
            {
                //MPI_ISend
                
                //Send last row to next process
                MPI_Isend(matrix + (numOfRows - 2) * dim, dim, 
                    MPI_DOUBLE, 1, send_tag, MPI_COMM_WORLD, &request);
                
                // Recieve last row from next process
                MPI_Recv(oldRows + (numOfRows - 1) * dim, dim,
                    MPI_DOUBLE, 1, send_tag, MPI_COMM_WORLD, NULL);
            }
            
            // Now we can process our chunk
            
            // We are in precision unless otherwise
            inPrecision = 1;
            
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
            
            // Wait for processes to recieve
            MPI_Wait(&request, NULL);
            
            int all_inPrecision = 1;
            
            if (inPrecision) // If we are in precision check others
            {
                // For all other processes
                for (int i = 1; i < num_process; i++)
                {
                    // Recieve whether they are in precision or not
                    MPI_Recv(&all_inPrecision, 1, MPI_INT, i, 
                        recieve_tag, MPI_COMM_WORLD, NULL);
                    
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
                MPI_Isend(&inPrecision, 1, MPI_INT, i, send_tag, MPI_COMM_WORLD, NULL);
            }
            
            // Swap old and new
            double* temp = oldRows;
            oldRows = newRows;
            newRows = temp;
            
            MPI_Wait(&request, NULL);
        }
        
        // Now that we have finished we need to 
        // collect the rows from the process
        
        int index = 0;
        
        // For each process
        for (int i = 1; i < num_process; i++)
        {
            // Recieve into old rows
            // doesn't matter since it won't bee used any more
            // we use oldRows as it has the newest data ironically
            MPI_Recv(oldRows + index, (indexData + i)->count, MPI_DOUBLE, i, 
                recieve_tag, MPI_COMM_WORLD, NULL);
                
            // the array index
            index += (indexData + i)->firstRow * dim;
        }
        
        // Finished
        
        // Clean up
        free(oldRows);
        free(newRows);
        free(indexData);
        
    }
    else // If we are not the main process
    {
        inPrecision = 1;
        int numOfRows;

        // Recieve rows from main process
        MPI_Recv(
            &numOfRows, 1, MPI_INT, send_row_count, 0, MPI_COMM_WORLD, NULL);
        
        int numOfElements = numOfRows * dim;
        
        double* oldRows = malloc(numOfRows * dim * sizeof(double));
        double* newRows = malloc(numOfRows * dim * sizeof(double));
        
        // Recieve elements
        MPI_Recv(oldRows, numOfElements, MPI_DOUBLE, 
            0, send_tag, MPI_COMM_WORLD, NULL);
        
        int firstTime = 1;
        
        //Now we can process our values
        while (!inPrecision)
        {
            if (!firstTime)
            {
                // Send first row to previous process
                MPI_Isend(oldRows + dim, dim, MPI_DOUBLE, process_id - 1, 
                    send_tag, MPI_COMM_WORLD, NULL);
                
                // If we are not the last process
                if (process_id != num_process - 1)
                {
                    // Send last row to next process_id
                    MPI_Isend(oldRows + (numOfRows - 2) * dim, dim, MPI_DOUBLE, 
                        process_id + 1, send_tag, MPI_COMM_WORLD, NULL);
                }
                
                // Recieve last row from previous process and 
                // put it before first
                MPI_Recv(oldRows, dim, MPI_DOUBLE, process_id - 1,  
                    send_tag, MPI_COMM_WORLD, NULL); 
                    
                // If we are not the last process
                if (process_id != num_process - 1)
                {
                    // Recieve first row from next process and
                    //put it after last row
                    MPI_Recv(oldRows + (numOfRows - 1) * dim, dim, MPI_DOUBLE, 
                        process_id + 1, send_tag, MPI_COMM_WORLD, NULL);
                }
            }
            else
            {
                // It is no longer the first time
                firstTime = 0;
            }
            
            // Suppose we are within precision
            inPrecision = 1;
            
            for (int i = 0; i < numOfRows - 1; i++)
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
            MPI_Send(&inPrecision, 1, MPI_INT, 0, recieve_tag, MPI_COMM_WORLD);
            
            // Wait for signal from main process
            MPI_Recv(&inPrecision, 1, MPI_INT, 0, 
                send_tag, MPI_COMM_WORLD, NULL);
            
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
    
    
    //Quit
    //return 0; // TODO: could possibly return old values??
}


void readMatrix(int dim, char* fileName, double* matrix)
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
    matrix = malloc(sizeof(double)*(dim * dim));
    
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
        matrix[count] = strtod(pStart, &pEnd);
        // Increment number of elements read
        ++count;
        // We wish to skip commas (or any deliminator really)
        pStart = pEnd + 1;
    }
}

