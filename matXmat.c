#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef max
	#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

int getSuccesseur(int rank, int numprocs);
int getPredecesseur(int rank, int numprocs);
struct matrix * allocateMatrix(int nblin, int nbCol);
void generateMatrix(struct matrix * s, int size);
void printMatrix(struct matrix * s);
void rotateMatrix(struct matrix * s, struct matrix * dest);
void produitMat(struct matrix * A,struct matrix * B,struct matrix * C);
void generateVector(struct matrix * s,int size);

// int MPI_Bcast( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm )

/*
  MPI_Scatter( void* send_data,  int send_count,  MPI_Datatype send_datatype,  void* recv_data,
              int recv_count,  MPI_Datatype recv_datatype,  int root,  MPI_Comm communicator)

 MPI_Gather(void* send_data, int send_count, MPI_Datatype send_datatype, void* recv_data,
    int recv_count, MPI_Datatype recv_datatype, int root, MPI_Comm communicator)
*/

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
	int master = 0;
  int * chunkSize = malloc (sizeof(int) * 1);

  struct matrix sourceA, finalC;

	// donner master toute les donnees
	if (rank == master){
		// TODO: implementer lecture fichier entree
		generateMatrix(&sourceA,numprocs);
    *chunkSize = sourceA.nbColonnes * sourceA.nbLignes / numprocs;
  //  finalC = allocateMatrix(sourceA.nbLignes,sourceA.nbColonnes);
    generateMatrix(&finalC, numprocs);
  }

  MPI_Bcast(chunkSize,1,MPI_INT,master,MPI_COMM_WORLD);
  printf("rank : %d, received : %d \n",rank,*chunkSize);

  struct matrix *localA = allocateMatrix(1,*chunkSize);

  MPI_Scatter(sourceA.mat,*chunkSize,MPI_INT,localA->mat,*chunkSize,MPI_INT,master,MPI_COMM_WORLD);


  // compute calculations


//  struct matrix *localB;
//  generateVector(localB,localA->nbColonnes);
//	struct matrix *C = allocateMatrix(A.nbLignes,B->nbColonnes);

//	produitMat(localA, localB ,&finalC);
  //printMatrix(C);

  // gather at master
  MPI_Gather (localA->mat,*chunkSize,MPI_INT, finalC.mat,*chunkSize,MPI_INT,master,MPI_COMM_WORLD);
  if (rank ==master){
    printMatrix(&finalC);
  }




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
  tmp->mat = calloc(nblin * nbCol, sizeof(int));
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

void generateVector(struct matrix * s,int size){
	s->nbLignes = size;
	s->nbColonnes = 1;
	int n = s->nbColonnes * s->nbLignes;
	s->mat=malloc (n * sizeof(int));
  for (int i = 0; i < n; i ++){
    s->mat[i] = i+1;
  }
}

void printMatrix(struct matrix * s){
  int taille = s->nbColonnes * s->nbLignes;
  printf("---- Matrix of %i lanes & %i columns---- \n", s->nbLignes,s->nbColonnes);
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
