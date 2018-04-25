#include <stdio.h>
#include <stdlib.h>
#ifndef max
	#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

int getSuccesseur(int rank, int numprocs);
int getPredecesseur(int rank, int numprocs);
struct matrix * allocateMatrix(int nblin, int nbCol);
void generateAMatrix(struct matrix * s);
void generateBMatrix(struct matrix * s);
void generateMatrix(struct matrix * s, int size);
void printMatrix(struct matrix * s);
void rotateMatrix(struct matrix * s, struct matrix * dest);
void produitMat(struct matrix * A,struct matrix * B,struct matrix * C);
void generateVector(struct matrix * s);

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
 struct matrix *B = allocateMatrix(3,3);

  generateMatrix(&A,3);
	generateMatrix(B,3);
	struct matrix *C = allocateMatrix(A.nbLignes,B->nbColonnes);
  printMatrix(&A);

	printMatrix(B);

	produitMat(&A, B ,C);
  printMatrix(C);
}

/*
---- Matrix of size 9 ----
30 36 42
66 81 96
102 126 150
*/







//__________________functions____________________
/*

void produitMat(struct matrix * A,struct matrix * B,struct matrix * C){
  for (int i = 0; i < C->nbLignes; i ++){ // ligne
    for (int j = 0; j < C->nbColonnes; j++){ // colonne
      for (int k = 0 ; k < C->nbLignes; k++){
        // C[i,j] = C[i,j] + A[i,k] * B [k,j]
				C->mat[i*C->nbColonnes + j] = C->mat[i*C->nbColonnes + j] +	A->mat[i*A->nbColonnes + k] * B->mat[k*B->nbColonnes + j];
      }
    }
  }
}
*/
void produitMat(struct matrix * A,struct matrix * B,struct matrix * C){
  for (int i = 0; i < A->nbColonnes; i ++){ // ligne
    for (int j = 0; j < B->nbLignes; j++){ // colonne
      for (int k = 0 ; k < B->nbLignes; k++){
        // C[i,j] = C[i,j] + A[i,k] * B [k,j]
				C->mat[i*C->nbColonnes + j] = C->mat[i*C->nbColonnes + j] +	A->mat[i*A->nbColonnes + k] * B->mat[k*B->nbColonnes + j];
      }
    }
  }
}
struct matrix * allocateMatrix(int nblin, int nbCol) {
  struct matrix * tmp = malloc(sizeof(struct matrix));
  tmp->nbColonnes = nbCol;
  tmp->nbLignes = nblin;
  tmp->mat = malloc(nblin * nbCol *sizeof(int));
  return tmp;
}

void rotateMatrix(struct matrix * s, struct matrix * dest){
  struct matrix * rotated = allocateMatrix(s->nbLignes, s->nbColonnes);
  int n = s->nbLignes;
  for (int l = 0; l < n; l++){
    for (int c = 0; c < n; c++){
      rotated->mat[ l * n + c] = s->mat[ c * n + l];
    }
  }
  dest->mat = rotated->mat;
  free(rotated);
}

void generateAMatrix(struct matrix * s) {
  //construction d'un tableau pour tester
  int n = 3;
  s->nbLignes = n;
  s->nbColonnes = n;
  s->mat=malloc(n*n *sizeof(int));
  s->mat[0] = 1;
  s->mat[1] = 0;
  s->mat[2] = -1;

  s->mat[3] = 4;
  s->mat[4] = 10;
  s->mat[5] = 7;

  s->mat[6] = 2;
  s->mat[7] = 13;
  s->mat[8] = 5;
}

void generateBMatrix(struct matrix * s) {
  //construction d'un tableau pour tester
  int n = 3;
  s->nbLignes = n;
  s->nbColonnes = n;
  s->mat=malloc(n*n *sizeof(int));
  s->mat[0] = 93;
  s->mat[1] = 23;
  s->mat[2] = 76;

  s->mat[3] = 18;
  s->mat[4] = 106;
  s->mat[5] = 1;

  s->mat[6] = 3;
  s->mat[7] = 10;
  s->mat[8] = 2;
}

void generateMatrix(struct matrix * s, int size) {
  //construction d'un tableau pour tester
  int n = size;
  s->nbLignes = n;
  s->nbColonnes = n;
  s->mat=malloc(n*n *sizeof(int));
  for (int i = 0; i < n * n; i ++){
    s->mat[i] = i + 1;
  }
}

void generateVector(struct matrix * s){
	s->nbLignes = 3;
	s->nbColonnes = 1;
	int n = s->nbColonnes * s->nbLignes;
	s->mat=malloc (n * sizeof(int));
	s->mat[0] = 1;
  s->mat[1] = 2;
  s->mat[2] = 3;
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
