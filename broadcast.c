#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int getSuccesseur(int rank, int numprocs);
int getPredecesseur(int rank, int numprocs);
void afficherTableau(int* tab, int length);
// int MPI_Bcast( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm )
// COMPILE : mpicc broadcast.c -o bcast
// RUN : mpirun ./bcast --oversubscribe -np 5
void main(int argc, char* argv[]){
  int numprocs, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int master = 4;
  int * dataSent = malloc(sizeof(int) * 4);
  *dataSent = 77;
  int * dataReceived;
  MPI_Status status;


  MPI_Bcast(dataSent,1,MPI_INT,master,MPI_COMM_WORLD);

  if (rank == master ){
    *dataSent = 1;
  }
  printf("rank : %d, received : %d \n",rank,*dataSent);






  MPI_Finalize();
}

int getSuccesseur(int rank, int numprocs){
    return (rank + 1) % numprocs;
}

int getPredecesseur(int rank, int numprocs){
    return (rank - 1 + numprocs) % numprocs;
}

void afficherTableau(int* tab, int length){

    for(int i = 0; i < length; i++){
        printf("data[%d] = %d\n", i, tab[i]);
    }
}
