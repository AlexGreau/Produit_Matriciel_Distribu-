#include <stdio.h>
#include <stdlib.h>



void generateMatrix(struct matrix * s, int size);

struct matrix {
  // lineariser
 	long * mat;
  int nbColonnes;
  int nbLignes;
};

int main(int argc, char const *argv[]) {
	/* code */
	return 0;
}
void generateMatrix(struct matrix * s, int size) {
  //construction d'un tableau pour tester
  int n = 64;
  s->nbLignes = n;
  s->nbColonnes = n;
  s->mat=malloc(n*n *sizeof(long));
  for (int i = 0; i < n * n; i ++){
    s->mat[i] = (i + 1) % n;
  }
}
