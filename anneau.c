#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int getSuccesseur(int rank, int numprocs);
int getPredecesseur(int rank, int numprocs);
void afficherTableau(int* tab, int length);

int main(int argc, char* argv[]){
    int numprocs, rank, p;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    p = 0;
    int * data = malloc(sizeof(int) * numprocs);
    MPI_Status status;

    /* Je suis l'émetteur
     * On suppose que le premier est le processus 0,
     * on pourra par la suite le mettre en argument (variable p)
     */
    if(rank == p) {
        /* L'émetteur envoie l'information en premier, tandis que tous les autres recoivent d'abord.
         * Puis il recoit le dernier message
         */

        /* On remplit notre tableau avec la date */
        data[rank] = (int)time(NULL);

        MPI_Send(data, numprocs, MPI_INT, getSuccesseur(rank, numprocs), 0, MPI_COMM_WORLD);
        MPI_Recv(data, numprocs, MPI_INT, getPredecesseur(rank, numprocs), 0, MPI_COMM_WORLD, &status);

        afficherTableau(data, numprocs);

    } else {

        MPI_Recv(data, numprocs, MPI_INT, getPredecesseur(rank, numprocs), 0, MPI_COMM_WORLD, &status);

        /* On remplit notre tableau avec la date */
        data[rank] = (int)time(NULL);
        MPI_Send(data, numprocs, MPI_INT, getSuccesseur(rank, numprocs), 0, MPI_COMM_WORLD);
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
