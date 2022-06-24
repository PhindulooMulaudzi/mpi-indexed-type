#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int rank, size, i;
	
	// create  MPI type
    MPI_Datatype type;
	
	// Below are the parameters of our indexed type
	/*
		displacement specifies where we start the index
		blocklen specifics the length of values to take
	*/
    int blocklen[3] = { 2, 3, 1 };
    int displacement[3] = { 0, 3, 8 };
    int buffer[10];
    MPI_Status status; // just status for the recieve

	// start MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
	// just assert that we have 2 processors
	if (size < 2)
    {
        printf("Please run with 2 processes.\n");
        MPI_Finalize();
        return 1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	// crux of this lesson, we create our indexed type with the defined params
    MPI_Type_indexed(3, blocklen, displacement, MPI_INT, &type);
    // we commit the type after it is created then its ready for use
	MPI_Type_commit(&type);
	
	// use rank zero to create a buffer and pass the type
	// this will only send the data specified by displacement and block length
    if (rank == 0)
    {
        for (i=0; i<10; i++)
            buffer[i] = i;
        MPI_Send(buffer, 1, type, 1, 123, MPI_COMM_WORLD);
    }

    if (rank == 1)
    {
		// lets fill the buffer with -1 so that we can clearly see
		// the indices of what we recive from rank 0
        for (i=0; i<10; i++)
            buffer[i] = -1;
        MPI_Recv(buffer, 1, type, 0, 123, MPI_COMM_WORLD, &status);
        
		// voilà! out buffer has been overwritten as specified by type!
		for (i=0; i<10; i++)
            printf("buffer[%d] = %d\n", i, buffer[i]);
        fflush(stdout);
    }

    MPI_Finalize();
    return 0;
}
