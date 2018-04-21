#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef max
	#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

int getSuccesseur(int rank, int numprocs);
int getPredecesseur(int rank, int numprocs);
struct matrix * allocateMatrix(int nblin, int nbCol);
void generateMatrix(struct matrix * s);
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
	// donner au master toute la matrice
	// scatter matrices : (quelle taille attendre ? envoyer taille au prealable ?)
	// chaque matrice fait son calcul
	// rassembler les resultats
	// print
	int numprocs, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int master = 1;
  int * chunkSize = malloc (sizeof(int) * 1);

	int nblA,nbca,nblB,nbcB;
  struct matrix A;

	// donner master toute les donnees
	if (rank == master){
		// TODO: implementer lecture fichier entree
		generateMatrix(&A);
    *chunkSize = A.nbColonnes / numprocs;
  }

  MPI_Bcast(chunkSize,1,MPI_INT,master,MPI_COMM_WORLD);
  printf("rank : %d, received : %d \n",rank,*chunkSize);

  // scatter
  /*MPI_Scatter(
    void* send_data,
    int send_count,
    MPI_Datatype send_datatype,
    void* recv_data,
    int recv_count,
    MPI_Datatype recv_datatype,
    int root,
    MPI_Comm communicator)
*/

	// recevoir tailles pour init la mat de resultats (taille A,taille B)
/*
 	struct matrix *B = allocateMatrix(3,3);

  MPI_Scatter(&A, nblA/numprocs, MPI_INT, &A,nblA/numprocs,MPI_INT,master,MPI_COMM_WORLD);
*/
/*
	struct matrix *C = allocateMatrix(A.nbLignes,B->nbColonnes);

	produitMat(&A, B ,C);
  printMatrix(C);
*/
	MPI_Finalize();
}







//__________________functions____________________

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
