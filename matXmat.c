// #include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int getSuccesseur(int rank, int numprocs);
int getPredecesseur(int rank, int numprocs);
struct matrix * allocateMatrix(int nblin, int nbCol);
void generateMatrix(struct matrix * s);
void printMatrix(struct matrix * s);

// int MPI_Bcast( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm )
// COMPILE : mpicc matXmat.c -o mXm
// RUN : mpirun --oversubscribe -np 5 mXm

struct matrix {
  // lineariser
  int * mat;
  int nbColonnes;
  int nbLignes;
};

int main(int argc, char* argv[]){
  struct matrix A;
// struct matrix *B = allocateMatrix(3,3);
//  struct matrix *C = allocateMatrix(3,3);

  generateMatrix(&A);
  printMatrix(&A);
}

struct matrix * allocateMatrix(int nblin, int nbCol) {
  struct matrix * tmp = malloc(sizeof(struct matrix));
  tmp->nbColonnes = nbCol;
  tmp->nbLignes = nblin;
  tmp->mat = malloc(nblin * nbCol *sizeof(int));
  return tmp;
}

void generateMatrix(struct matrix * s) {
  //construction d'un tableau pour tester
  int n = 3;
  s->nbLignes = n;
  s->nbColonnes = n;
  s->mat=malloc(n*n *sizeof(int));
  s->mat[0] = 1;
  s->mat[1] = 2;
  s->mat[2] = 3;

  s->mat[3] = 4;
  s->mat[4] = 5;
  s->mat[5] = 6;

  s->mat[6] = 7;
  s->mat[7] = 8;
  s->mat[8] = 9;
}

void printMatrix(struct matrix * s){
  int taille = s->nbColonnes * s->nbLignes;
  printf("---- Matrix of size %i ---- \n", taille);
  for (int i = 0; i < s->nbLignes; i++){
    for (int j = 0; j < s->nbColonnes; j++){
      printf("%d ", s->mat[i*s->nbColonnes + j]);
    }
    printf ("\n");
  }
}

int getSuccesseur(int rank, int numprocs){
    return (rank + 1) % numprocs;
}

int getPredecesseur(int rank, int numprocs){
    return (rank - 1 + numprocs) % numprocs;
}
