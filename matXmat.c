#define _GNU_SOURCE
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#ifndef max
	#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

int getSuccesseur(int rank, int numprocs);
int getPredecesseur(int rank, int numprocs);
struct matrix * allocateMatrix(int nblin, int nbCol);
void generateMatrix(struct matrix * s, int size);
void generate0Matrix(struct matrix * s, int size);

void printMatrix(struct matrix * s);
void transposeMatrix(struct matrix * s);
void rotateMatrix(struct matrix * s);
void produitMat(struct matrix * A,struct matrix * B,struct matrix * C);
void generateVector(struct matrix * s,int size);

void allocateMat(struct matrix * tmp,int nblin, int nbCol);
int nfinder(char* file);
struct matrix * input(char * file, int n);
void transferInto(struct matrix* source, struct matrix * dest, int tour, int rank,int n);

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
	MPI_Status status;


	int master = 0;

  int * chunkSize = malloc (sizeof(int) * 1);
	int * n = malloc (sizeof(int) *1);
  struct matrix * sourceA = malloc (sizeof(struct matrix));
	struct matrix * sourceB = malloc (sizeof(struct matrix));
	struct matrix * finalC  = malloc (sizeof(struct matrix));

	// donner master toute les donnees
	if (rank == master){
		*n = nfinder(argv[1]);
		sourceA = input (argv[1],*n);
    finalC = allocateMatrix(sourceA->nbLignes,sourceA->nbColonnes);
		sourceB = input (argv[2],*n);
		transposeMatrix(sourceB);
	}

	MPI_Bcast(n,1,MPI_INT,master,MPI_COMM_WORLD);

	// si n = numprocs
  struct matrix * localA = malloc (sizeof(struct matrix));
	struct matrix * localB = malloc (sizeof(struct matrix));
	struct matrix * localC = malloc (sizeof(struct matrix));
	struct matrix * localTemp = malloc (sizeof(struct matrix));

	*chunkSize = *n / numprocs;
	// car n = p
	localA = allocateMatrix (*chunkSize, *n);
	localB = allocateMatrix (*n, *chunkSize);
	localTemp = allocateMatrix(*chunkSize,*chunkSize);
	int * localBtemp = malloc(sizeof (int) * localB->nbColonnes * localB->nbLignes);


  MPI_Scatter(sourceA->mat,*chunkSize * *n,MPI_INT,localA->mat,*chunkSize * *n,MPI_INT,master,MPI_COMM_WORLD);
	MPI_Scatter(sourceB->mat, *chunkSize * *n,MPI_INT,localB->mat, *chunkSize * *n, MPI_INT,master,MPI_COMM_WORLD);
//	rotateMatrix(localB);

	localC = allocateMatrix(localA->nbLignes,*n);
	int Successeur = (rank + 1) % numprocs;
	int Predecesseur = (rank - 1 + numprocs) % numprocs;
	// transmit chunk of B
	for (int tour = 0; tour < numprocs - 1;tour++){
		// do job
		produitMat(localA, localB ,localTemp);
		transferInto(localTemp,localC,tour,rank,*n);
		// end job
		if (rank != master){
			MPI_Send (localB->mat, localB->nbLignes * localB->nbColonnes, MPI_INT, Successeur, tour , MPI_COMM_WORLD);
			MPI_Recv (localB->mat, localB->nbLignes * localB->nbColonnes, MPI_INT, Predecesseur, tour , MPI_COMM_WORLD, &status);
		}
		else {
			memcpy(localBtemp,localB->mat,sizeof (int) * localB->nbColonnes * localB->nbLignes);

			MPI_Recv (localB->mat, localB->nbLignes * localB->nbColonnes, MPI_INT, Predecesseur, tour , MPI_COMM_WORLD, &status);
			MPI_Send (localBtemp, localB->nbLignes * localB->nbColonnes, MPI_INT, Successeur, tour , MPI_COMM_WORLD);
		}
	}
	// once all finished
  // gather at master
  MPI_Gather (localC->mat,localC->nbLignes *localC->nbColonnes ,MPI_INT, finalC->mat,localC->nbLignes *localC->nbColonnes,MPI_INT,master,MPI_COMM_WORLD);

	/* debug / verif */
	if (rank == master ){
	//	printMatrix(localA);
		printMatrix(finalC);
	//	printMatrix(localB);

  }

	MPI_Finalize();
}







//__________________functions____________________

void produitMat(struct matrix * A,struct matrix * B,struct matrix * C){
	int n = A->nbColonnes;
	#pragma omp parallel for
  for (int i = 0; i < A->nbLignes; i ++){ // ligne
    for (int j = 0; j < B->nbColonnes; j++){ // colonne
      for (int k = 0 ; k < n; k++){
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

void transposeMatrix(struct matrix * s){
	// pour recuperer les colonnes
  struct matrix * rotated = allocateMatrix(s->nbColonnes,s->nbLignes);
  for (int l = 0; l < s->nbLignes; l++){
    for (int c = 0; c < s->nbColonnes; c++){
      rotated->mat[ l * s->nbColonnes + c] = s->mat[ c * s->nbLignes + l];
    }
  }
	int n;
	n = s->nbColonnes;
	s->nbColonnes = s->nbLignes;
	s->nbLignes = n;
	*s = *rotated;
}

void rotateMatrix(struct matrix * s){
	struct matrix * rotated = allocateMatrix(s->nbColonnes,s->nbLignes);
	for (int i = 0; i < s->nbLignes; i++){
		for (int j = 0; j < s->nbColonnes; j ++){
			// A [i,j] => A [j,i]
			rotated->mat[j*s->nbLignes + i] = s->mat[i * s->nbColonnes + j];
		}
	}
	int n;
	n = s->nbColonnes;
	s->nbColonnes = s->nbLignes;
	s->nbLignes = n;
	*s = *rotated;
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


int nfinder(char* file){
	FILE* fichier = NULL;
	fichier = fopen(file, "r");
	if(fichier == NULL){
		fprintf(stderr, "The file \"%s\" doesn't exist.\n", file);
		exit(EXIT_FAILURE);
	}else { // procedure normale
		char * line = NULL;
		char * token;
		size_t len = 0;
		int n = 0;

		getline(&line, &len, fichier);

		token = strtok(line, " ");

	  while(token != NULL){
			  token = strtok(NULL, " ");
	      n++;
	  }
		return n;
	}
}

struct matrix * input(char * file, int n){
	struct matrix *source = malloc (sizeof(struct matrix));
	source = allocateMatrix(n,n);
	char * line = NULL;
	size_t len = 0;
	FILE* fichier = NULL;
	fichier = fopen(file, "r");
	if(fichier == NULL){
		fprintf(stderr, "The file \"%s\" doesn't exist.\n", file);
		exit(EXIT_FAILURE);
	}

	char * token;

	for (int i = 0; i < n; i++){
		getline(&line,&len, fichier);
		token = strtok(line, " ");

		for (int j = 0; j < n; j++){
			// tokeniser et passer au suivant
			int value = atoi(token);
			source->mat[i*n+j] = value;
			token = strtok(NULL, " ");
		}
	}
	fclose(fichier);
	return source;
}

void transferInto(struct matrix* source, struct matrix * dest, int tour, int rank,int n){
	int offset = ((tour +rank)*source->nbColonnes)%n;
	for (int i =0; i < source->nbLignes; i ++){
		for (int j = 0 ; j < source->nbColonnes; j ++ ){
			dest->mat[offset + j + i*dest->nbColonnes] = source->mat[j + i *source->nbColonnes];
		}
	}
}
