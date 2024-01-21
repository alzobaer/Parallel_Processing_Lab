#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
 int rank, n_ranks, numbers_per_rank;
 char send_message[40], *receive_message;

 // First call MPI_Init
 MPI_Init(&argc, &argv);
 // Get my rank and the number of ranks
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

 // Allocate space for all received messages in receive_message
 receive_message = malloc( n_ranks*40*sizeof(char) );

 //Use gather to send all messages to rank 0
 sprintf(send_message, "Hello World, I'm rank %d\n", rank);
 MPI_Gather( send_message, 40, MPI_CHAR, receive_message, 40, MPI_CHAR, 0, MPI_COMM_WORLD );
 
 if(rank == 0){
    for( int i=0; i<n_ranks; i++){
      printf("%s", receive_message + i*40);
    }
 }
 
 // Free memory and finalise
 free( receive_message );
 return MPI_Finalize();
}