#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int getSuccesseur(int rank, int numprocs);
int getPredecesseur(int rank, int numprocs);
void afficherTableau(int* tab, int length);
// int MPI_Bcast( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm )
// COMPILE : mpicc all2all.c -o a2a
// RUN : mpirun --oversubscribe -np 5 a2a
void main(int argc, char* argv[]){
  int numprocs, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int master = 4;
  int * dataSent = calloc(sizeof(int) * numprocs,9);
  dataSent [rank] = rank;
  MPI_Status status;

  for (int i = 0 ; i < numprocs; i ++){
    MPI_Bcast(&dataSent[i],1,MPI_INT,i,MPI_COMM_WORLD);
  }

  if (rank == 3 ) {
    printf(" tableau du rank %d \n", rank);
    afficherTableau(dataSent,numprocs);
  }



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
