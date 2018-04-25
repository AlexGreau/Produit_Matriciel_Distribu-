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
void generate0Matrix(struct matrix * s, int size);

void printMatrix(struct matrix * s);
void rotateMatrix(struct matrix * s, struct matrix * dest);
void produitMat(struct matrix * A,struct matrix * B,struct matrix * C);
void generateVector(struct matrix * s,int size);

void allocateMat(struct matrix * tmp,int nblin, int nbCol);

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
	int numprocs, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int master = 0;
  int * chunkSize = malloc (sizeof(int) * 1);
  struct matrix * sourceA = malloc (sizeof(struct matrix));
	struct matrix * sourceB = malloc (sizeof(struct matrix));
	struct matrix * finalC = malloc (sizeof(struct matrix));

	// donner master toute les donnees
	if (rank == master){
		// TODO: implementer lecture fichier entree
		generateMatrix(sourceA,numprocs);
    *chunkSize = sourceA->nbColonnes * sourceA->nbLignes / numprocs;
    finalC = allocateMatrix(sourceA->nbLignes,sourceA->nbColonnes);
		generateMatrix(sourceB,numprocs);
  }

  MPI_Bcast(chunkSize,1,MPI_INT,master,MPI_COMM_WORLD);
  printf("rank : %d, received : %d \n",rank,*chunkSize);

	// si n = numprocs
  struct matrix * localA = malloc(sizeof(struct matrix)); // ne pas oublier de malloc
	struct matrix * localB = malloc (sizeof(struct matrix));
	struct matrix * localC = malloc (sizeof(struct matrix));

// car n = p
	localA = allocateMatrix (1,*chunkSize);
	localB = allocateMatrix(*chunkSize,1);
  MPI_Scatter(sourceA->mat,*chunkSize,MPI_INT,localA->mat,*chunkSize,MPI_INT,master,MPI_COMM_WORLD);
	// scatter B
	MPI_Scatter(sourceB->mat, *chunkSize,MPI_INT,localB->mat, *chunkSize, MPI_INT,master,MPI_COMM_WORLD);


  // calculations

	localC = allocateMatrix(localA->nbLignes,localB->nbColonnes);

	produitMat(localA, localB ,localC);

	// transmit chunk of B

  // gather at master
  MPI_Gather (localC->mat,*chunkSize,MPI_INT, finalC->mat,*chunkSize,MPI_INT,master,MPI_COMM_WORLD);
  if (rank == master){
    //printMatrix(finalC);
		printMatrix(localB);
  }

	MPI_Finalize();
}







//__________________functions____________________

void produitMat(struct matrix * A,struct matrix * B,struct matrix * C){
  for (int i = 0; i < A->nbLignes; i ++){ // ligne
    for (int j = 0; j < B->nbColonnes; j++){ // colonne
      for (int k = 0 ; k < max (C->nbLignes,C->nbColonnes); k++){
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

void allocateMat(struct matrix * tmp,int nblin, int nbCol){
	tmp->nbColonnes = nbCol;
	tmp->nbLignes = nblin;
	tmp->mat = calloc(nblin * nbCol, sizeof(int));
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
void generate0Matrix(struct matrix * s, int size){
	int n = size;
  s->nbLignes = n;
  s->nbColonnes = n;
  s->mat=malloc(n*n *sizeof(int));
  for (int i = 0; i < n * n; i ++){
		s->mat[i] = 0;
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
