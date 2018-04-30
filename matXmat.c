#include <mpi.h>
#include <stdio.h>
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
void rotateMatrix(struct matrix * s);
void produitMat(struct matrix * A,struct matrix * B,struct matrix * C);
void generateVector(struct matrix * s,int size);

void allocateMat(struct matrix * tmp,int nblin, int nbCol);

void input( struct matrix * source, char* file,int* n);

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
	int * n = malloc (sizeof(int) *1);
  struct matrix * sourceA = malloc (sizeof(struct matrix));
	struct matrix * sourceB = malloc (sizeof(struct matrix));
	struct matrix * finalC  = malloc (sizeof(struct matrix));

	// donner master toute les donnees
	if (rank == master){
		// TODO: implementer lecture fichier entree
		input(sourceA,argv[1],n);
		generateMatrix(sourceA,numprocs*2);
		*n = sourceA->nbLignes;
    finalC = allocateMatrix(sourceA->nbLignes,sourceA->nbColonnes);
		generateMatrix(sourceB,numprocs * 2);
		rotateMatrix(sourceB);
	}

	MPI_Bcast(n,1,MPI_INT,master,MPI_COMM_WORLD);
	*chunkSize = *n / numprocs;

  printf("rank : %d, received : %d \n",rank,*chunkSize);

	// si n = numprocs
  struct matrix * localA = malloc (sizeof(struct matrix));
	struct matrix * localB = malloc (sizeof(struct matrix));
	struct matrix * localC = malloc (sizeof(struct matrix));

// car n = p
	localA = allocateMatrix (*chunkSize, *n);
	localB = allocateMatrix (*n, *chunkSize);
  MPI_Scatter(sourceA->mat,*chunkSize * *n,MPI_INT,localA->mat,*chunkSize * *n,MPI_INT,master,MPI_COMM_WORLD);
	MPI_Scatter(sourceB->mat, *chunkSize * *n,MPI_INT,localB->mat, *chunkSize * *n, MPI_INT,master,MPI_COMM_WORLD);
	rotateMatrix(localB);
  // calculations
	localC = allocateMatrix(localA->nbLignes,localB->nbColonnes);
	produitMat(localA, localB ,localC);

	// transmit chunk of B

  // gather at master
  MPI_Gather (localC->mat,*chunkSize,MPI_INT, finalC->mat,*chunkSize,MPI_INT,master,MPI_COMM_WORLD);
  if (rank == master +1){
		/*
		printMatrix(localA);
		printMatrix(localB);
    printMatrix(localC);
		*/
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

void rotateMatrix(struct matrix * s){
  struct matrix * rotated = allocateMatrix(s->nbColonnes,s->nbLignes);
  for (int l = 0; l < s->nbLignes; l++){
    for (int c = 0; c < s->nbColonnes; c++){
      rotated->mat[ l * s->nbColonnes + c] = s->mat[ c * s->nbLignes + l];
    }
  }
	*s = *rotated;
	int n;
	n = s->nbColonnes;
	s->nbColonnes = s->nbLignes;
	s->nbLignes = n;
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


void input(struct matrix* source, char* file,int* n){
	FILE* fichier = NULL;
	fichier = fopen(file, "r");
	if(fichier == NULL){
		fprintf(stderr, "The file \"%s\" doesn't exist.\n", file);
		exit(EXIT_FAILURE);
	}else {
		char * line = NULL;
		char * token;
		ssize_t read;
		size_t len = 0;
		*n = 0;

		read = getline(&line, &len, fichier);
		printf("Réception d'une ligne de longueur %zu :\n", read);
		printf("%s\n", line);
		char temp;
		token = strtok(line, " ");

	  while(token != NULL){
				//printf("%s\n", line);
				printf("%s\n", token);
			  token = strtok(NULL, " ");
	      //scanf("%hh%c ", line, &temp);
	      *n++;
	  }

		printf("%d\n", *n);


	}
}
